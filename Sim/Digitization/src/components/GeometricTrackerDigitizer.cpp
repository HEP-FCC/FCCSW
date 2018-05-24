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
#include <chrono>
#include <ctime>
// hack
#include "TFile.h"
#include "TTree.h"
#include "datamodel/MCParticle.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/MCParticleData.h"

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

  /// timeMerge = std::chrono::system_clock::now() - std::chrono::system_clock::now();
  /// timeFillLoop = std::chrono::system_clock::now() - std::chrono::system_clock::now();
  /// timeFillMap = std::chrono::system_clock::now() - std::chrono::system_clock::now();
  /// timeSearchMap = std::chrono::system_clock::now() - std::chrono::system_clock::now();
  /// timeCheckEdges = std::chrono::system_clock::now() - std::chrono::system_clock::now();
  /// timeLabelLoop = std::chrono::system_clock::now() - std::chrono::system_clock::now();

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

  // the track hits of the cluster
  auto trackHits = m_trackHits.createAndPut();
  // prepare the output clusters
  auto trackClusters = m_trackClusters.createAndPut();
  // prepare output planar clusters
  auto planarClusters = m_planarClusterHandle.createAndPut();
  std::cout << "begin digitization" << std::endl;
  planarClusters->reserve(100000);

  std::chrono::duration<double> ccTime = std::chrono::system_clock::now() - std::chrono::system_clock::now();
  // get the cells per surface
  // the cells to be used
  std::map<long long int, std::vector<sim::FCCDigitizationCell>> cellsPerSurface;
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
    auto startCreateClusters = std::chrono::system_clock::now();

    auto clusterMap =
        createClusters<sim::FCCDigitizationCell>(surf.second, binUtility.bins(0), binUtility.bins(1), true, 0.);

    auto endCreateClusters = std::chrono::system_clock::now();
    ccTime += (endCreateClusters - startCreateClusters);

    // acts digi cells
    for (auto& cells : clusterMap) {
      // #tracks per cluster
      // set of track identification - to only have hits from different tracks
      size_t nTracksPerCluster = 0;
      // create the track cluster
      fcc::TrackCluster trackCluster = trackClusters->create();

      //--- Now create the merged cluster ---
      // parameters to be averaged over cells for cluster
      double localX = 0., localY = 0., norm = 0., clusterEnergy = 0., clusterTime = 0.;
      // go through the merged cells to calculate the averaged local position and time of the cluster
      for (auto& cell : cells) {
        auto cellTime = cell.averagedTime();
        // get all tracks of this cell
        nTracksPerCluster += cell.uniqueTracks().size();
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

      planarClusters->emplace_back(sim::FCCPlanarCluster(clusterEnergy, nTracksPerCluster, hitSurface,
                                                         Identifier(geoID.value()), std::move(cov), localX, localY,
                                                         std::move(cells)));
    }
  }
  std::cout << "after creating clusters" << std::endl;
  std::cout << "elapsed time for creating clusters & merging cells: " << ccTime.count() << std::endl;
  return StatusCode::SUCCESS;
}

