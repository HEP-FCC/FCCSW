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
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
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
  declareProperty("clusterWriter", m_clusterWriter, "clusterWriter");
  declareProperty("trackingGeometrySvc", m_trkGeoSvc, "The geometry service providing the tracking geometry");
  declareProperty("geoSvc", m_geoSvc, "The geometry service providing the tracking geometry");
  declareProperty("digiTrackHitAssociation", m_digiTrackHitAssociation, "Handle for input digiTrackHitAssociation");
  declareProperty("trackClusters", m_trackClusters, "Handle for output clusters");
  declareProperty("clusterTrackHits", m_trackHits, "Handle for output hits belonging to the clusters");
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

  // retrieve the extrapolation tool
  if (m_writeClusterInfo && !m_clusterWriter.retrieve()) {
    error() << "ClusterWriter tool cannot be retieved" << endmsg;
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

  // the track hits of the cluster
  auto trackHits = m_trackHits.createAndPut();
  // prepare the output clusters
  auto trackClusters = m_trackClusters.createAndPut();
  // get the cells per surface
  // the cells to be used
  std::unordered_map<long long int, std::unordered_map<size_t, std::pair<sim::FCCDigitizationCell, bool>>>
      cellsPerSurface;
  createCells(cellsPerSurface);
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
    // get the dd4hep segmentation for the current detector element
    dd4hep::SensitiveDetector sensDet(dd4hepDetElement.volume().sensitiveDetector());
    auto dd4hepSegmentation = sensDet.readout().segmentation();
    // get the segmentation
    const Acts::Segmentation& segmentation = hitDigitizationModule->segmentation();
    const Acts::BinUtility& binUtility = segmentation.binUtility();
    // merge cells
    // @ todo apply energy cut
    // group together cells which belong to same cluster
    auto clusterMap = createClusters<sim::FCCDigitizationCell>(surf.second, binUtility.bins(0), true, 0.);
    // acts digi cells
    for (auto& cells : clusterMap) {
      // #tracks per cluster
      // set of track identification - to only have hits from different tracks
      std::vector<unsigned> tracksPerCluster;
      tracksPerCluster.reserve(cells.size() * 2);
      // create the track cluster
      fcc::TrackCluster trackCluster = trackClusters->create();
      //--- Now create the merged cluster ---
      // parameters to be averaged over cells for cluster
      double localX = 0., localY = 0., norm = 0., clusterEnergy = 0., clusterTime = 0.;
      // go through the merged cells to calculate the averaged local position and time of the cluster
      for (auto& cell : cells) {
        auto cellTime = cell.averagedTime();
        // get all tracks of this cell and add it to the cluster
        tracksPerCluster.insert(std::end(tracksPerCluster), std::begin(cell.trackHits), std::end(cell.trackHits));
        // get energy of this cell
        clusterEnergy += cell.data;
        // calculate mean of time
        clusterTime += cellTime;
        // get center position of the current cell
        auto cellCenter = segmentation.cellPosition(cell);
        // calculate position of hit (for digital readout data==1)
        if (m_analogueReadout) {
          localX += cell.data * cellCenter.x();
          localY += cell.data * cellCenter.y();
          norm += cell.data;
        } else {
          localX += cellCenter.x();
          localY += cellCenter.y();
          norm += 1;
        }
        // calculate dd4hep cellID for the fcc TrackHit
        // @todo make this more general, this is because we are using xz-segmentation
        // global position is actually not used for conversion
        auto dd4hepHitLocPos =
            dd4hep::Position(cellCenter.x() / Acts::units::_cm, 0., cellCenter.y() / Acts::units::_cm);
        unsigned long long hitCellID =
            dd4hepSegmentation.cellID(dd4hepHitLocPos, dd4hep::Position(0., 0., 0.), surfaceID);
        // create trackhit for each cell in data model
        fcc::TrackHit th = trackHits->create();
        th.core().time = cellTime;
        th.core().energy = cell.data;
        th.core().cellId = hitCellID;
        trackCluster.addhits(th);
      }
      // divide by the total path - analogue clustering
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
      // ----------- Create Acts cluster - possibly to be written out -----------
      // ----------- Create unique Acts global channel identifier -----------
      // get the bins of the local position to create channel identifier for this surface
      if (m_writeClusterInfo) {
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
        // unique tracks per cluster
        std::sort(tracksPerCluster.begin(), tracksPerCluster.end());
        tracksPerCluster.erase(unique(tracksPerCluster.begin(), tracksPerCluster.end()), tracksPerCluster.end());
        // create and write the cluster
        m_clusterWriter->write(sim::FCCPlanarCluster(clusterEnergy, tracksPerCluster.size(), hitSurface,
                                                     Identifier(geoID.value()), std::move(cov), localX, localY,
                                                     std::move(cells)),
                               Gaudi::Hive::currentContext().evt());
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode GeometricTrackerDigitizer::createCells(
    std::unordered_map<long long int, std::unordered_map<size_t, std::pair<sim::FCCDigitizationCell, bool>>>&
        cellsPerSurface) {
  // Retrieve the input hits
  const fcc::DigiTrackHitAssociationCollection* hits = m_digiTrackHitAssociation.get();
  // go through hits and create cells
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
      // get the segmentation
      const Acts::Segmentation& segmentation = hitDigitizationModule->segmentation();
      const Acts::BinUtility& binUtility = segmentation.binUtility();
      // get the hit position
      Acts::Vector3D globalIntersection(hit.hit().position().x, hit.hit().position().y, hit.hit().position().z);
      // access the post step position to calculate the direction
      Acts::Vector3D postStepPosition(hit.postStepPosition().x, hit.postStepPosition().y, hit.postStepPosition().z);
      // calculate the momentum direction & intersection point in local frame
      Acts::Vector3D localDirection = (postStepPosition - globalIntersection).unit();
      // transform direction into local frame
      localDirection = hitSurface.transform().inverse().linear() * localDirection;
      // to ignore extreme incident angles
      if (std::abs(localDirection.z()) < m_cosThetaLocMin) {
        continue;
      }
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
        dSteps = m_moduleStepper->cellSteps(*hitDigitizationModule, positionLocalFrame, postPositionLocalFrame);
      }
      double totalG4HitEnergy = hit.hit().core().energy;
      double totalG4StepLength = (postStepPosition - globalIntersection).mag();
      // everything under threshold or edge effects
      if (!dSteps.size()) {
        continue;
      }
      // loop over the steps and create cells
      for (auto dStep : dSteps) {
        // smeared, passed and taken
        // create digitization cell
        // smear the path length with (1 +/- epsilon)
        /// @todo check for digital readout
        double sLength =
            (m_smearParameter != 0.) ? (dStep.stepLength * (1. + m_smearParameter * m_gauss())) : dStep.stepLength;
        // check if it falls below threshold
        if (m_cutParameter != 0. && sLength < m_cutParameter * thickness) {
          continue;
        }
        // scale energy for each cell by path length
        float cellEnergy = (totalG4StepLength > 0) ? totalG4HitEnergy * sLength / totalG4StepLength : 0.;
        // in case the track does not pass the module completely, which means, it is a secondary which does not leave
        // the module, don't count it as a track
        std::vector<unsigned> trackIDs = {};
        if (sLength * cos(localDirection.theta()) >=
            (2. * hitDigitizationModule->halfThickness() - 10e-5 * Acts::units::_mm)) {
          trackIDs.push_back(hit.hit().core().bits);
        }
        auto digiCell = sim::FCCDigitizationCell(std::move(trackIDs), hit.hit().core().time, dStep.stepCell.channel0,
                                                 dStep.stepCell.channel1, cellEnergy);

        // calculate key of cell which is the global grid index
        size_t globalIndex = digiCell.channel0 + binUtility.bins(0) * digiCell.channel1;
        // insert new cell - only adds cell, if cell was not there yet
        auto insertCell = cellsPerSurface[surfaceID].insert({globalIndex, {std::move(digiCell), false}});
        if (!insertCell.second) {
          // check if there is already a cell at same position and merge in that
          // case
          insertCell.first->second.first.addCell(digiCell);
        }
      }  // dSteps
    }    // if hitDigitizationModule
  }      // hits

  return StatusCode::SUCCESS;
}

