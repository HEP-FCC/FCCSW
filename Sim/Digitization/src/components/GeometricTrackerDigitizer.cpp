#include "GeometricTrackerDigitizer.h"
#include "ACTS/Detector/DetectorElementBase.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/PlanarModuleStepper.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Surfaces/PlanarBounds.hpp"
#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "DD4hep/Segmentations.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "Digitization/FCCDigitizationCell.h"
#include "RecTracker/ACTSLogger.h"
#include "datamodel/DigiTrackHitAssociationCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackCluster.h"
#include "datamodel/TrackClusterCollection.h"
#include "datamodel/TrackHitCollection.h"
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

DECLARE_ALGORITHM_FACTORY(GeometricTrackerDigitizer)

GeometricTrackerDigitizer::GeometricTrackerDigitizer(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc),
      m_geoSvc("GeoSvc", "GeometricTrackerDigitizer"),
      m_trkGeoSvc("TrackingGeoSvc", "GeometricTrackerDigitizer"),
      m_detectorElements() {
  declareProperty("trackingGeometrySvc", m_trkGeoSvc, "The geometry service providing the tracking geometry");
  declareProperty("geoSvc", m_geoSvc, "The geometry service providing the tracking geometry");
  declareProperty("digiTrackHitAssociation", m_digiTrackHitAssociation, "Handle for input digiTrackHitAssociation");
  declareProperty("trackClusters", m_trackClusters, "Handle for output clusters");
  declareProperty("clusterTrackHits", m_trackHits, "Handle for output hits belonging to the clusters");
  declareProperty("planarClusters", m_planarClusterHandle,
                  "Handle to sim::FCCPlanarCluster needed to possibly write out additional information for studies");
}

StatusCode GeometricTrackerDigitizer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();

  if (!m_geoSvc) {
    error() << "Did not retrieve tgeometry service!" << endmsg;
    sc = StatusCode::FAILURE;
  }
  if (!m_trkGeoSvc) {
    error() << "Did not retrieve tracking geometry service!" << endmsg;
    sc = StatusCode::FAILURE;
  }

  // Initialize random service
  if (service("RndmGenSvc", m_randSvc).isFailure()) {
    error() << "Could not get RndmGenSvc!" << endmsg;
    return StatusCode::FAILURE;
  }
  // initialize gauss and flat distribution
  sc = m_gauss.initialize(m_randSvc, Rndm::Gauss(0., 1.));
  sc = m_flatDist.initialize(m_randSvc, Rndm::Flat(0., 1.));

  // set up the planar module stepper
  Acts::PlanarModuleStepper::Config pmStepperConfig;
  ServiceHandle<IMessageSvc> msgSvc1("MessageSvc", "PlanarStepper");
  auto GaudiLogger = std::make_unique<Acts::Logger>(std::make_unique<GaudiPrintPolicy>(&(*msgSvc1)),
                                                    std::make_unique<GaudiFilterPolicy>(&(*msgSvc1)));
  m_moduleStepper = std::make_shared<Acts::PlanarModuleStepper>(pmStepperConfig, std::move(GaudiLogger));

  if (!m_moduleStepper) {
    error() << "Could not create planar module stepper!" << endmsg;
    sc = StatusCode::FAILURE;
  }

  // Access the detector elements of the tracker
  m_detectorElements = m_trkGeoSvc->trackingGeometry()->highestTrackingVolume()->detectorElements();
  if (m_detectorElements.empty()) {
    error() << "Could not access detector elements!" << endmsg;
    sc = StatusCode::FAILURE;
  }
  // setup volume manager
  m_volumeManager = m_geoSvc->lcdd()->volumeManager();
  if (!m_volumeManager) {
    error() << "Could not retrieve dd4hep volume manager!" << endmsg;
    sc = StatusCode::FAILURE;
  }
  return sc;
}

