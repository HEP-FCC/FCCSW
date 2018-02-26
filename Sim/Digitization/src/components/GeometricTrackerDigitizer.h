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

/** @class GeometricTrackerDigitizer Digitization/src/components/GeometricTrackerDigitizer.h GeometricTrackerDigitizer.h
 *
 *  @brief Geometric digitization of track hits.
 *
 *  The GeometricTrackerDigitizer geometrically digitizes fcc::DigiTrackHitAssociationCollection (which is a positioned
 * track hit +
 * post step point) created by simulation and writes out a fcc::TrackClusterCollection.
 * It internally uses Acts Digitization tools.
 * The user can decide whether analogue or digital readout should be simulated and can set smear and cut paameters and
 * the hit inefficiency. Especially in the FCChh environment we do expect to also have merged clusters which are not
 * only
 * created by one single particle. The user can decide if the mergin of clusters should happen, if hit cells share a
 * common edge (default) or even if they already share a common corner.
 * For analysis reasons one can also write out the single particle clusters and the Acts clusters (Turned off per
 * default).
 *
 *  @author Julia Hrdinka
 *  @date   2017-04
 */

namespace Acts {
class PlanarModuleStepper;
class DetectorElementBase;
class PlanarModuleCluster;
class DigitizationModule;
class Surface;
class DigitizationCell;
}

namespace fcc {
class DigiTrackHitAssociationCollection;
class TrackClusterCollection;
class TrackHitCollection;
}

class IRndmGenSvc;

namespace sim {
class FCCDigitizationCell;
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
  /// the collection of output single particle clusters [optional]
  DataHandle<fcc::TrackClusterCollection> m_singleTrackClusters{"singleTrackClusters", Gaudi::DataHandle::Writer, this};
  /// the collection of output single particle cluster track hits [optional]
  DataHandle<fcc::TrackHitCollection> m_singleTrackHits{"singleClusterTrackHits", Gaudi::DataHandle::Writer, this};
  /// Handle to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Handle to the tracking geometry service
  ServiceHandle<ITrackingGeoSvc> m_trkGeoSvc;
  /// Inefficiency for full hits
  Gaudi::Property<double> m_hitInefficiency{this, "hitInefficiency", 0.01, "The hit inefficiency."};
  /// Cut parameter - percentage of thickness to be traversed
  Gaudi::Property<double> m_cutParameter{this, "cutParameter", 0.,
                                         "Cut parameter - percentage of thickness to be traversed."};
  /// amount of smearing
  Gaudi::Property<double> m_smearParameter{this, "smearParameter", 0.1,
                                           "Amount of smearing - accounts for n % gaussian width."};
  /// flag indicating if analog readout should be used (default is digital)
  Gaudi::Property<bool> m_analogReadout{
      this, "analogReadout", false,
      "This flag should be turned on in case analog readout should be simulated. Default is digitital readout."};
  /// flag indicating if cells having a common corner should be merged to a cluster (default: clusters sharing common
  /// edge)
  Gaudi::Property<bool> m_commonCorner{
      this, "commonCorner", false, "This flag should be turned on in cells having a common corner should be merged to "
                                   "a cluster. Per default merging of cells only happens when having a common edge."};
  /// flag indicating if single particle clusters should be written out
  Gaudi::Property<bool> m_singleParticleClusters{
      this, "singleParticleClusters", false,
      "This flag should be turned on case single particle clusters should be written to the edm."};
  /// flag indicating if acts planarclusters should be written out (for studies)
  Gaudi::Property<bool> m_writeActsClusters{
      this, "singleParticleClusters", false,
      "This flag should be turned on in case the Acts::PlanarClusters should be written out."};
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
  /// Planar clusters per event - used for analysis
  std::vector<std::vector<Acts::PlanarModuleCluster>> m_clustersPerEvent;

  /// Private method collecting all hits of the current event per surface
  /// @param hits Collection of track hits + digi information
  /// @return Map of surfaces and their corresponding hits
  const std::map<long long int, fcc::DigiTrackHitAssociationCollection>
  collectHitsOnSurface(const fcc::DigiTrackHitAssociationCollection& hits) const;

  /// Private method to merge the produced Geant4 hits of one partile into one hit per surface
  /// @param hitsPerSurface map of surfaces and their corresponding hits
  /// @return The updated track hits collection
  const fcc::DigiTrackHitAssociationCollection
  mergeHits(const std::map<long long int, fcc::DigiTrackHitAssociationCollection>& hitsPerSurface) const;

  /// Private method creating clusters out of digitization cells
  /// @param cells All digitization cells
  /// @param commonCorner If the cells should be merged if they are sharing a corner (default: they only get merged when
  /// they share a common edge)
  /// @return The digitization cells grouped together in a vector of vectors (each entry of the first vector dimension
  /// represents one cluster; for each cluster there is one vector of cells of which it is composed of)
  /// @note This function internally uses the boost implementation of the connected components algorithm
  const std::vector<std::vector<sim::FCCDigitizationCell>>
  createClusters(const std::vector<sim::FCCDigitizationCell>& cells, bool commonCorner = false) const;
};

#endif  // DIGITIZATION_GEOMETRICTRACKERDIGITIZER_H
