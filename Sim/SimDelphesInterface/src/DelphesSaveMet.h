#ifndef SIMDELPHESINTERFACE_DELPHESSAVEMET
#define SIMDELPHESINTERFACE_DELPHESSAVEMET

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimDelphesInterface/IDelphesSaveOutputTool.h"

// datamodel
namespace fcc {
class METCollection;
}

/** @class DelphesSaveMet SimG4Components/src/DelphesSaveMet.h DelphesSaveMet.h
 *
 *  Save missing transverse energy from Delphes simulation to FCC EDM
 *
 *  @author Z. Drasal, J. Lingemann
 */

class DelphesSaveMet : public GaudiTool, virtual public IDelphesSaveOutputTool {
public:
  explicit DelphesSaveMet(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~DelphesSaveMet();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save Delphes collection to EDM.
   *   Converts met to fcc::MET and creates associations to fcc::MCParticle
   *   @param[in] delphes: reference to Delphes module
   *   @param[in] mcParticles: Not used here will vanish in the future: (FIXME: will be input at some point)
   *   @return status code
   */
  virtual StatusCode saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) final;

private:
  /// Handle the METs to be saved
  DataHandle<fcc::METCollection> m_mets{"missingEt", Gaudi::DataHandle::Writer, this};
  /// Name of the Delphes MET array that should be converted
  Gaudi::Property<std::string> m_delphesMETArrayName{this, "delphesMETArrayName", "",
                                                     "Name of the Delphes MET array that should be converted"};
  /// Name of the Delphes SHT array that should be converted
  Gaudi::Property<std::string> m_delphesSHTArrayName{this, "delphesSHTArrayName", "",
                                                     "Name of the Delphes SHT array that should be converted"};
};

#endif /* SIMDELPHESINTERFACE_DELPHESSAVEMET */
