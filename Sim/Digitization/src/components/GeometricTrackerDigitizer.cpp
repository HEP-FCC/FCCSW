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
#include "Math/Math/Math.h"
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
  declareProperty("singleTrackClusters", m_singleTrackClusters,
                  "Handle for single particle output clusters [optional]");
  declareProperty("singleClusterTrackHits", m_singleTrackHits,
                  "Handle for output hits belonging to the single particle clusters [optional]");
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
  const fcc::DigiTrackHitAssociationCollection* preHits = m_digiTrackHitAssociation.get();

  // the map of hits per surface
  auto hitsPerSurface = collectHitsOnSurface(*preHits);
  // merge the hits to one hit per surface
  auto hits = mergeHits(hitsPerSurface);
  // the track hits of the cluster
  auto trackHits = m_trackHits.createAndPut();
  // prepare the output clusters
  auto trackClusters = m_trackClusters.createAndPut();
  // possible outout of single particle clusters
  auto singleTrackClusters = m_singleTrackClusters.createAndPut();
  // possible outout of single particle clusters
  auto singleTrackHits = m_singleTrackHits.createAndPut();
  // the cells to be used
  std::map<size_t, std::vector<sim::FCCDigitizationCell>> cellsPerSurface;
  // go through hits
  for (auto hit : hits) {
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
    auto hitDetElement = m_detectorElements.at(Identifier(surfaceID));
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
      // create from hit
      // get the hit position
      fcc::Vector3D globalIntersection(hit.hit().position().x, hit.hit().position().y, hit.hit().position().z);
      // access the post step position to calculate the direction
      fcc::Vector3D postStepPosition(hit.postStepPosition().x, hit.postStepPosition().y, hit.postStepPosition().z);
      // calculate the momentum direction & intersection point in local frame
      fcc::Vector3D localDirection = (postStepPosition - globalIntersection).unit();
      fcc::Vector2D localIntersection(0., 0.);
      hitSurface.globalToLocal(globalIntersection, localDirection, localIntersection);
      localDirection = (hitSurface.transform().inverse()) * localDirection;
      // calculate the digitization steps
      std::vector<Acts::DigitizationStep> dSteps =
          m_moduleStepper->cellSteps(*hitDigitizationModule, localIntersection, localDirection.unit());
      // everything under threshold or edge effects
      if (!dSteps.size()) continue;
      // possible single particle cluster
      fcc::TrackCluster singleTrackCluster;
      // (1) calculate position of the single particle cluster
      double localX = 0, localY = 0;
      // go through the merged cells to calculate the averaged local position of the cluster
      double norm = 0;
      // calculate time of cluster (mean of all cells)
      double clusterTime = 0.;
      // loop over the steps
      for (auto dStep : dSteps) {
        // smeared, passed and taken
        // create digitization cell
        fcc::TrackHit trackHit = singleTrackHits->create();
        trackHit.core(hit.hit().core());
        // smear the path length with (1 +/- epsilon)
        double sLength =
            (m_smearParameter != 0.) ? (dStep.stepLength * (1. + m_smearParameter * m_gauss())) : dStep.stepLength;
        // write path length (for digital use default 1)
        double wLength = (m_analogReadout) ? sLength : 1.;
        // check if it falls below threshold
        if (m_cutParameter != 0. && sLength < m_cutParameter * thickness) continue;
        // check if single particle cluster parameters should be calculated
        if (m_singleParticleClusters) {
          // calculate position of hit (for digital readout data==1)
          localX += wLength * dStep.stepCellCenter.x();
          localY += wLength * dStep.stepCellCenter.y();
          norm += wLength;
          clusterTime += trackHit.core().time;
          // @todo not working yet
          singleTrackCluster.addhits(trackHit);
        }
        auto digiCell = sim::FCCDigitizationCell(trackHit, dStep.stepCell.channel0, dStep.stepCell.channel1, wLength);
        // push back used cells per surface
        cellsPerSurface[surfaceID].push_back(std::move(digiCell));
      }  // dSteps
      if (m_singleParticleClusters) {
        // local position of single particle cluster
        fcc::Vector2D localPosition(localX, localY);
        // calculate global position of the cluster
        fcc::Vector3D globalPosition(0., 0., 0.);
        hitSurface.localToGlobal(localPosition, fcc::Vector3D(0., 0., 0.), globalPosition);
        // translate to fcc edm
        auto position = fcc::Point();
        position.x = globalPosition.x();
        position.y = globalPosition.y();
        position.z = globalPosition.z();
        // save position in single particle track cluster
        singleTrackCluster.core().position = position;
        // calculate total energy deposited in cluster (sum up energy of all cells)
        singleTrackCluster.core().energy = norm;
        singleTrackCluster.core().time = clusterTime;
        singleTrackClusters->push_back(singleTrackCluster);
      }
    }  // if hitDigitizationModule
  }    // hits

  // Now create clusters for cells per surface
  for (auto& surf : cellsPerSurface) {
    auto clusterMap = createClusters(surf.second, m_commonCorner);
    // access the surface & digitizationmodule from the surfaceID
    auto surfaceID = surf.first;
    auto search = m_detectorElements.find(Identifier(surfaceID));
    if (search == m_detectorElements.end()) {
      error() << "Detector element with identifier: " << surfaceID << " not found!" << endmsg;
      return StatusCode::FAILURE;
    }
    auto hitDetElement = m_detectorElements.at(Identifier(surfaceID));
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
    // get the dd4hep segmentation for the current detector element
    dd4hep::SensitiveDetector sensDet(dd4hepDetElement.volume().sensitiveDetector());
    auto dd4hepSegmentation = sensDet.readout().segmentation();
    // acts digi cells

    for (auto& cells : clusterMap) {
      // create the track cluster
      fcc::TrackCluster trackCluster = trackClusters->create();
      // create cluster
      // get the segmentation
      const Acts::Segmentation& segmentation = hitDigitizationModule->segmentation();
      //--- Now create the merged cluster ---
      // (1) calculate position of the cluster
      double localX = 0, localY = 0;
      // go through the merged cells to calculate the averaged local position of the cluster
      double norm = 0;
      // calculate time of cluster (mean of all cells)
      double clusterTime = 0.;
      for (auto& cell : cells) {
        // get center position of the current cell
        auto cellCenter = segmentation.cellPosition(cell);
        // calculate position of hit (for digital readout data==1)
        localX += cell.data * cellCenter.x();
        localY += cell.data * cellCenter.y();
        norm += cell.data;
        // the local position of the hit
        fcc::Vector2D locPos(cell.data * cellCenter.x(), cell.data * cellCenter.y());
        // calculate dd4hep cellID for the fcc TrackHit
        // @todo make this more general, this is because we are using xz-segmentation
        // global position is actually not used for conversion
        auto dd4hepHitLocPos = dd4hep::Position(locPos.x() / Acts::units::_cm, 0., locPos.y() / Acts::units::_cm);
        unsigned long long hitCellID =
            dd4hepSegmentation.cellID(dd4hepHitLocPos, dd4hep::Position(0., 0., 0.), surfaceID);
        // create fcc::edm track hits which are in the cluster
        fcc::TrackHit outHit = trackHits->create();
        outHit.core().time = cell.trackHit.core().time;
        outHit.core().bits = cell.trackHit.core().bits;
        outHit.core().cellId = hitCellID;
        outHit.core().energy = cell.data;
        // calculate mean of time
        clusterTime += outHit.core().time;
        trackCluster.addhits(outHit);
      }
      // divide by the total path - analog clustering
      if (norm > 0) {
        localX /= norm;
        localY /= norm;
        clusterTime /= cells.size();
      }
      /// ----------- Create unique global channel identifier -----------
      // local position of merged cluster
      fcc::Vector2D localPosition(localX, localY);
      // calculate global position of the cluster
      fcc::Vector3D globalPosition(0., 0., 0.);
      hitSurface.localToGlobal(localPosition, fcc::Vector3D(0., 0., 0.), globalPosition);
      // translate to fcc edm
      auto position = fcc::Point();
      position.x = globalPosition.x();
      position.y = globalPosition.y();
      position.z = globalPosition.z();
      // save position in cluster
      trackCluster.core().position = position;
      // calculate total energy deposited in cluster (sum up energy of all cells)
      trackCluster.core().energy = norm;
      trackCluster.core().time = clusterTime;
    }
  }
  return StatusCode::SUCCESS;
}

