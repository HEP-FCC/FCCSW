#ifndef PARTICLE_INFORMATION_H
#define PARTICLE_INFORMATION_H

// albers
#include "datamodel/MCParticleHandle.h"
#include "datamodel/ParticleHandle.h"

// Geant4
#include "G4VUserPrimaryParticleInformation.hh"

/**
	@brief     Primary particle information
   @details   Describes the information that can be assosiated with a G4PrimaryParticle class object.
 	@author    Anna Zaborowska
*/

class ParticleInformation: public G4VUserPrimaryParticleInformation
{
public:
   /**
      A constructor.
      @param aMC A handle to the MC particle used for the association with the simulated particle.
    */
   ParticleInformation(const MCParticleHandle aMCpart, ParticleHandle aPart);
   virtual ~ParticleInformation();
   virtual void Print() const;
   const MCParticleHandle GetMCParticleHandle() const;
   ParticleHandle GetParticleHandle() const;
private:
   const MCParticleHandle m_mcpart;
   ParticleHandle m_part;
};

#endif

