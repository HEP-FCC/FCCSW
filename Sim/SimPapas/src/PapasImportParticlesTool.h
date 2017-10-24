#ifndef SIM_PAPASIMPORTPARTICLESTOOL_H
#define SIM_PAPASIMPORTPARTICLESTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
// FCCSW
#include "FWCore/DataHandle.h"
#include "SimPapas/IPapasTool.h"
// papas
#include "papas/datatypes/DefinitionsCollections.h"

namespace fcc {
class MCParticleCollection;
}

/** @class PapasImportParticlesTool Sim/SimPapas/src/PapasImportParticlesTool PapasImportParticlesTool
 *
 *  This creates a collection of papas simulated particles from Pythia generated particles.
 *  @author A.J. Robson
 */

class PapasImportParticlesTool : public GaudiTool, virtual public IPapasTool {
public:
  /// Constructor.
  PapasImportParticlesTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PapasImportParticlesTool();
  /// Structures
  StatusCode createOutputStructures() { return SUCCESS; };
  /// Create any output structures needed.
  virtual StatusCode initialize();
  /// empty class structures.
  virtual StatusCode clear();
  /// Execute papas tool.
  virtual StatusCode run(papas::Event& papas) final;
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the ParticleCollection to be read
  DataHandle<fcc::MCParticleCollection> m_iGenpHandle{"MCparticles", Gaudi::DataHandle::Reader, this};
  /// Collection of papas particles that new particles will be added to
  papas::Particles m_particles;
};

#endif  // SIM_PAPASSIMULATORTOOL_H