const std::map<long long int, fcc::DigiTrackHitAssociationCollection>
GeometricTrackerDigitizer::collectHitsOnSurface(const fcc::DigiTrackHitAssociationCollection& hits) const {
  // the map of hits per surface
  std::map<long long int, fcc::DigiTrackHitAssociationCollection> hitsPerSurface;
  // loop through the Geant4 hits to get all hits on one surface
  for (auto hit : hits) {
    // access the cell ID
    long long int cellID = hit.hit().core().cellId;
    // get the corresponding dd4hep detelement
    auto dd4hepDetElement = m_volumeManager.lookupDetElement(cellID);
    if (!dd4hepDetElement) {
      error() << "DD4hep detector element with channel identifier: " << cellID << "not valid!" << endmsg;
    }
    // get the surface ID
    long long int surfaceID = dd4hepDetElement.volumeID();
    // push back hits per surface
    hitsPerSurface[surfaceID].push_back(hit.clone());
  }
  return hitsPerSurface;
}

const fcc::DigiTrackHitAssociationCollection GeometricTrackerDigitizer::mergeHits(
    const std::map<long long int, fcc::DigiTrackHitAssociationCollection>& hitsPerSurface) const {
  // the output merged hits
  fcc::DigiTrackHitAssociationCollection mergedHits;
  for (auto& surface : hitsPerSurface) {
    // the surface ID
    auto surfaceID = surface.first;
    // create hits per track of this surface
    std::map<unsigned, std::vector<fcc::DigiTrackHitAssociation>> hitsPerTrack;
    for (auto& hit : surface.second) {
      auto trackID = hit.hit().core().bits;
      hitsPerTrack[trackID].push_back(hit);
    }
    // loop through hits per track
    for (auto& track : hitsPerTrack) {
      // average hits per track
      auto cellID = track.second.front()
                        .hit()
                        .cellId();  // set the first cellID (is later only used to identify surface not channel)
      // create one hits for all hits of this track
      double energy = 0;
      float time = 0;
      auto bits = track.first;
      // initialize positions with first positions
      fcc::Vector3D position(track.second.front().hit().position().x, track.second.front().hit().position().y,
                             track.second.front().hit().position().z);
      fcc::Vector3D postPosition(track.second.front().postStepPosition().x, track.second.front().postStepPosition().y,
                                 track.second.front().postStepPosition().z);
      for (auto& hit : track.second) {
        /// the current hit position
        fcc::Vector3D hitPos(hit.hit().position().x, hit.hit().position().y, hit.hit().position().z);
        // access the post step position to calculate the direction
        fcc::Vector3D hitPostPos(hit.postStepPosition().x, hit.postStepPosition().y, hit.postStepPosition().z);
        // get first and last point in this module
        if (hitPos.norm() < position.norm()) position = hitPos;
        if (hitPostPos.norm() > postPosition.norm()) postPosition = hitPostPos;
        // collect all the energy deposited
        energy += hit.hit().core().energy;
        // collect time
        time += hit.hit().core().time;
      }  // hits per track
      // average time
      if (track.second.size() > 0) time /= track.second.size();
      // intersect surface
      // access surface at this position
      auto search = m_detectorElements.find(Identifier(surfaceID));
      if (search == m_detectorElements.end()) {
        error() << "Detector element with identifier: " << surfaceID << " and cellID: " << cellID << " not found!"
                << endmsg;
      }
      auto hitDetElement = m_detectorElements.at(Identifier(surfaceID));
      if (!hitDetElement) {
        error() << "Invalid Detector element with identifier: " << surfaceID << "!" << endmsg;
      }
      // access the surface corresponding to the detector element
      const Acts::Surface& hitSurface = hitDetElement->surface();
      fcc::Vector3D dir = (postPosition - position).unit();
      auto intersection = hitSurface.intersectionEstimate(position, dir, false, true);
      // if intersection is !valid (e.g. in case of 0 steplength) continuw
      if (!intersection.valid) continue;
      // set position to be exactly on the surface (makes digitzation more accurate)
      position = intersection.position;
      // in case the new position is further outside than the post position - create a new post position
      if (position.norm() >= postPosition.norm()) postPosition = position + intersection.pathLength * dir;

      auto fccPosition = fcc::Point();
      fccPosition.x = position.x();
      fccPosition.y = position.y();
      fccPosition.z = position.z();
      // set the post position
      auto fccPostPosition = fcc::Point();
      fccPostPosition.x = postPosition.x();
      fccPostPosition.y = postPosition.y();
      fccPostPosition.z = postPosition.z();
      // create the new output hit
      fcc::PositionedTrackHit outHit;
      outHit.core().cellId = cellID;
      outHit.core().energy = energy;
      outHit.core().time = time;
      outHit.core().bits = bits;
      outHit.position(fccPosition);
      // create the digitization association
      fcc::DigiTrackHitAssociation outDigiHit;
      outDigiHit.hit(outHit);
      outDigiHit.postStepPosition(fccPostPosition);
      // collect the hits
      mergedHits.push_back(outDigiHit);
    }  // track
  }    // surface
  return mergedHits;
}

const std::vector<std::vector<sim::FCCDigitizationCell>>
GeometricTrackerDigitizer::createClusters(const std::vector<sim::FCCDigitizationCell>& cells, bool commonCorner) const {
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
      if (commonCorner) {
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
  size_t num = connected_components(graph, &component[0]);
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
