#ifndef GEANT_PARTICLE_INFORMATION_H
#define GEANT_PARTICLE_INFORMATION_H

// albers
#include "datamodel/MCParticleHandle.h"
#include "datamodel/ParticleHandle.h"

// Geant4
#include "G4VUserPrimaryParticleInformation.hh"

/**
	@brief     Primary particle information
   @details   Describes the information that can be assosiated with a G4PrimaryParticle class object. It contains handles to the EDM particles.
 	@author    Anna Zaborowska
*/

class ParticleInformation: public G4VUserPrimaryParticleInformation {
public:
   /**
      A constructor.
      @param aMCpart A handle to the EDM MCParticle.
      @param aPart A handle to the EDM Particle.
    */
   ParticleInformation(const MCParticleHandle& aMCpart, ParticleHandle& aPart);
   virtual ~ParticleInformation();
   virtual void Print() const;
   const MCParticleHandle& GetMCParticleHandle() const;
   ParticleHandle& GetParticleHandle();
private:
   /// Handle to the MC particle
   const MCParticleHandle m_mcpart;
   /// Handle to the particle associated with a MCParticle. It is meant to be filled with information on particle momentum etc. at the end of Geant processing (done in SaveParticlesTrackingAction::PostUserTrackingAction).
   ParticleHandle m_part;
};

#endif

