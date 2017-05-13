#include "TestGeometry/GdmlDetectorConstruction.h"

// FCCSW
#include "TestGeometry/TestCalorimeterSD.h"

// Geant4
#include "G4SDManager.hh"

namespace test {
GdmlDetectorConstruction::GdmlDetectorConstruction(const std::string& aFileName)
    : m_msgSvc("MessageSvc", "GdmlDetectorConstruction"),
      m_log(&(*m_msgSvc), "GdmlDetectorConstruction"),
      m_fileName(aFileName) {
  m_parser.Read(m_fileName);
}

GdmlDetectorConstruction::~GdmlDetectorConstruction() {}

G4VPhysicalVolume* GdmlDetectorConstruction::Construct() { return m_parser.GetWorldVolume(); }

void GdmlDetectorConstruction::ConstructSDandField() {
  // Example from Geant4 examples/extended/persistency/gdml/G04
  // First create and register sensitive detectors:
  std::string caloSDname = "ECal";
  TestCalorimeterSD* caloSD = new TestCalorimeterSD(caloSDname, 11);
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  SDman->AddNewDetector(caloSD);
  const G4GDMLAuxMapType* auxMap = m_parser.GetAuxMap();
  for (auto& entry : *auxMap) {
    for (auto& info : entry.second) {
      if (info.type == "SensDet") {
        // attach SD registered above
        m_log << MSG::INFO << "Attaching a sensitive detector: " << info.value
              << " to volume: " << entry.first->GetName() << endmsg;
        G4VSensitiveDetector* mydet = SDman->FindSensitiveDetector(info.value);
        if (mydet) {
          entry.first->SetSensitiveDetector(mydet);
          entry.first->GetSolid()->DumpInfo();
        } else {
          m_log << MSG::WARNING << "GDML contains sensitive detector of type: " << info.value << " but it is not found"
                << endmsg;
        }
      }
    }
  }
}
}
