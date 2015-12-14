#ifndef SIM_DD4HEPDETECTOR_H
#define SIM_DD4HEPDETECTOR_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4DetectorConstruction.h"
class IGeoSvc;

class DD4hepDetector: public GaudiTool, virtual public IG4DetectorConstruction {
  public:
  DD4hepDetector(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~DD4hepDetector();

  virtual StatusCode initialize();

  virtual G4VUserDetectorConstruction* getDetectorConstruction();
private:
  SmartIF<IGeoSvc> m_geoSvc;
};

#endif /* SIM_DD4HEPDETECTOR_H */
