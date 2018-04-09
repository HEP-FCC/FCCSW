#include "JetClustering.h"

// #include "DataObjects/LorentzVector.h"

// #include "TLorentzVector.h"

// #include <vector>

// using namespace fastjet;

#include "datamodel/BareParticle.h"
#include "datamodel/GenJetCollection.h"
#include "datamodel/JetCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleCollection.h"

typedef JetClustering<fcc::ParticleCollection, fcc::JetCollection> ParticleJetClustering;
typedef JetClustering<fcc::MCParticleCollection, fcc::GenJetCollection> MCParticleGenJetClustering;

DECLARE_COMPONENT(ParticleJetClustering)
DECLARE_COMPONENT_WITH_ID(ParticleJetClustering, "ParticleJetClustering")
DECLARE_COMPONENT(MCParticleGenJetClustering)
DECLARE_COMPONENT_WITH_ID(MCParticleGenJetClustering, "MCParticleGenJetClustering")
