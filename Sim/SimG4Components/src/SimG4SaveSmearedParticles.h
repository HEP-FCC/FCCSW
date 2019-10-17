#ifndef SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H
#define SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"

// datamodel
namespace fcc {
class ParticleCollection;
class ParticleMCParticleAssociationCollection;
}

/** @class SimG4SaveSmearedParticles SimG4Components/src/SimG4SaveSmearedParticles.h SimG4SaveSmearedParticles.h
 *
 *  Save 'reconstructed' (smeared) particles.
 *
 *  @author Anna Zaborowska
 */

class SimG4SaveSmearedParticles : public GaudiTool, virtual public ISimG4SaveOutputTool {
public:
  explicit SimG4SaveSmearedParticles(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4SaveSmearedParticles();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save the data output.
   *   Saves the 'reconstructed' (smeared) particles and associates them with MC particles.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) final;

private:
  /// Handle for the particles to be written
  DataHandle<fcc::ParticleCollection> m_particles{"smearedParticles", Gaudi::DataHandle::Writer, this};
  /// Handle for the associations between particles and MC particles to be written
  DataHandle<fcc::ParticleMCParticleAssociationCollection> m_particlesMCparticles{"smearedParticlesToParticles",
                                                                                  Gaudi::DataHandle::Writer, this};
};

#endif /* SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H */