StatusCode GeometricTrackerDigitizer::execute() {

  // Retrieve the input hits
  const fcc::DigiTrackHitAssociationCollection* hits = m_digiTrackHitAssociation.get();
  // the track hits of the cluster
  auto trackHits = m_trackHits.createAndPut();
  // prepare the output clusters
  auto trackClusters = m_trackClusters.createAndPut();
  // the cells to be used
  std::map<size_t, std::vector<sim::FCCDigitizationCell>> cellsPerSurface;
  // go through hits
  for (auto hit : (*hits)) {
    // the cell ID
    long long int cellID = hit.hit().core().cellId;
    // throw a certain number of hits away if configured
    if (m_hitInefficiency != 0. && m_flatDist() < m_hitInefficiency) continue;
    // get the corresponding dd4hep detelement
    auto dd4hepDetElement = m_volumeManager.lookupDetElement(cellID);
    if (!dd4hepDetElement) {
      error() << "DD4hep detector element with channel identifier: " << cellID << "not valid!" << endmsg;
      return StatusCode::FAILURE;
    }
    // get the surface ID
    long long int surfaceID = dd4hepDetElement.volumeID();

    // access the detector element corresponding to the hit
    auto search = m_detectorElements.find(Identifier(surfaceID));
    if (search == m_detectorElements.end()) {
      error() << "Acts Detector element with identifier: " << surfaceID << " and cellID: " << cellID << " not found!"
              << endmsg;
      return StatusCode::FAILURE;
    }
    auto hitDetElement = search->second;
    if (!hitDetElement) {
      error() << "Invalid Acts Detector element with identifier: " << surfaceID << "!" << endmsg;
      return StatusCode::FAILURE;
    }
    // access the surface corresponding to the detector element
    const Acts::Surface& hitSurface = hitDetElement->surface();

    auto hitDigitizationModule = hitDetElement->digitizationModule();
    if (!hitDigitizationModule) {
      error() << "No digitization module attached to detector element with identifier: " << surfaceID << "!" << endmsg;
      return StatusCode::FAILURE;
    }
    if (hitDigitizationModule) {
      // get the lorentz angle
      double lorentzAngle = hitDigitizationModule->lorentzAngle();
      double thickness = hitDetElement->thickness();
      double lorentzShift = thickness * tan(lorentzAngle);
      lorentzShift *= -(hitDigitizationModule->readoutDirection());
      // get the hit position
      Acts::Vector3D globalIntersection(hit.hit().position().x, hit.hit().position().y, hit.hit().position().z);
      // access the post step position to calculate the direction
      Acts::Vector3D postStepPosition(hit.postStepPosition().x, hit.postStepPosition().y, hit.postStepPosition().z);
      // calculate the momentum direction & intersection point in local frame
      Acts::Vector3D localDirection = (postStepPosition - globalIntersection).unit();
      // transform direction into local frame
      localDirection = hitSurface.transform().inverse().linear() * localDirection;
      // to ignore extreme incident angles
      if (std::abs(localDirection.z()) < m_cosThetaLocMin) continue;

      // The digitization steps
      std::vector<Acts::DigitizationStep> dSteps;
      if (m_fastSimInterface) {
        // using fast sim interface
        // calculate local intersection point
        Acts::Vector2D localIntersection(0., 0.);
        hitSurface.globalToLocal(globalIntersection, localDirection, localIntersection);
        // calculate the digitization steps
        dSteps = m_moduleStepper->cellSteps(*hitDigitizationModule, localIntersection, localDirection.unit());
      } else {
        // full sim interface
        // calculate position in local frame
        auto positionLocalFrame = hitSurface.transform().inverse() * globalIntersection;
        // calculate post position in local frame
        auto postPositionLocalFrame = hitSurface.transform().inverse() * postStepPosition;
        // calculate the digitization steps
        std::vector<Acts::DigitizationStep> dSteps =
            m_moduleStepper->cellSteps(*hitDigitizationModule, positionLocalFrame, postPositionLocalFrame);
      }

      // everything under threshold or edge effects
      if (!dSteps.size()) continue;
      // loop over the steps and create cells
      for (auto dStep : dSteps) {
        // smeared, passed and taken
        // create digitization cell
        // smear the path length with (1 +/- epsilon)
        double sLength =
            (m_smearParameter != 0.) ? (dStep.stepLength * (1. + m_smearParameter * m_gauss())) : dStep.stepLength;
        // write path length (for digital use default 1)
        double wLength = (m_analogReadout) ? sLength : 1.;
        // check if it falls below threshold
        if (m_cutParameter != 0. && sLength < m_cutParameter * thickness) continue;
        // translate DigiTrackhitAssociation into TrackHit
        // @todo check easier possibility
        fcc::TrackHit th = trackHits->create();
        th.core().time = hit.hit().core().time;
        th.core().bits = hit.hit().core().bits;
        th.core().cellId = hit.hit().core().cellId;
        th.core().energy = hit.hit().core().energy;
        //    std::vector<fcc::TrackHit> th = {hit.hit()};
        auto digiCell = sim::FCCDigitizationCell({th}, dStep.stepCell.channel0, dStep.stepCell.channel1, wLength);
        // push back used cells per surface
        cellsPerSurface[surfaceID].push_back(std::move(digiCell));
      }  // dSteps
    }    // if hitDigitizationModule
  }      // hits

  // Now create clusters for cells per surface
  for (auto& surf : cellsPerSurface) {
    // access the surface & digitizationmodule from the surfaceID
    auto surfaceID = surf.first;
    auto search = m_detectorElements.find(Identifier(surfaceID));
    if (search == m_detectorElements.end()) {
      error() << "Detector element with identifier: " << surfaceID << " not found!" << endmsg;
      return StatusCode::FAILURE;
    }
    auto hitDetElement = search->second;
    if (!hitDetElement) {
      error() << "Invalid Detector element with identifier: " << surfaceID << "!" << endmsg;
      return StatusCode::FAILURE;
    }
    // access the surface corresponding to the detector element
    const Acts::Surface& hitSurface = hitDetElement->surface();

    auto hitDigitizationModule = hitDetElement->digitizationModule();
    if (!hitDigitizationModule) {
      error() << "No digitization module attached to detector element with identifier: " << surfaceID << "!" << endmsg;
      return StatusCode::FAILURE;
    }

    // get the corresponding dd4hep detelement
    auto dd4hepDetElement = m_volumeManager.lookupDetElement(surfaceID);
    if (!dd4hepDetElement) {
      error() << "DD4hep detector element with identifier: " << surfaceID << "not valid!" << endmsg;
      return StatusCode::FAILURE;
    }
    if (!dd4hepDetElement.volume().isSensitive()) {
      error() << "DD4hep detector element with identifier: " << surfaceID << " is not sensitive!" << endmsg;
      return StatusCode::FAILURE;
    }
    surfaceID = dd4hepDetElement.volumeID();
    // get the segmentation
    const Acts::Segmentation& segmentation = hitDigitizationModule->segmentation();
    const Acts::BinUtility& binUtility = segmentation.binUtility();
    // merge cells
    // @ todo apply energy cut
    auto mergedCells = mergeCells(surf.second);
    // group together cells which belong to same cluster
    auto clusterMap = createClusters(mergedCells);
    // #tracks per cluster
    // set of track identification - to only have hits from different tracks
    std::set<unsigned> nTracksPerCluster;
    // acts digi cells
    for (auto& cells : clusterMap) {
      // create the track cluster
      fcc::TrackCluster trackCluster = trackClusters->create();
      //--- Now create the merged cluster ---
      // parameters to be averaged over cells for cluster
      double localX = 0., localY = 0., norm = 0., clusterEnergy = 0., clusterTime = 0.;
      // go through the merged cells to calculate the averaged local position and time of the cluster
      for (auto& cell : cells) {
        for (auto& h : cell.trackHits) {
          // get all tracks of this cell
          nTracksPerCluster.insert(h.core().bits);
          // get energy of this cell
          clusterEnergy += h.core().energy;
          // calculate mean of time
          clusterTime += h.core().time;
        }
        // get center position of the current cell
        auto cellCenter = segmentation.cellPosition(cell);
        // calculate position of hit (for digital readout data==1)
        localX += cell.data * cellCenter.x();
        localY += cell.data * cellCenter.y();
        norm += cell.data;

        for (auto& th : cell.trackHits) {
          trackCluster.addhits(th);
        }
      }
      // divide by the total path - analog clustering
      if (norm > 1) {
        localX /= norm;
        localY /= norm;
        clusterTime /= cells.size();
      }
      /// ----------- Create track cluster -----------
      // local position of merged cluster
      Acts::Vector2D localPosition(localX, localY);
      // calculate global position of the cluster
      Acts::Vector3D globalPosition(0., 0., 0.);
      hitSurface.localToGlobal(localPosition, Acts::Vector3D(0., 0., 0.), globalPosition);
      // translate to fcc edm
      auto position = fcc::Point();
      position.x = globalPosition.x();
      position.y = globalPosition.y();
      position.z = globalPosition.z();
      // save position in cluster
      trackCluster.core().position = position;
      trackCluster.core().energy = clusterEnergy;
      trackCluster.core().time = clusterTime;
      /// ----------- Create Acts cluster - possibly to be written out -----------
      /// ----------- Create unique Acts global channel identifier -----------
      // get the bins of the local position to create channel identifier for this surface
      size_t bin0 = binUtility.bin(localPosition, 0);
      size_t bin1 = binUtility.bin(localPosition, 1);
      size_t binSerialized = binUtility.serialize({bin0, bin1, 0});
      // get the surface identifier
      Acts::GeometryID geoID(hitSurface.geoID());
      // create the unique global channel identifier by adding to surface identifier
      geoID.add(binSerialized, Acts::GeometryID::channel_mask);
      // the covariance is currently set to 0.
      // @todo create resolution maps & allow reading in resolution maps or use cov for digital readout
      Acts::ActsSymMatrixD<2> cov;
      cov << 0., 0., 0., 0.;

      m_planarClusterHandle.put(new sim::FCCPlanarCluster(clusterEnergy, nTracksPerCluster.size(), hitSurface,
                                                          Identifier(geoID.value()), std::move(cov), localX, localY,
                                                          std::move(cells)));
    }
  }
  return StatusCode::SUCCESS;
}

