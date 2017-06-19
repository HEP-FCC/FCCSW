#ifndef SIM_PAPASEXPORTPARTICLES_H
#define SIM_PAPASEXPORTPARTICLES_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiTool.h"
#include "SimPapas/IPapasTool.h"
#include "datamodel/ParticleCollection.h"

#include <string>

/** @class PapasExportParticlesTool Sim/SimPapas/src/PapasExportParticlesTool.h PapasExportParticlesTool.h
 *
 *  Takes papasparticles stored in the papas::Event and converts to MCParticles.
 *  @author A.J. Robson
 */
class PapasExportParticlesTool : public GaudiTool, virtual public IPapasTool {
public:
  /// Constructor.
  PapasExportParticlesTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  /// Initialize.
  virtual StatusCode initialize();
  /// create outputs
  virtual StatusCode createOutputStructures();
  /// Execute:
  virtual StatusCode run(papas::Event& pevent);
  /// Clear:
  virtual StatusCode clear() { return SUCCESS; };
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// handle for reconstructed particles collection
  DataHandle<fcc::ParticleCollection> m_particlesHandle{"recparticles", Gaudi::DataHandle::Writer, this};
  /// Particles type to be exported
  Gaudi::Property<std::string> m_particleSubtype{this, "particleSubtype", "", "particle subtype"};
  fcc::ParticleCollection* m_particles;  /// mcparticles collection
};

#endif  // SIM_PAPASPARTICLEEXPORTER_H
