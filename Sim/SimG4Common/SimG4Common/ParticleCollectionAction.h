#ifndef SIMG4COMMON_ParticleCollectionAction_H
#define SIMG4COMMON_ParticleCollectionAction_H

#include "G4VUserPrimaryGeneratorAction.hh"

#include "FWCore/DataHandle.h"

class G4Event;

// datamodel
namespace fcc {
class MCParticleCollection;
}

class ParticleCollectionAction: public G4VUserPrimaryGeneratorAction {
public:
	ParticleCollectionAction();
	~ParticleCollectionAction() {;}
	
	void GeneratePrimaries(G4Event* anEvent);
	
	void SetPrimaryCollection( DataHandle<fcc::MCParticleCollection>* );
	
private:
	DataHandle<fcc::MCParticleCollection>* m_particles;
};

#endif
