#ifndef _ANALYSE_TEST_H_
#define _ANALYSE_TEST_H_

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"

// albers
#include "datamodel/ParticleCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/ParticleMCAssociationCollection.h"


class Analyse: public GaudiAlgorithm {
   friend class AlgFactory<Analyse> ;
public:
   /// Constructor.
   Analyse(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Execute.
   virtual StatusCode execute();
   /// Finalize.
   virtual StatusCode finalize();
   enum SimType { FULL, FAST };
private:
   /// Handle for the EDM MC vertices to be read
   DataHandle<GenVertexCollection> m_genvhandle;
   /// Handle for the EDM MC particles to be read
   DataHandle<MCParticleCollection> m_genphandle;
   /// Handle for the particles to be written
   DataHandle<ParticleCollection> m_recphandle;
   /// Handle for the associations between particles and MC particles to be written
   DataHandle<ParticleMCAssociationCollection> m_partassociationhandle;
   /// Pointer to the interface of geometry service
};

#endif
