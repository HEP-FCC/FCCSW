#ifndef SIMG4COMPONENTS_G4SingleParticleGeneratorTool_H
#define SIMG4COMPONENTS_G4SingleParticleGeneratorTool_H

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "SimG4Interface/IG4ParticleGeneratorTool.h"

class G4VUserPrimaryGeneratorAction;

/** @class G4SingleParticleGeneratorTool G4SingleParticleGeneratorTool.h "G4SingleParticleGeneratorTool.h"
*  
*  Tool for the concrete implementation of a Particle Generator class
* 
*  @author Andrea Dell'Acqua
*  @date   2014-10-01
*/
class G4SingleParticleGeneratorTool : public GaudiTool, virtual public IG4ParticleGeneratorTool {
public:
/// Standard constructor
	G4SingleParticleGeneratorTool( const std::string& type , const std::string& name,
				     const IInterface* parent ) ;
                        
	virtual ~G4SingleParticleGeneratorTool( ); ///< Destructor

/// Initialize method
	virtual StatusCode initialize( ) ;
/// Finalize method
	virtual StatusCode finalize( ) ;
                                           

/** Implements 
                                              */
											  
	virtual G4VUserPrimaryGeneratorAction* getParticleGenerator() ;
	
private:

	G4VUserPrimaryGeneratorAction* m_partGenerator;
	
	double eMin,eMax;
	double etaMin,etaMax;
	double phiMin,phiMax;
	double vX,vY,vZ;
	std::string particleName;
	
};

#endif
