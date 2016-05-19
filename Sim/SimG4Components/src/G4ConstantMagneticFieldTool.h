#ifndef SIMG4COMPONENTS_G4ConstantMagneticFieldTool_H
#define SIMG4COMPONENTS_G4ConstantMagneticFieldTool_H

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "SimG4Interface/IG4MagneticFieldTool.h"

class G4MagIntegratorStepper;
class G4ConstantField;

/** @class G4ConstantMagneticFieldTool G4ConstantMagneticFieldTool.h "G4ConstantMagneticFieldTool.h"
*
*  Implementation of IG4MagneticFieldTool that generates a constant field
*
*  @author Andrea Dell'Acqua
*  @date   2016-02-22
*/

class G4ConstantMagneticFieldTool : public GaudiTool, virtual public IG4MagneticFieldTool {
public:
  /// Standard constructor
  G4ConstantMagneticFieldTool( const std::string& type , const std::string& name,
             const IInterface* parent ) ;

  /// Destructor
  virtual ~G4ConstantMagneticFieldTool( );

  /// Initialize method
  virtual StatusCode initialize( ) final;

  /// Finalize method
  virtual StatusCode finalize( ) final;

  /// Get the actual G4MagneticField
  virtual const G4MagneticField* field() const final;
  /// Get the stepper, ownership is transferred to the caller
  G4MagIntegratorStepper* stepper(std::string, G4MagneticField* ) const;

private:
  /// Pointer to the actual Geant 4 magnetic field
  G4ConstantField* m_field;
  /// Switch to turn field on or off (default is off). Set with property FieldOn
  bool m_fieldOn;
  /// Minimum epsilon (relative error of position / momentum, see G4 doc for more details). Set with property MinimumEpsilon
  double m_minEps;
  /// Maximum epsilon (relative error of position / momentum, see G4 doc for more details). Set with property MaximumEpsilon
  double m_maxEps;
  /// This parameter governs accuracy of volume intersection, see G4 doc for more details. Set with property DeltaChord
  double m_deltaChord;
  /// This parameter is roughly the position error which is acceptable in an integration step, see G4 doc for details.
  /// Set with property DeltaOneStep
  double m_deltaOneStep;
  /// Upper limit of the step size, see G4 doc for more details. Set with property MaximumStep
  double m_maxStep;
  /// Name of the integration stepper, defaults to NystromRK4.
  std::string m_integratorStepper;

  /// Field component in X direction. Set with property FieldComponentX
  double m_fieldComponentX;
  /// Field component in Y direction. Set with property FieldComponentY
  double m_fieldComponentY;
  /// Field component in Z direction. Set with property FieldComponentZ
  double m_fieldComponentZ;
  /// Size of the field in radial direction. Set with property FieldRMax
  double m_fieldRadMax;
  /// Size of the field along the beam line. Set with property FieldZMax
  double m_fieldZMax;
};

#endif
