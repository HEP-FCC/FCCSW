#define ALLOW_ALL_TYPES
#include <cstdlib>
#include <vector>

#include "albers/Registry.h"
#include "DataObjects/Jet.h"
#include "DataObjects/JetHandle.h"

#include "DataObjects/Particle.h"
#include "DataObjects/ParticleHandle.h"

#include "DataObjects/MCParticle.h"
#include "DataObjects/MCParticleHandle.h"

#include "DataObjects/JetParticle.h"
#include "DataObjects/JetParticleHandle.h"

#include "DataObjects/GenJet.h"

#include "DataObjects/EventInfo.h"
#include "DataObjects/EventInfoHandle.h"

namespace DataModelDict {

  std::vector<Jet> jet;
  std::vector<GenJet> genjet;
  std::vector<EventInfo> eventinfo;
  std::vector<Particle> particle;
  std::vector<MCParticle> mcparticle;
  std::vector<JetParticle> jetparticle;
  BareParticle bareparticle;
  BareJet      barejet;
}
