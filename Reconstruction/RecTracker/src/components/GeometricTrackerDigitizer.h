#ifndef DIGITIZATION_GEOMETRICTRACKERDIGITIZER_H
#define DIGITIZATION_GEOMETRICTRACKERDIGITIZER_H

#include "ACTS/Utilities/Identifier.hpp"
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"
#include "DetInterface/IGeoSvc.h"
#include "DetInterface/ITrackingGeoSvc.h"
#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "RecTracker/IClusterWriter.h"

/** @class GeometricTrackerDigitizer Digitization/src/components/GeometricTrackerDigitizer.h GeometricTrackerDigitizer.h
 *
 *  @brief Geometric digitization of track hits.
 *
 *  The GeometricTrackerDigitizer geometrically digitizes fcc::DigiTrackHitAssociationCollection (which is a positioned
 * track hit + post step point) created by geant4 simulation and writes out a fcc::TrackClusterCollection.
 * It internally uses Acts Digitization tools.
 * The user can decide whether analogue or digital readout should be simulated and can set smear and cut paameters and
 * the hit inefficiency. Especially in the FCChh environment we do expect to also have merged clusters which are not
 * only created by one single particle. The user can decide if the merging of clusters should happen, if hit cells share
 * a common edge (default) or even if they share a common corner.
 * For analysis reasons one can also write out more specific cluster information using a cluster writer tool (Turned off
 * per default).
 *
 * @todo implement energy cut with next ACTS release (when digitization module knows about it)
 * @todo implement analogue/digital readout per module for next ACTS release
 *
 *  @author Julia Hrdinka
 *  @date   2017-04
 */

namespace Acts {
class PlanarModuleStepper;
class DetectorElementBase;
}

namespace fcc {
class DigiTrackHitAssociationCollection;
class TrackClusterCollection;
class TrackHitCollection;
}

class IRndmGenSvc;

namespace sim {
class FCCDigitizationCell;
class FCCPlanarCluster;
}

class GeometricTrackerDigitizer : public GaudiAlgorithm {
  friend class AlgFactory<GeometricTrackerDigitizer>;

public:
  /// Constructor
  GeometricTrackerDigitizer(const std::string& name, ISvcLocator* svcLoc);
  /// Destructor
  ~GeometricTrackerDigitizer() = default;
  /// Gaudi interface initialization method
  virtual StatusCode initialize() final;
  /// Gaudi interface finalize method
  virtual StatusCode finalize() final;
  /// Gaudi interface execute method
  virtual StatusCode execute() final;

private:
  /// the collection of input tracker hits
  DataHandle<fcc::DigiTrackHitAssociationCollection> m_digiTrackHitAssociation{"digiTrackHitAssociation",
                                                                               Gaudi::DataHandle::Reader, this};
  /// the collection of output clusters
  DataHandle<fcc::TrackClusterCollection> m_trackClusters{"trackClusters", Gaudi::DataHandle::Writer, this};
  /// the collection cluster trackhits
  DataHandle<fcc::TrackHitCollection> m_trackHits{"clusterTrackHits", Gaudi::DataHandle::Writer, this};
  // ClusterWriter tool
  ToolHandle<IClusterWriter> m_clusterWriter{"clusterWriter", this};
  /// flag indicating if specific cluster information should be written out
  Gaudi::Property<bool> m_writeClusterInfo{this, "writeClusterInfo", false,
                                           "This flag should be turned on in case more "
                                           "information should be written out for the "
                                           "clusters needed for studies."};
  /// Handle to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Handle to the tracking geometry service
  ServiceHandle<ITrackingGeoSvc> m_trkGeoSvc;
  /// Inefficiency for full hits
  Gaudi::Property<double> m_hitInefficiency{this, "hitInefficiency", 0.00, "The hit inefficiency."};
  /// Cut parameter - percentage of thickness to be traversed
  Gaudi::Property<double> m_cutParameter{this, "cutParameter", 0.0,
                                         "Cut parameter - percentage of thickness to be traversed."};
  /// amount of smearing
  Gaudi::Property<double> m_smearParameter{this, "smearParameter", 0.1,
                                           "Amount of smearing - accounts for n % gaussian width."};
  /// maximum incidence angle cut-off
  Gaudi::Property<double> m_cosThetaLocMin{this, "cosThetaLocMin", 0.0, "Maximum incidence angle cut-off."};
  /// flag indicating if analog readout should be used (default is digital)
  Gaudi::Property<bool> m_analogueReadout{
      this, "analogReadout", false,
      "This flag should be turned on in case analog readout should be simulated. Default is digitital readout."};
  /// flag indicating if cells having a common corner should be merged to a cluster (default: clusters sharing common
  /// edge)
  Gaudi::Property<bool> m_commonCorner{
      this, "commonCorner", true,
      "This flag should be turned off if cells having a common corner should not be merged to "
      "a cluster and only cells sharing a common edge should be in the same cluster."};
  /// flag indicating if single particle clusters should be written out
  Gaudi::Property<bool> m_fastSimInterface{
      this, "fastSimInterface", false,
      "This flag should be turned on in case fast simulation (creating just one hit per sensitive module) is used."};
  /// energy threshold per readout cell (@todo will be a property of the digitization module when using next version of
  /// ACTS)
  Gaudi::Property<double> m_energyThreshold{this, "energyThreshold", 3.6e-6,
                                            "Possibile energy threshold to be reached in order to activate a read out "
                                            "cell. Default: producing 1000 electron-hole pairs(3.6keV)."};
  /// The volume manager
  dd4hep::VolumeManager m_volumeManager;
  /// The planar module stepper needed to step thorugh the module
  std::shared_ptr<Acts::PlanarModuleStepper> m_moduleStepper;
  /// Map containing combining Acts detector elements with the dd4hep volumeID
  std::map<Identifier, const Acts::DetectorElementBase*> m_detectorElements;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for the generation of random noise hits
  Rndm::Numbers m_gauss;
  /// Flat random number generator
  Rndm::Numbers m_flatDist;

