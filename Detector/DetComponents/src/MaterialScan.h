#include "DetInterface/IGeoSvc.h"

#include "GaudiKernel/Service.h"

class MaterialScan: public Service {
public:
  explicit MaterialScan(const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual ~MaterialScan() {};

private:
  std::string m_filename;
  SmartIF<IGeoSvc> m_geoSvc;
  double m_etaBinning;
  double m_etaMax;
};