const std::vector<sim::FCCDigitizationCell>
GeometricTrackerDigitizer::mergeCells(std::vector<sim::FCCDigitizationCell>& cells, double energyCut) const {
  if (!cells.size()) return cells;
  // the output
  std::vector<std::vector<sim::FCCDigitizationCell>> mergedCells;
  // create the graph
  boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> graph;
  //  add the needed amount of vertices to the graph
  for (auto cell : cells) {
    // add vertex to graph for each cell
    add_vertex(graph);
  }

  // loop through cells and set the edges
  for (auto cellA = std::begin(cells); cellA != (std::end(cells) - 1); cellA++) {
    auto indexA = std::distance(cells.begin(), cellA);
    for (auto cellB = (cellA + 1); cellB != std::end(cells); cellB++) {
      auto indexB = std::distance(cells.begin(), cellB);
      // the channels
      auto c0A = cellA->channel0;
      auto c1A = cellA->channel1;
      auto c0B = cellB->channel0;
      auto c1B = cellB->channel1;

      // the conditions
      bool isSameChannel0 = (c0A == c0B);
      bool isSameChannel1 = (c1A == c1B);
      // same cell
      if (isSameChannel0 && isSameChannel1) {
        add_edge(indexA, indexB, graph);
      }  // if
    }    // cellsB
  }      // cellsA

  std::vector<size_t> component(num_vertices(graph));
  connected_components(graph, &component[0]);
  // copy the component map
  std::vector<size_t> keys = component;
  // sort
  std::sort(keys.begin(), keys.end());
  // get the keys
  auto it = std::unique(keys.begin(), keys.end());
  keys.erase(it, keys.end());

  for (auto& comp : keys) {
    std::vector<sim::FCCDigitizationCell> compCells;
    for (size_t i = 0; i != component.size(); ++i) {
      if (component[i] == comp) {
        compCells.push_back(cells.at(i));
      }
    }
    mergedCells.push_back(compCells);
  }
  std::vector<sim::FCCDigitizationCell> sameCells;

  for (auto& sCells : mergedCells) {
    sim::FCCDigitizationCell& newCell = *std::begin(sCells);
    for (auto sCell = (std::begin(sCells) + 1); sCell != std::end(sCells); sCell++) {
      newCell.addCell((*sCell), m_analogReadout);
    }
    // apply energycut - if given
    double totalCellEnergy = 0;
    if (m_analogReadout) {
      totalCellEnergy = newCell.data;
    } else {
      // for digital case accumulate energy of all hits
      for (auto& th : newCell.trackHits)
        totalCellEnergy += th.core().energy;
    }
    // only count if total cell energy is above threshold
    if (totalCellEnergy >= energyCut) sameCells.push_back(newCell);
  }
  return sameCells;
}