StatusCode GeometricTrackerDigitizer::createCells(
    std::map<long long int, std::vector<sim::FCCDigitizationCell>>& cellsPerSurface) {
  auto startCells = std::chrono::system_clock::now();
  std::chrono::duration<double> timeSteps = std::chrono::system_clock::now() - std::chrono::system_clock::now();
  std::chrono::duration<double> detLookUp = std::chrono::system_clock::now() - std::chrono::system_clock::now();

  // Retrieve the input hits
  const fcc::DigiTrackHitAssociationCollection* hits = m_digiTrackHitAssociation.get();
  // go through hits and create cells
  for (auto hit : (*hits)) {
    // auto searchParticle = m_particleMap.find(hit.hit().core().bits);
    // if (searchParticle == m_particleMap.end()) continue;
    // exclude secondaries
    // if (searchParticle != m_particleMap.end() && searchParticle->second.core.status != 1) continue;
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
    auto startDetLookUp = std::chrono::system_clock::now();
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
    auto endDetLookUp = std::chrono::system_clock::now();
    detLookUp += (endDetLookUp - startDetLookUp);
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
        auto startSteps = std::chrono::system_clock::now();
        dSteps = m_moduleStepper->cellSteps(*hitDigitizationModule, positionLocalFrame, postPositionLocalFrame);
        auto endSteps = std::chrono::system_clock::now();
        timeSteps += endSteps - startSteps;
      }

      double totalG4HitEnergy = hit.hit().core().energy;
      double totalG4StepLength = (postStepPosition - globalIntersection).mag();

      // everything under threshold or edge effects
      if (!dSteps.size()) continue;
      // loop over the steps and create cells
      for (auto dStep : dSteps) {
        // smeared, passed and taken
        // create digitization cell
        // smear the path length with (1 +/- epsilon)
        /// @todo check for digital readout
        double sLength =
            (m_smearParameter != 0.) ? (dStep.stepLength * (1. + m_smearParameter * m_gauss())) : dStep.stepLength;
        // check if it falls below threshold
        if (m_cutParameter != 0. && sLength < m_cutParameter * thickness) continue;
        // translate DigiTrackhitAssociation into TrackHit
        // @todo check easier possibility
        //   fcc::TrackHit th = trackHits->create();
        //   th.core().time = hit.hit().core().time;
        //   th.core().bits = hit.hit().core().bits;

        // scale energy for each cell by path length
        float cellEnergy = (totalG4StepLength > 0) ? totalG4HitEnergy * sLength / totalG4StepLength : 0.;
        auto digiCell = sim::FCCDigitizationCell({hit.hit().core().bits}, hit.hit().core().time,
                                                 dStep.stepCell.channel0, dStep.stepCell.channel1, cellEnergy);
        // push back used cells per surface
        cellsPerSurface[surfaceID].push_back(std::move(digiCell));
      }  // dSteps
    }    // if hitDigitizationModule
  }      // hits

  std::cout << "after going through hits and creating cells" << std::endl;
  auto endCells = std::chrono::system_clock::now();
  std::chrono::duration<double> timeCells = endCells - startCells;

  std::cout << "elapsed time for creating cells: " << timeCells.count() << std::endl;
  std::cout << "elapsed time for creating steps: " << timeSteps.count() << std::endl;
  std::cout << "elapsed time for det element lookup: " << detLookUp.count() << std::endl;
  return StatusCode::SUCCESS;
}

template <typename Cell>
std::vector<std::vector<Cell>> GeometricTrackerDigitizer::createClusters(const std::vector<Cell>& cells, size_t nBins0,
                                                                         size_t nBins1, bool commonCorner,
                                                                         double energyCut) {
  // the output
  std::vector<std::vector<Cell>> mergedCells;
  // map containing all activated cells + boolean indicating if they have been
  // used already
  std::unordered_map<size_t, std::pair<Cell, bool>> cellMap;
  // Now fill cell map and merge cells if requested

  ///  auto startFillLoop = std::chrono::system_clock::now();
  for (auto& cell : cells) {
    // calculate key of cell which is the global grid index
    size_t globalIndex = cell.channel0 + nBins0 * cell.channel1;
    // insert new cell - only adds cell, if cell was not there yet
    ///  auto startFillMap = std::chrono::system_clock::now();
    auto insertCell = cellMap.insert({globalIndex, {cell, false}});
    ///   auto endFillMap = std::chrono::system_clock::now();
    ///   timeFillMap += (endFillMap - startFillMap);

    ///   auto startMerge = std::chrono::system_clock::now();
    if (!insertCell.second) {
      // check if there is already a cell at same position and merge in that
      // case
      insertCell.first->second.first.addCell(cell);
    }
    ///  auto endMerge = std::chrono::system_clock::now();
    ///    timeMerge += (endMerge - startMerge);
  }
  /// auto endFillLoop = std::chrono::system_clock::now();
  /// timeFillLoop += (endFillLoop - startFillLoop);

  /// auto startLabelLoop = std::chrono::system_clock::now();
  // now go through cells and label
  for (auto& cell : cellMap) {
    // check if the cell was already used
    if (!(cell.second.second) && (cell.second.first.depositedEnergy() >= energyCut)) {
      // create new cluster
      mergedCells.push_back(std::vector<Cell>());
      // fill all cells belonging to that cluster
      ccl(mergedCells, cellMap, cell.first, nBins0, nBins1, commonCorner, energyCut);
    }
  }
  /// auto endLabelLoop = std::chrono::system_clock::now();
  /// timeLabelLoop += (endLabelLoop - startLabelLoop);
  // return the grouped together cells
  return mergedCells;
}

