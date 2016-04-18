#ifndef SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H
#define SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H

// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/IG4SaveOutputTool.h"

// datamodel
namespace fcc {
class ParticleCollection;
class ParticleMCParticleAssociationCollection;
}

/** @class G4SaveSmearedParticles SimG4Components/src/G4SaveSmearedParticles.h G4SaveSmearedParticles.h
 *
 *  Save 'reconstructed' (smeared) particles.
 *
 *  @author Anna Zaborowska
 */

class G4SaveSmearedParticles: public AlgTool, virtual public IG4SaveOutputTool {
public:
  explicit G4SaveSmearedParticles(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4SaveSmearedParticles();
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
  DataHandle<fcc::ParticleCollection> m_particles;
  /// Handle for the associations between particles and MC particles to be written
  DataHandle<fcc::ParticleMCParticleAssociationCollection> m_particlesMCparticles;

};

#endif /* SIMG4COMPONENTS_G4SAVESMEAREDPARTICLES_H */
