#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/Service.h"

/** @class MaterialScan Detector/DetComponents/src/MaterialScan.h MaterialScan.h
 *
 *  Service that facilitates material scan on initialize
 *  This service outputs a ROOT file containing a TTree with radiation lengths and material thickness
 *  For an example on how to read the file, see Examples/scripts/material_plots.py
 *
 *  @author J. Lingemann
 */

class MaterialScan: public Service {
public:
  explicit MaterialScan(const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual ~MaterialScan() {};

private:
  /// name of the output file
  std::string m_filename;
  /// Handle to the geometry service from which the detector is retrieved
  SmartIF<IGeoSvc> m_geoSvc;
  /// Step size in eta
  double m_etaBinning;
  /// Maximum eta until which to scan, scan is performed from -m_etaMax to +m_etaMax
  double m_etaMax;
};
