#ifndef SIM_GDMLDETECTORCONSTRUCTION_H
#define SIM_GDMLDETECTORCONSTRUCTION_H

// Geant
#include "G4VUserDetectorConstruction.hh"
#include "G4GDMLParser.hh"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace sim {
class GdmlDetectorConstruction: public G4VUserDetectorConstruction {
public:
  explicit GdmlDetectorConstruction(const std::string& aFileName);
  virtual ~GdmlDetectorConstruction();

  virtual G4VPhysicalVolume* Construct() override;
  virtual void ConstructSDandField() override;
private:
  /// name of the GDML file
  std::string m_fileName;
  /// Parser of the GDML file
  G4GDMLParser m_parser;
  /// Message Service
  ServiceHandle<IMessageSvc> m_msgSvc;
  /// Message Stream
  MsgStream m_log;
};
}

#endif /* SIM_GDMLDETECTORCONSTRUCTION_H */
