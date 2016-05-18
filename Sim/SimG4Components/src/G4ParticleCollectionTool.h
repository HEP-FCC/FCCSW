#ifndef SIMG4COMPONENTS_G4ParticleCollectionTool_H
#define SIMG4COMPONENTS_G4ParticleCollectionTool_H

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "FWCore/DataHandle.h"

#include "SimG4Interface/IG4ParticleGeneratorTool.h"

#include "G4VUserPrimaryGeneratorAction.hh"

// datamodel
namespace fcc {
class MCParticleCollection;
}

/** @class G4ParticleCollectionTool G4ParticleCollectionTool.h "G4ParticleCollectionTool.h"
*  
*  Tool for the concrete implementation of a Particle Generator class
* 
*  @author Andrea Dell'Acqua
*  @date   2016-01-11
*/
class G4ParticleCollectionTool : public GaudiTool, virtual public IG4ParticleGeneratorTool {
public:
/// Standard constructor
	G4ParticleCollectionTool( const std::string& type , const std::string& name,
				     const IInterface* parent ) ;
                        
	virtual ~G4ParticleCollectionTool( ); ///< Destructor

/// Initialize method
	virtual StatusCode initialize( ) ;
                                           

/** Implements 
                                              */
											  
	virtual G4VUserPrimaryGeneratorAction* getParticleGenerator();
	
private:

	/// Handle for the EDM MC particles to be read
  	DataHandle<fcc::MCParticleCollection> m_genParticles;

	G4VUserPrimaryGeneratorAction* m_partGenerator;
	
};


#endif
