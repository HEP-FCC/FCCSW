#ifndef SIMG4INTERFACE_ISIMG4PARTICLEHISTORY_H
#define SIMG4INTERFACE_ISIMG4PARTICLEHISTORY_H

// Forward declarations
class G4Track;

/** @class ISimG4ParticleHistory SimG4Interface/SimG4Interface/ISimG4ParticleHistory.h ISimG4ParticleHistory.h
 *
 *  Abstract interface for a particle history container
 *
 *  @author J. Lingemann
 */

class ISimG4ParticleHistory {
public:
  virtual void addParticle(G4Track* aSecondary) = 0;
};

#endif /* SIMG4INTERFACE_ISIMG4PARTICLEHISTORY_H */
