#ifndef SIMG4COMPONENTS_G4BFIELDTOOL_H
#define SIMG4COMPONENTS_G4BFIELDTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/ISimG4MagneticFieldTool.h"

// Geant4
#include "G4SystemOfUnits.hh"

#include "MagneticField/IBFieldSvc.h"

// Forward declarations:
// Geant 4 classes
class G4MagIntegratorStepper;

// FCCSW
namespace sim {
class BFieldG4;
}

/** @class SimG4BFieldTool SimG4Components/src/SimG4BFieldTool.h
* SimG4BFieldTool.h
*
*  Implementation of ISimG4MagneticFieldTool using internally the given magnetic field service following the interface
* IBFieldSvc
*
*  @author Julia Hrdinka
*  @date   2018-01-31
*/

class SimG4BFieldTool : public GaudiTool, virtual public ISimG4MagneticFieldTool {
public:
  /// Standard constructor
  SimG4BFieldTool(const std::string& type, const std::string& name, const IInterface* parent);

  /// Destructor
  virtual ~SimG4BFieldTool();

  /// Initialize method
  virtual StatusCode initialize() final;

  /// Finalize method
  virtual StatusCode finalize() final;

  /// Get the magnetic field
  /// @return pointer to G4MagneticField
  virtual const G4MagneticField* field() const final;

  /// Get the stepper
  /// @return pointer to G4MagIntegratorStepper (ownership is transferred to the caller)
  G4MagIntegratorStepper* stepper(const std::string&, G4MagneticField*) const;

private:
  /// Pointer to the actual Geant 4 magnetic field
  sim::BFieldG4* m_field;
  /// Handle to the magnetic field service
  ServiceHandle<IBFieldSvc> m_bFieldSvc{this, "bFieldSvc", "", "the magnetic field service"};
  /// Switch to turn field on or off (default is off). Set with property FieldOn
  Gaudi::Property<bool> m_fieldOn{this, "FieldOn", false, "Switch to turn field off"};
  /// Minimum epsilon (relative error of position / momentum, see G4 doc for more details). Set with property
  /// MinimumEpsilon
  Gaudi::Property<double> m_minEps{this, "MinimumEpsilon", 0, "Minimum epsilon (see G4 documentation)"};
  /// Maximum epsilon (relative error of position / momentum, see G4 doc for more details). Set with property
  /// MaximumEpsilon
  Gaudi::Property<double> m_maxEps{this, "MaximumEpsilon", 0, "Maximum epsilon (see G4 documentation)"};
  /// This parameter governs accuracy of volume intersection, see G4 doc for more details. Set with property DeltaChord
  Gaudi::Property<double> m_deltaChord{this, "DeltaChord", 0, "Missing distance for the chord finder"};
  /// This parameter is roughly the position error which is acceptable in an integration step, see G4 doc for details.
  /// Set with property DeltaOneStep
  Gaudi::Property<double> m_deltaOneStep{this, "DeltaOneStep", 0, "Delta(one-step)"};
  /// Upper limit of the step size, see G4 doc for more details. Set with property MaximumStep
  Gaudi::Property<double> m_maxStep{this, "MaximumStep", 1. * m, "Maximum step length in field (see G4 documentation)"};
  /// Name of the integration stepper, defaults to NystromRK4.
  Gaudi::Property<std::string> m_integratorStepper{this, "IntegratorStepper", "NystromRK4", "Integrator stepper name"};
  /// Size of the field in radial direction. Set with property FieldRMax
  Gaudi::Property<double> m_fieldRadMax{this, "FieldRMax", 6 * m, "Field max radius"};
  /// Size of the field along the beam line. Set with property FieldZMax
  Gaudi::Property<double> m_fieldZMax{this, "FieldZMax", 20. * m, "Field max Z"};
};

#endif  // SIMG4COMPONENTS_G4BFIELDTOOL_H
