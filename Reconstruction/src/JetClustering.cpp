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
#include "DataObjects/JetParticleAssociationCollection.h"
#include "DataObjects/GenJetParticleAssociationCollection.h"

typedef JetClustering<ParticleCollection, JetCollection, JetParticleAssociationCollection> ParticleJetClustering;
typedef JetClustering<MCParticleCollection, GenJetCollection, GenJetParticleAssociationCollection> MCParticleGenJetClustering;

DECLARE_COMPONENT(ParticleJetClustering)
DECLARE_COMPONENT_WITH_ID(ParticleJetClustering,"ParticleJetClustering")
DECLARE_COMPONENT(MCParticleGenJetClustering)
DECLARE_COMPONENT_WITH_ID(MCParticleGenJetClustering,"MCParticleGenJetClustering")

