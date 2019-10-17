#ifndef GENERATION_IPARTICLEGUNTOOL_H
#define GENERATION_IPARTICLEGUNTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "IHepMCProviderTool.h"

/** @class IParticleGunTool IParticleGunTool.h "Generation/IParticleGunTool.h"
 *
 *  Abstract interface to particle gun tool. Generates a single particle.
 *
 *  @author Patrick Robbe
 *  @date   2008-05-18
 */

class IParticleGunTool : virtual public IHepMCProviderTool {
public:
  DeclareInterfaceID(IParticleGunTool, 1, 0);
  typedef std::vector<int> PIDs;

  /** Generates one particle.
   *  @param[out] fourMomentum  four-momentum of the generated particle gun
   *  @param[out] origin        four-momentum of the origin vertex of the particle gun
   *  @param[out] pdgId         pdgId of the generated particle
   */
  virtual void generateParticle(Gaudi::LorentzVector& fourMomentum, Gaudi::LorentzVector& origin, int& pdgId) = 0;

  /// Print various counters at the end of the job
  virtual void printCounters() = 0;
};

#endif
