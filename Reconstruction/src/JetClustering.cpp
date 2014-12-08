#include "JetClustering.h"

// #include "DataObjects/LorentzVector.h"


// #include "TLorentzVector.h"

// #include <vector>

// using namespace fastjet;

#include "DataObjects/ParticleCollection.h"
#include "DataObjects/BareParticle.h"
#include "DataObjects/JetCollection.h"
#include "DataObjects/MCParticleCollection.h"
#include "DataObjects/GenJetCollection.h"

typedef JetClustering<ParticleCollection,JetCollection> ParticleJetClustering;
typedef JetClustering<MCParticleCollection,GenJetCollection> MCParticleGenJetClustering;

DECLARE_COMPONENT(ParticleJetClustering)
DECLARE_COMPONENT_WITH_ID(ParticleJetClustering,"ParticleJetClustering")
DECLARE_COMPONENT(MCParticleGenJetClustering)
DECLARE_COMPONENT_WITH_ID(MCParticleGenJetClustering,"MCParticleGenJetClustering")

