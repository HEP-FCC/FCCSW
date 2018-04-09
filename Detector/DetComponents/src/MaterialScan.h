#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Service.h"

/** @class MaterialScan Detector/DetComponents/src/MaterialScan.h MaterialScan.h
 *
 *  Service that facilitates material scan on initialize
 *  This service outputs a ROOT file containing a TTree with radiation lengths and material thickness
 *  For an example on how to read the file, see Examples/scripts/material_plots.py
 *
 *  @author J. Lingemann
 */

class MaterialScan : public Service {
public:
  explicit MaterialScan(const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual ~MaterialScan(){};

private:
  /// name of the output file
  Gaudi::Property<std::string> m_filename{this, "filename", "", "file name to save the tree to"};
  /// Handle to the geometry service from which the detector is retrieved
  SmartIF<IGeoSvc> m_geoSvc;
  /// Step size in eta
  Gaudi::Property<double> m_etaBinning{this, "etaBinning", 0.05, "eta bin size"};
  /// Maximum eta until which to scan, scan is performed from -m_etaMax to +m_etaMax
  Gaudi::Property<double> m_etaMax{this, "etaMax", 6, "maximum eta value"};
  /// number of random, uniformly distributed phi values to average over
  Gaudi::Property<double> m_nPhiTrials{this, "nPhiTrials", 100,
                                       "number of random, uniformly distributed phi values to average over"};
  /// Name of the envelope within which the material is measured (by default: world volume)
  Gaudi::Property<std::string> m_envelopeName{this, "envelopeName", "world",
                                              "name of the envelope within which the material is measured"};
  /// Flat random number generator
  Rndm::Numbers m_flatPhiDist;
};