  /// @brief create cells from simulated hits
  /// This function receives the simulated hits, which are geant4 steps, of each event from the event store finds all
  /// cells which are activated along the step. These cells are already sorted for each module, because the following
  /// clusterization will be done per module. The cells are stored in a map, sorted by their global index on the module.
  /// @param [in,out] cellsPerSurface
  StatusCode
  createCells(std::unordered_map<long long int, std::unordered_map<size_t, std::pair<sim::FCCDigitizationCell, bool>>>&
                  cellsPerSurface);

  /// @todo this function will sit in ACTS from the next release
  /// @brief create clusters
  /// This function recieves digitization cells and bundles the neighbouring to
  /// create clusters later and does cell merging. Furthermore an energy
  /// cut (excluding cells which fall below threshold) can be applied. The
  /// function is templated on the digitization cell type to allow users to use
  /// their own implementation of Acts::DigitizationCell.
  /// @tparam Cell the digitization cell
  /// @param [in] cellMap map of all cells per cell ID on module
  /// @param [in] nBins0 number of bins in direction 0
  /// @param [in] commonCorner flag indicating if also cells sharing a common
  /// corner should be merged into one cluster
  /// @param [in] energyCut possible energy cut to be applied
  /// @return vector (the different clusters) of vector of digitization cells (the
  /// cells which belong to each cluster)
  template <typename Cell>
  std::vector<std::vector<Cell>> createClusters(std::unordered_map<size_t, std::pair<Cell, bool>>& cellMap,
                                                size_t nBins0,
                                                bool commonCorner = true,
                                                double energyCut = 0.);

  /// @todo this function will sit in ACTS from the next release
  /// @brief fillCluster
  /// This function is a helper function internally used by Acts::createClusters.
  /// It does connected component labelling using a hash map in order to find out
  /// which cells are neighbours. This function is called recursively by all
  /// neighbours of the current cell. The function is templated on the
  /// digitization cell type to allow users to use their own implementation
  /// inheriting from Acts::DigitizationCell.
  /// @tparam Cell the digitization cell
  /// @param [in,out] mergedCells the final vector of cells to which cells of one
  /// cluster should be added
  /// @param [in,out] cellMap the hashmap of all present cells + a flag indicating
  /// if they have been added to a cluster already, with the key being the global
  /// grid index
  /// @param [in] index the current global grid index of the cell
  /// @param [in] cellA the current cell
  /// @param [in] nBins0 number of bins in direction 0
  /// @param [in] commonCorner flag indicating if also cells sharing a common
  /// corner should be merged into one cluster
  /// @param [in] energyCut possible energy cut to be applied
  template <typename Cell>
  void fillCluster(std::vector<std::vector<Cell>>& mergedCells,
                   std::unordered_map<size_t, std::pair<Cell, bool>>& cellMap,
                   size_t index,
                   size_t nBins0,
                   bool commonCorner = true,
                   double energyCut = 0.);
};

#endif  // DIGITIZATION_GEOMETRICTRACKERDIGITIZER_H
