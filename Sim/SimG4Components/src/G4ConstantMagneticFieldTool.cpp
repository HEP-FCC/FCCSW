
// Include files 

// local
#include "G4ConstantMagneticFieldTool.h"

#include "SimG4Common/G4ConstantField.h"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4ChordFinder.hh"
#include "G4PropagatorInField.hh"


//-----------------------------------------------------------------------------
// Implementation file for class : G4ConstantMagneticFieldTool
//
// 2014-10-01 : Andrea Dell'Acqua
//-----------------------------------------------------------------------------

// Declaration of the Tool
DECLARE_COMPONENT( G4ConstantMagneticFieldTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4ConstantMagneticFieldTool::G4ConstantMagneticFieldTool( const std::string& type,
                                    const std::string& nam,const IInterface* parent )
  				  : GaudiTool ( type, nam , parent ),m_field(0)
{
	declareInterface< IG4MagneticFieldTool >( this ) ;
	
	declareProperty( "FieldOn",m_fieldOn,"Switch to turn field off");
  	declareProperty( "DeltaChord" , m_deltaChord , "Missing distance for the chord finder" );
  	declareProperty( "DeltaOneStep" , m_deltaOneStep , "Delta(one-step)" );
  	declareProperty( "MinimumEpsilon" , m_minEps , "Minimum epsilon (see G4 documentation)" );
  	declareProperty( "MaximumEpsilon" , m_maxEps , "Maximum epsilon (see G4 documentation)" );
  	declareProperty( "MaximumStep" , m_maxStep , "Maximum step length in field (see G4 documentation)" );

  	declareProperty( "IntegratorStepper" , m_integratorStepper="NystromRK4","Integrator stepper name" );
	declareProperty( "FieldComponentX",m_fieldComponentX,"Field X component");
	declareProperty( "FieldComponentY",m_fieldComponentY,"Field Y component");
	declareProperty( "FieldComponentZ",m_fieldComponentZ,"Field Z component");
	declareProperty( "FieldRMax",m_fieldRadMax,"Field max radius");
	declareProperty( "FieldZMax",m_fieldZMax,"field max Z");

}

//=============================================================================
// Destructor 
//=============================================================================

G4ConstantMagneticFieldTool::~G4ConstantMagneticFieldTool()
{
}

//=============================================================================
// Initialize 
//=============================================================================
StatusCode G4ConstantMagneticFieldTool::initialize( )
{
	info()<<" G4ConstantMagneticFieldTool::initialize( ) "<<endmsg;
	StatusCode sc = GaudiTool::initialize( ) ;
	if ( sc.isFailure() ) return sc ;
	
	if (m_fieldOn) 
	{
		m_field = new G4ConstantField();
		if (m_fieldZMax) m_field->zMax=m_fieldZMax;
		if (m_fieldRadMax) m_field->rMax=m_fieldRadMax;
		if (m_fieldComponentX) m_field->bX=m_fieldComponentX;
		if (m_fieldComponentY) m_field->bX=m_fieldComponentY;
		if (m_fieldComponentZ) m_field->bX=m_fieldComponentZ;
		
		G4TransportationManager* transpManager=G4TransportationManager::GetTransportationManager();
		G4FieldManager* fieldManager=transpManager->GetFieldManager();
		G4PropagatorInField* propagator=transpManager->GetPropagatorInField();

		fieldManager->SetDetectorField(m_field);

		fieldManager->CreateChordFinder(m_field);
		G4ChordFinder* chordFinder=fieldManager->GetChordFinder();
		chordFinder->GetIntegrationDriver()
			->RenewStepperAndAdjust( GetStepper(m_integratorStepper,m_field) );

		propagator->SetLargestAcceptableStep(m_maxStep);

		if (m_deltaChord>0) fieldManager->GetChordFinder()->SetDeltaChord(m_deltaChord);
		if (m_deltaOneStep>0) fieldManager->SetDeltaOneStep(m_deltaOneStep);
		if (m_minEps>0) fieldManager->SetMinimumEpsilonStep(m_minEps);
		if (m_maxEps>0) fieldManager->SetMaximumEpsilonStep(m_maxEps);
	}
	info()<<" done!!! G4ConstantMagneticFieldTool::initialize( ) "<<endmsg;
	return sc;
}


//=============================================================================
// Finalize 
//=============================================================================
StatusCode G4ConstantMagneticFieldTool::finalize( )
{
	info()<<" G4ConstantMagneticFieldTool::finalize( ) "<<endmsg;
	StatusCode sc = GaudiTool::finalize( ) ;
	return sc;
}

G4MagneticField* G4ConstantMagneticFieldTool::getField() const
{
	return m_field;
}

#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4NystromRK4.hh"
#include "G4ClassicalRK4.hh"

G4MagIntegratorStepper* G4ConstantMagneticFieldTool::GetStepper(std::string name, G4MagneticField* field) const
{
	G4Mag_UsualEqRhs* fEquation= new G4Mag_UsualEqRhs(field); 
	if (name=="HelixImplicitEuler") return new G4HelixImplicitEuler( fEquation);
	else if (name=="HelixSimpleRunge") return new G4HelixSimpleRunge( fEquation);
	else if (name=="HelixExplicitEuler") return new G4HelixExplicitEuler( fEquation);
	else if (name=="NystromRK4") return new G4NystromRK4( fEquation);
	else if (name=="ClassicalRK4") return new G4ClassicalRK4( fEquation);
        else 
	{
		error()<< "Stepper "<<name<<" not available! returning NystromRK4!"<<endmsg;
		return new G4NystromRK4( fEquation);
	}
}
