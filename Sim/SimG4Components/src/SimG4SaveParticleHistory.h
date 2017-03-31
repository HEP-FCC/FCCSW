#ifndef SIMG4COMPONENTS_SIMG4SAVEPARTICLEHISTORY_H
#define SIMG4COMPONENTS_SIMG4SAVEPARTICLEHISTORY_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"
class IGeoSvc;

// datamodel
namespace fcc {
class GenVertexCollection;
class MCParticleCollection;
}

/** @class SimG4SaveParticleHistory SimG4Components/src/SimG4SaveParticleHistory.h SimG4SaveParticleHistory.h
 *
 *  This tool allows to save the particle history of particles decaying during the simulation
 *
 *  @author J. Lingemann
 */

class SimG4SaveParticleHistory : public GaudiTool, virtual public ISimG4SaveOutputTool {
public:
  explicit SimG4SaveParticleHistory(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4SaveParticleHistory() = default;

  /**  Save the history
   *   Creates particles and gen vertices that allow association of parents and children
   *   @param[in] aEvent The Geant Event conatining data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) final;

private:
  /// Handle for collection of MC particles to create
  DataHandle<fcc::MCParticleCollection> m_mcParticles;
  /// Handle for the vertex collection to create
  DataHandle<fcc::GenVertexCollection> m_genVertices;
};

#endif /* SIMG4COMPONENTS_SIMG4SAVEPARTICLEHISTORY_H */
