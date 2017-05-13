#ifndef SIMDELPHESINTERFACE_DELPHESSAVECHARGEDPARTICLES
#define SIMDELPHESINTERFACE_DELPHESSAVECHARGEDPARTICLES

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimDelphesInterface/IDelphesSaveOutputTool.h"

// datamodel
namespace fcc {
class ParticleCollection;
class ParticleMCParticleAssociationCollection;
class TaggedParticleCollection;
}

/** @class DelphesSaveChargedParticles SimG4Components/src/DelphesSaveChargedParticles.h DelphesSaveChargedParticles.h
 *
 *  Save charged particles from Delphes simulation to FCC EDM
 *
 *  @author Z. Drasal, J. Lingemann
 */

class DelphesSaveChargedParticles : public GaudiTool, virtual public IDelphesSaveOutputTool {
public:
  explicit DelphesSaveChargedParticles(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~DelphesSaveChargedParticles();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save Delphes collection to EDM.
   *   Converts charged particles to fcc::Particle and creates associations to fcc::MCParticle
   *   If isolation tags are defined, they are also translated and associations are created.
   *   @param[in] delphes: reference to Delphes module
   *   @param[in] mcParticles: MCParticle collection that is used to create associations (FIXME: will be input at some
   * point)
   *   @return status code
   */
  virtual StatusCode saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) final;

private:
  /// Handle the particles to be saved
  DataHandle<fcc::ParticleCollection> m_particles{"particles", Gaudi::DataHandle::Writer, this};
  /// Handle to associate particles with MCParticles
  DataHandle<fcc::ParticleMCParticleAssociationCollection> m_mcAssociations{"mcAssociations", Gaudi::DataHandle::Writer,
                                                                            this};
  /// Handle for isolation tags
  DataHandle<fcc::TaggedParticleCollection> m_isolationTaggedParticles{"isolationTags", Gaudi::DataHandle::Writer,
                                                                       this};
  /// Name of the Delphes array that should be converted
  Gaudi::Property<std::string> m_delphesArrayName{this, "delphesArrayName", "",
                                                  "Name of the Delphes array that should be converted"};
  /// Switch whether to save tag information
  Gaudi::Property<bool> m_saveIso{this, "saveIsolation", true, "Switch whether to save tag information"};
};

#endif /* SIMDELPHESINTERFACE_DELPHESSAVECHARGEDPARTICLES */
