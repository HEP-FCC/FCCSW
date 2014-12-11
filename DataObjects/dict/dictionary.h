#define ALLOW_ALL_TYPES
#include <cstdlib>
#include <vector>

#include "albers/Registry.h"
#include "DataObjects/Jet.h"
#include "DataObjects/JetHandle.h"

#include "DataObjects/BareHit.h"
#include "DataObjects/BareCluster.h"

#include "DataObjects/Particle.h"
#include "DataObjects/ParticleHandle.h"

#include "DataObjects/MCParticle.h"
#include "DataObjects/MCParticleHandle.h"

#include "DataObjects/JetParticleAssociation.h"
#include "DataObjects/JetParticleAssociationHandle.h"

#include "DataObjects/GenJet.h"
#include "DataObjects/GenJetHandle.h"

#include "DataObjects/GenJetParticleAssociation.h"
#include "DataObjects/GenJetParticleAssociationHandle.h"

#include "DataObjects/EventInfo.h"
#include "DataObjects/EventInfoHandle.h"

namespace DataModelDict {

  std::vector<Jet> jet;
  std::vector<GenJet> genjet;
  std::vector<EventInfo> eventinfo;
  std::vector<Particle> particle;
  std::vector<MCParticle> mcparticle;
  JetParticleAssociation jetparticleassoc;
  std::vector<JetParticleAssociation> jetparticleassocs;
  GenJetParticleAssociation genjetparticleassoc;
  std::vector<GenJetParticleAssociation> genjetparticleassocs;
  LorentzVector lorentzvector;
  Point point;
  BareHit barehit;
  BareCluster barecluster;
  BareParticle bareparticle;
  BareJet      barejet;
}
