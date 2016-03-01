#ifndef SIMG4COMPONENTS_G4ConstantMagneticFieldTool_H
#define SIMG4COMPONENTS_G4ConstantMagneticFieldTool_H

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "SimG4Interface/IG4MagneticFieldTool.h"

#include "G4MagneticField.hh"
#include "G4SystemOfUnits.hh"
class G4MagIntegratorStepper;
class G4ConstantField;

/** @class G4ConstantMagneticFieldTool G4ConstantMagneticFieldTool.h "G4ConstantMagneticFieldTool.h"
*  
*  Tool for the concrete implementation of a field class
* 
*  @author Andrea Dell'Acqua
*  @date   2016-02-22
*/

class ConstantField: public G4MagneticField
{
friend class G4ConstantMagneticFieldTool;

private:	
	double bX=0;
	double bY=0;
	double bZ=6.*tesla;
	
	double rMax=9*m;
	double zMax=8*m;
	
public:
	void GetFieldValue(const G4double point[4],double* bField ) const
	{
		if (std::sqrt(point[0]*point[0]+point[1]*point[1])<rMax &&
		    std::abs(point[2])<zMax)
		{
			bField[0]=bX;
			bField[1]=bY;
			bField[2]=bZ;
		}
		else bField[0]=bField[1]=bField[2]=0;
	}
};

class G4ConstantMagneticFieldTool : public GaudiTool, virtual public IG4MagneticFieldTool {
public:
/// Standard constructor
	G4ConstantMagneticFieldTool( const std::string& type , const std::string& name,
				     const IInterface* parent ) ;
                        
	virtual ~G4ConstantMagneticFieldTool( ); ///< Destructor

/// Initialize method
	virtual StatusCode initialize( ) ;
/// Finalize method
	virtual StatusCode finalize( ) ;
                                           

/** Implements 
                                              */
											  
	virtual G4MagneticField* getField() const;
	G4MagIntegratorStepper* GetStepper(std::string, G4MagneticField* ) const;
private:

	G4ConstantField* m_field;
	
	bool m_fieldOn=true;
	double m_minEps=0;
	double m_maxEps=0;
	double m_deltaChord=0;
	double m_deltaOneStep=0;
	std::string m_integratorStepper="NystromRK4";
	
	double m_fieldComponentX=0;
	double m_fieldComponentY=0;
	double m_fieldComponentZ=0;
	double m_fieldRadMax=0;
	double m_fieldZMax=0;
	
};

#endif
