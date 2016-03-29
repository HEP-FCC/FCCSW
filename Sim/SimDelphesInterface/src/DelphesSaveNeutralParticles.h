#ifndef SIMDELPHESINTERFACE_DELPHESSAVENEUTRALPARTICLES
#define SIMDELPHESINTERFACE_DELPHESSAVENEUTRALPARTICLES

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimDelphesInterface/IDelphesSaveOutputTool.h"

// datamodel
namespace fcc {
class ParticleCollection;
class ParticleMCParticleAssociationCollection;
class TagCollection;
class ParticleTagAssociationCollection;
}

/** @class DelphesSaveNeutralParticles SimG4Components/src/DelphesSaveNeutralParticles.h DelphesSaveNeutralParticles.h
 *
 *  Save charged particles from Delphes simulation to FCC EDM
 *
 *  @author Z. Drasal, J. Lingemann
 */

class DelphesSaveNeutralParticles: public GaudiTool, virtual public IDelphesSaveOutputTool {
public:
  explicit DelphesSaveNeutralParticles(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~DelphesSaveNeutralParticles();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save Delphes collection to EDM.
   *   Converts neutral particles to fcc::Particle and creates associations to fcc::MCParticle
   *   If isolation switch is true, isolation tags are also translated and associations are created.
   *   @param[in] delphes: reference to Delphes module
   *   @param[in] mcParticles: MCParticle collection that is used to create associations (FIXME: will be input at some point)
   *   @return status code
   */
  virtual StatusCode saveOutput(Delphes& delphes,
                                const fcc::MCParticleCollection& mcParticles) final;
private:
  /// Handle the particles to be saved
  DataHandle<fcc::ParticleCollection> m_particles;
  /// Handle to associate particles with MCParticles
  DataHandle<fcc::ParticleMCParticleAssociationCollection> m_mcAssociations;
  /// Handle for isolation tags
  DataHandle<fcc::TagCollection> m_isolationTags;
  /// Handle to associate particles with isolation tags
  DataHandle<fcc::ParticleTagAssociationCollection> m_isoAssociations;
  /// Name of the Delphes array that should be converted
  std::string m_delphesArrayName;
  /// Switch whether to save tag information
  bool m_saveIso;

};

#endif /* SIMDELPHESINTERFACE_DELPHESSAVENEUTRALPARTICLES */
