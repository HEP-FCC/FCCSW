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
#include "Math/Math.h"
#include <boost/optional.hpp>

/** @class GeometricTrackerDigitizer
 *
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
  /// the collection of output clusters
  DataHandle<fcc::TrackHitCollection> m_trackHits{"clusterTrackHits", Gaudi::DataHandle::Writer, this};
  /// Handle to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Handle to the tracking geometry service
  ServiceHandle<ITrackingGeoSvc> m_trkGeoSvc;
  /// name of the tracker barrel readout
  Gaudi::Property<std::string> m_readoutBarrelName{this, "readoutBarrelName", "TrackerBarrelReadout",
                                                   "Names of the readout of the tracker barrel"};
  /// name of the tracker endcap readout
  Gaudi::Property<std::string> m_readoutEndcapName{this, "readoutEndcapName", "TrackerEndcapReadout",
                                                   "Names of the readout of the tracker end cap"};
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

  const std::map<long long int, fcc::DigiTrackHitAssociationCollection>
  collectHitsOnSurface(const fcc::DigiTrackHitAssociationCollection& hits) const;

  const fcc::DigiTrackHitAssociationCollection
  mergeHits(const std::map<long long int, fcc::DigiTrackHitAssociationCollection>& hitsPerSurface) const;

  const std::vector<std::vector<sim::FCCDigitizationCell>>
  createClusters(const std::vector<sim::FCCDigitizationCell>& cells, bool commonCorner = false) const;
};

#endif  // DIGITIZATION_GEOMETRICTRACKERDIGITIZER_H
