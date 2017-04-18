// local
#include "SimG4ConstantMagneticFieldTool.h"

// FCCSW
#include "SimG4Common/ConstantField.h"

// Geant 4
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"
#include "G4TransportationManager.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4ClassicalRK4.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4NystromRK4.hh"
#include "G4PropagatorInField.hh"

// Declaration of the Tool
DECLARE_COMPONENT(SimG4ConstantMagneticFieldTool)

SimG4ConstantMagneticFieldTool::SimG4ConstantMagneticFieldTool(const std::string& type, const std::string& name,
                                                               const IInterface* parent)
    : GaudiTool(type, name, parent), m_field(nullptr) {
  declareInterface<ISimG4MagneticFieldTool>(this);
}

SimG4ConstantMagneticFieldTool::~SimG4ConstantMagneticFieldTool() {
  if (nullptr != m_field) delete m_field;
}

StatusCode SimG4ConstantMagneticFieldTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if (m_fieldOn) {
    G4TransportationManager* transpManager = G4TransportationManager::GetTransportationManager();
    G4FieldManager* fieldManager = transpManager->GetFieldManager();
    G4PropagatorInField* propagator = transpManager->GetPropagatorInField();

    // The field manager keeps an observing pointer to the field, ownership stays with this tool. (Cleaned up in dtor)
    m_field =
        new sim::ConstantField(m_fieldComponentX, m_fieldComponentY, m_fieldComponentZ, m_fieldRadMax, m_fieldZMax);
    fieldManager->SetDetectorField(m_field);

    fieldManager->CreateChordFinder(m_field);
    G4ChordFinder* chordFinder = fieldManager->GetChordFinder();
    chordFinder->GetIntegrationDriver()->RenewStepperAndAdjust(stepper(m_integratorStepper, m_field));

    propagator->SetLargestAcceptableStep(m_maxStep);

    if (m_deltaChord > 0) fieldManager->GetChordFinder()->SetDeltaChord(m_deltaChord);
    if (m_deltaOneStep > 0) fieldManager->SetDeltaOneStep(m_deltaOneStep);
    if (m_minEps > 0) fieldManager->SetMinimumEpsilonStep(m_minEps);
    if (m_maxEps > 0) fieldManager->SetMaximumEpsilonStep(m_maxEps);
  }
  return sc;
}

StatusCode SimG4ConstantMagneticFieldTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}

const G4MagneticField* SimG4ConstantMagneticFieldTool::field() const { return m_field; }

G4MagIntegratorStepper* SimG4ConstantMagneticFieldTool::stepper(const std::string& name, G4MagneticField* field) const {
  G4Mag_UsualEqRhs* fEquation = new G4Mag_UsualEqRhs(field);
  if (name == "HelixImplicitEuler")
    return new G4HelixImplicitEuler(fEquation);
  else if (name == "HelixSimpleRunge")
    return new G4HelixSimpleRunge(fEquation);
  else if (name == "HelixExplicitEuler")
    return new G4HelixExplicitEuler(fEquation);
  else if (name == "NystromRK4")
    return new G4NystromRK4(fEquation);
  else if (name == "ClassicalRK4")
    return new G4ClassicalRK4(fEquation);
  else {
    error() << "Stepper " << name << " not available! returning NystromRK4!" << endmsg;
    return new G4NystromRK4(fEquation);
  }
}