const std::vector<std::vector<sim::FCCDigitizationCell>>
GeometricTrackerDigitizer::createClusters(const std::vector<sim::FCCDigitizationCell>& cells) const {
  // the output
  std::vector<std::vector<sim::FCCDigitizationCell>> mergedCells;
  // create the graph
  boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> graph;
  //  add the needed amount of vertices to the graph
  for (auto cell : cells) {
    // add vertex to graph for each cell
    add_vertex(graph);
  }

  // loop through cells and set the edges
  for (auto cellA = std::begin(cells); cellA != (std::end(cells) - 1); cellA++) {
    auto indexA = std::distance(cells.begin(), cellA);
    for (auto cellB = (cellA + 1); cellB != std::end(cells); cellB++) {
      auto indexB = std::distance(cells.begin(), cellB);
      // the channels
      auto c0A = cellA->channel0;
      auto c1A = cellA->channel1;
      auto c0B = cellB->channel0;
      auto c1B = cellB->channel1;

      // the conditions
      bool isNeighbour0 = (c0A == (c0B - 1) || c0A == (c0B + 1));
      bool isSameChannel0 = (c0A == c0B);
      bool isNeighbour1 = (c1A == (c1B - 1) || c1A == (c1B + 1));
      bool isSameChannel1 = (c1A == c1B);
      // distinguish between between merging cells, when they have a common
      // corner (8cell) or a common edge (4cell)
      if (m_commonCorner) {
        // 8cell
        if ((isNeighbour0 || isSameChannel0) && (isNeighbour1 || isSameChannel1)) {
          add_edge(indexA, indexB, graph);
        }
      } else {
        // 4cell
        if ((isNeighbour0 && isSameChannel1) || (isNeighbour1 && isSameChannel0)) {
          add_edge(indexA, indexB, graph);
        }
      }
    }  // cellsB
  }    // cellsA

  std::vector<size_t> component(num_vertices(graph));
  connected_components(graph, &component[0]);
  // copy the component map
  std::vector<size_t> keys = component;
  // sort
  std::sort(keys.begin(), keys.end());
  // get the keys
  auto it = std::unique(keys.begin(), keys.end());
  keys.erase(it, keys.end());

  for (auto& comp : keys) {
    std::vector<sim::FCCDigitizationCell> compCells;
    for (size_t i = 0; i != component.size(); ++i) {
      if (component[i] == comp) {
        compCells.push_back(cells.at(i));
      }
    }
    mergedCells.push_back(compCells);
  }
  return mergedCells;
}

StatusCode GeometricTrackerDigitizer::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
