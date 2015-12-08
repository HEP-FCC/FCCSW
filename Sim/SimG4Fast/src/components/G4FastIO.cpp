#include "G4FastIO.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// albers
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCAssociationCollection.h"

DECLARE_COMPONENT(G4FastIO)

G4FastIO::G4FastIO(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent) {
   declareInterface<IG4IOTool>(this);
   declareOutput("particles", m_recphandle);
   declareOutput("particleassociation", m_partassociationhandle);
   declareProperty("tmp", m_tmp);
}

G4FastIO::~G4FastIO() {}

StatusCode G4FastIO::initialize() {
   if(GaudiTool::initialize().isFailure()) {
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

StatusCode G4FastIO::saveOutput() {
  return StatusCode::SUCCESS;
}
