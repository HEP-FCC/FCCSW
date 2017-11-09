#ifndef SIM_PAPASIMPORTPARTICLESTOOL_H
#define SIM_PAPASIMPORTPARTICLESTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
// FCCSW
#include "FWCore/DataHandle.h"
#include "SimPapas/IPapasImportTool.h"
// PAPAS
#include "papas/datatypes/Definitions.h"
#include "papas/datatypes/DefinitionsCollections.h"
#include <unordered_map>

namespace fcc {
class MCParticleCollection;
}

/** @class PapasImportParticlesTool Sim/SimPapas/src/PapasImportParticlesTool PapasImportParticlesTool
 *
 *  This creates a collection of papas simulated particles from Pythia generated particles.
 *  @author A.J. Robson
 */

class PapasImportParticlesTool : public GaudiTool, virtual public IPapasImportTool {
public:
  /// Constructor.
  PapasImportParticlesTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PapasImportParticlesTool();
  /// Initialise
  virtual StatusCode initialize();
  /// empty class structures.
  virtual StatusCode clear();
  /// Execute papas tool.
  virtual StatusCode run(papas::Event& papas, std::unordered_map<papas::Identifier, int>& links,
                         std::shared_ptr<papas::Detector> spDetector) final;
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the ParticleCollection to be read
  DataHandle<fcc::MCParticleCollection> m_genParticlesHandle{"MCparticles", Gaudi::DataHandle::Reader, this};
  /// Collection of papas particles that new particles will be added to
  papas::Particles m_particles;
};

#endif  // SIM_PAPASSIMULATORTOOL_H
