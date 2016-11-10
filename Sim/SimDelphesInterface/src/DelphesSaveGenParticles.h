#ifndef SIMDELPHESINTERFACE_DELPHESSAVEGENPARTICLES
#define SIMDELPHESINTERFACE_DELPHESSAVEGENPARTICLES

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimDelphesInterface/IDelphesSaveOutputTool.h"

// datamodel
namespace fcc {
class MCParticleCollection;
class GenVertexCollection;
}

/** @class DelphesSaveGenParticles Sim/SimDelphes/src/DelphesSaveGenParticles.h DelphesSaveGenParticles.h
 *
 *  Save MC particles from Delphes simulation to FCC EDM
 *
 *  @author M. Selvaggi
 */

class DelphesSaveGenParticles: public GaudiTool, virtual public IDelphesSaveOutputTool {
public:
  explicit DelphesSaveGenParticles(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~DelphesSaveGenParticles();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save Delphes collection to EDM.
   *   Converts Delphes MC particles to fcc::Particle and creates associations to fcc::MCParticle
   *   @return status code
   */
  virtual StatusCode saveOutput(Delphes& delphes) final;

private:
  /// Handle the particles to be saved
  DataHandle<fcc::MCParticleCollection> m_particles;
  /// Name of the Delphes array that should be converted
  std::string m_delphesArrayName;
};

#endif /* SIMDELPHESINTERFACE_DELPHESSAVEGENPARTICLES */
