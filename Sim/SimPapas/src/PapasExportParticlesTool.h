#ifndef SIM_PAPASEXPORTPARTICLES_H
#define SIM_PAPASEXPORTPARTICLES_H

// Gaudi
#include "FWCore/DataHandle.h"
// FCCSW
#include "GaudiAlg/GaudiTool.h"
#include "SimPapas/IPapasExportTool.h"
// FCCEDM
#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"

#include <string>

/** @class PapasExportParticlesTool Sim/SimPapas/src/PapasExportParticlesTool.h PapasExportParticlesTool.h
 *
 *  Takes papasparticles stored in the papas::Event and converts to MCParticles.
 *  @author A.J. Robson
 */
class PapasExportParticlesTool : public GaudiTool, virtual public IPapasExportTool {
public:
  /// Constructor.
  PapasExportParticlesTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  /// Initialize.
  virtual StatusCode initialize();
  /// Create any output structures needed.
  virtual StatusCode createOutputStructures();
  /// Execute papas tool.
  virtual StatusCode run(papas::Event& pevent, std::unordered_map<papas::Identifier, int>& links);
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// handle for reconstructed particles collection
  DataHandle<fcc::ParticleCollection> m_recParticlesHandle{"recparticles", Gaudi::DataHandle::Writer, this};
  /// Handle for the ParticleCollection to be associated with
  DataHandle<fcc::MCParticleCollection> m_genParticlesHandle{"MCparticles", Gaudi::DataHandle::Reader, this};
  /// Handle for the ParticleMCParticlAdssociationCollection to be written
  DataHandle<fcc::ParticleMCParticleAssociationCollection> m_associationHandle{"particleMCparticleAssociations",
                                                                               Gaudi::DataHandle::Writer, this};

  /// Particles type to be exported
  Gaudi::Property<std::string> m_particleSubtype{this, "particleSubtype", "", "particle subtype"};
  /// pointer to the reconstructed particles collection
  fcc::ParticleCollection* m_particles;                                            /// mcparticles collection
  fcc::ParticleMCParticleAssociationCollection* m_particleMCParticleAssociations;  /// mcparticles collection
};

#endif  // SIM_PAPASPARTICLEEXPORTER_H