template <typename Cell>
std::vector<std::vector<Cell>>
GeometricTrackerDigitizer::createClusters(std::unordered_map<size_t, std::pair<Cell, bool>>& cellMap, size_t nBins0,
                                          bool commonCorner, double energyCut) {
  // the output
  std::vector<std::vector<Cell>> mergedCells;
  // now go through cells and label
  for (auto& cell : cellMap) {
    // check if the cell was already used
    if (!(cell.second.second) && (cell.second.first.depositedEnergy() >= energyCut)) {
      // create new cluster
      mergedCells.push_back(std::vector<Cell>());
      // add current cell to current cluster
      mergedCells.back().push_back(cell.second.first);
      // set cell to be used already
      cell.second.second = true;
      // fill all cells belonging to that cluster
      fillCluster(mergedCells, cellMap, cell.first, nBins0, commonCorner, energyCut);
    }
  }
  // return the grouped together cells
  return mergedCells;
}

template <typename Cell>
void GeometricTrackerDigitizer::fillCluster(std::vector<std::vector<Cell>>& mergedCells,
                                            std::unordered_map<size_t, std::pair<Cell, bool>>& cellMap,
                                            size_t index,
                                            size_t nBins0,
                                            bool commonCorner,
                                            double energyCut) {
  // go recursively through all neighbours of this cell, if present
  // calculate neighbour indices first
  constexpr int iMin = -1;
  int jMin = -nBins0;
  constexpr int iMax = 1;
  int jMax = nBins0;
  std::vector<int> neighbourIndices;
  // the neighbour indices - filled depending on merging case
  if ((index % nBins0) == 0) {
    // left edge case
    if (commonCorner) {
      neighbourIndices = {jMin, jMin + iMax, iMax, jMax, jMax + iMax};
    } else {
      neighbourIndices = {jMin, iMax, jMax};
    }
  } else if (((index + 1) % nBins0) == 0) {
    // right edge case
    if (commonCorner) {
      neighbourIndices = {jMin + iMin, jMin, iMin, jMax + iMin, jMax};
    } else {
      neighbourIndices = {jMin, iMin, jMax};
    }
  } else {
    if (commonCorner) {
      neighbourIndices = {jMin + iMin, jMin, jMin + iMax, iMin, iMax, jMax + iMin, jMax, jMax + iMax};
    } else {
      neighbourIndices = {jMin, iMin, iMax, jMax};
    }
  }
  // go through neighbours and recursively call connected component algorithm
  for (auto& i : neighbourIndices) {
    // calculate neighbour index of current cell
    int neighbourIndex = int(index) + i;
    // check if neighbour is there
    auto search = cellMap.find(neighbourIndex);
    if ((search != cellMap.end())) {
      // get the corresponding index and call function again
      auto newIndex = search->first;
      auto currentCell = search->second.first;
      // if cell was not already added to cluster & deposited energy is higher
      // than the energy threshold, add it to the cluster
      if (!search->second.second && currentCell.depositedEnergy() >= energyCut) {
        // add current cell to current cluster
        mergedCells.back().push_back(currentCell);
        // set cell to be used already
        search->second.second = true;
        // add all neighbours to cluster
        fillCluster(mergedCells, cellMap, newIndex, nBins0, commonCorner, energyCut);
      }  // check if was used already
    }    // check if neighbour is there
  }      // go through neighbour indics
}

StatusCode GeometricTrackerDigitizer::finalize() {

  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
