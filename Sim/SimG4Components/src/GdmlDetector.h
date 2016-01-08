#ifndef SIM_GDMLDETECTOR_H
#define SIM_GDMLDETECTOR_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4DetectorConstruction.h"

// Geant4
#include "G4GDMLParser.hh"

class GdmlDetector: public GaudiTool, virtual public IG4DetectorConstruction {
public:
  GdmlDetector(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~GdmlDetector();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual G4VUserDetectorConstruction* getDetectorConstruction();
private:
  /// name of the GDML file
  std::string m_gdmlFile;
};

#endif /* SIM_GDMLDETECTOR_H */