template <typename Cell>
void GeometricTrackerDigitizer::ccl(std::vector<std::vector<Cell>>& mergedCells,
                                    std::unordered_map<size_t, std::pair<Cell, bool>>& cellMap,
                                    size_t index,
                                    size_t nBins0,
                                    size_t nBins1,
                                    bool commonCorner,
                                    double energyCut) {
  // add current cell to cluster
  auto cellA = cellMap.at(index).first;
  // check if cell energy is higher than energy threshold to activate the cell
  if (cellA.depositedEnergy() >= energyCut) {
    // add current cell to current cluster
    mergedCells.back().push_back(cellA);
    cellMap.at(index).second = true;
    // go recursively through all neighbours of this cell, if present
    // calculate neighbour indices first
    int iMin = -1;
    int jMin = -nBins0;
    int iMax = 1;
    int jMax = nBins0;
    std::vector<int> neighbourIndices;
    ///  auto startCheckEdges = std::chrono::system_clock::now();
    // the neighbour indices - filled depending on merging case
    if ((index % nBins0) == 0) {
      // left edge case
      if (commonCorner) {
        neighbourIndices = {jMin, jMin + iMax, iMax, jMax, jMax + iMax};
      } else {
        neighbourIndices = {jMin, iMax, jMax};
      }
    } /*else if (index <= (nBins0 - 1)) {
      // upper edge case
      if (commonCorner) {
        neighbourIndices = {iMin, iMax, jMax + iMin, jMax, jMax + iMax};
      } else {
        neighbourIndices = {iMin, jMax, iMax};
      }
    }*/ else if (((index + 1) % nBins0) == 0) {
      // right edge case
      if (commonCorner) {
        neighbourIndices = {jMin + iMin, jMin, iMin, jMax + iMin, jMax};
      } else {
        neighbourIndices = {jMin, iMin, jMax};
      }
    } /*else if (index >= ((nBins0 * nBins1) - nBins0)) {
      // lower edge case
      if (commonCorner) {
        neighbourIndices = {jMin + iMin, jMin, jMin + iMax, iMin, iMax};
      } else {
        neighbourIndices = {iMin, jMin, iMax};
      }
    }*/ else {
      if (commonCorner) {
        neighbourIndices = {jMin + iMin, jMin, jMin + iMax, iMin, iMax, jMax + iMin, jMax, jMax + iMax};
      } else {
        neighbourIndices = {jMin, iMin, iMax, jMax};
      }
    }
    ///   auto endCheckEdges = std::chrono::system_clock::now();
    ///   timeCheckEdges += (endCheckEdges - startCheckEdges);
    // go through neighbours and recursively call connected component algorithm
    for (auto& i : neighbourIndices) {
      // calculate neighbour index of current cell
      int neighbourIndex = int(index) + i;
      // check if neighbour is there
      ///   auto startSearchMap = std::chrono::system_clock::now();
      auto search = cellMap.find(neighbourIndex);
      ///   auto endSearchMap = std::chrono::system_clock::now();
      ///  timeSearchMap += (endSearchMap - startSearchMap);
      if ((search != cellMap.end())) {
        // get the corresponding index and call function again
        auto newIndex = search->first;
        if (!cellMap.at(newIndex).second) {
          ccl(mergedCells, cellMap, newIndex, nBins0, nBins1, commonCorner, energyCut);
        }  // check if was used already
      }    // check if neighbour is there
    }      // go through neighbour indics
  }        // check energy cut
}

StatusCode GeometricTrackerDigitizer::finalize() {

  /// std::cout << "timeMerge: " << timeMerge.count() << std::endl;
  ///  std::cout << "timeFillLoop: " << timeFillLoop.count() << std::endl;
  /// std::cout << "timeFillMap: " << timeFillMap.count() << std::endl;
  /// std::cout << "timeSearchMap: " << timeSearchMap.count() << std::endl;
  /// std::cout << "timeCheckEdges: " << timeCheckEdges.count() << std::endl;
  /// std::cout << "timeLabelLoop: " << timeLabelLoop.count() << std::endl;
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
