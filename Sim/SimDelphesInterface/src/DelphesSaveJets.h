#ifndef SIMDELPHESINTERFACE_DELPHESSAVEJETS
#define SIMDELPHESINTERFACE_DELPHESSAVEJETS

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimDelphesInterface/IDelphesSaveOutputTool.h"

// datamodel
namespace fcc {
class JetCollection;
class ParticleCollection;
class JetParticleAssociationCollection;
class JetIntTagAssociationCollection;
class TagCollection;
class IntTagCollection;
class JetTagAssociationCollection;
}

/** @class DelphesSaveJets SimG4Components/src/DelphesSaveJets.h DelphesSaveJets.h
 *
 *  Save charged particles from Delphes simulation to FCC EDM
 *
 *  @author Z. Drasal, J. Lingemann
 */

class DelphesSaveJets: public GaudiTool, virtual public IDelphesSaveOutputTool {
public:
  explicit DelphesSaveJets(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~DelphesSaveJets();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save Delphes collection to EDM.
   *   Converts genJets to fcc::Jet and creates associations to fcc::MCParticle
   *   If flavor tags are defined, they are also translated and associations are created.
   *   @param[in] delphes: reference to Delphes module
   *   @param[in] mcParticles: MCParticle collection that is used to create associations (FIXME: will be input at some point)
   *   @return status code
   */
  virtual StatusCode saveOutput(Delphes& delphes,
                                const fcc::MCParticleCollection& mcParticles) final;
private:
  /// Handle to the jets to be saved
  DataHandle<fcc::JetCollection> m_jets;
  /// Handle to the jet constituents to be saved
  DataHandle<fcc::ParticleCollection> m_jetParticles;
  /// Handle for associations between the jet and its constituents to be saved
  DataHandle<fcc::JetParticleAssociationCollection> m_jetParticleAssociations;
  /// Handle to the jet flavor tags to be saved
  DataHandle<fcc::IntTagCollection> m_jetFlavorTags;
  /// Handle for associations between the jet and its flavor tag to be saved
  DataHandle<fcc::JetIntTagAssociationCollection> m_jetFlavorAssociations;
  /// Handle to the b tags to be saved
  DataHandle<fcc::TagCollection> m_bTags;
  /// Handle for associations between the jet and its b-tag to be saved
  DataHandle<fcc::JetTagAssociationCollection> m_jetBTagAssociations;
  /// Handle to the c tags to be saved
  DataHandle<fcc::TagCollection> m_cTags;
  /// Handle for associations between the jet and its c-tag to be saved
  DataHandle<fcc::JetTagAssociationCollection> m_jetCTagAssociations;
  /// Handle to the tau tags to be saved
  DataHandle<fcc::TagCollection> m_tauTags;
  /// Handle for associations between the jet and its tau-tag to be saved
  DataHandle<fcc::JetTagAssociationCollection> m_jetTauTagAssociations;
  /// Name of the Delphes array that should be converted
  std::string m_delphesArrayName;

};

#endif /* SIMDELPHESINTERFACE_DELPHESSAVEJETS */
