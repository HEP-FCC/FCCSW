#ifndef SIMDELPHESINTERFACE_DELPHESSAVEGENJETS
#define SIMDELPHESINTERFACE_DELPHESSAVEGENJETS

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimDelphesInterface/IDelphesSaveOutputTool.h"

// datamodel
namespace fcc {
class GenJetCollection;
class TaggedGenJetCollection;
}

class Candidate;

/** @class DelphesSaveGenJets SimG4Components/src/DelphesSaveGenJets.h DelphesSaveGenJets.h
 *
 *  Save charged particles from Delphes simulation to FCC EDM
 *
 *  @author Z. Drasal, J. Lingemann
 */

class DelphesSaveGenJets : public GaudiTool, virtual public IDelphesSaveOutputTool {
public:
  explicit DelphesSaveGenJets(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~DelphesSaveGenJets();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save Delphes collection to EDM.
   *   Converts genJets to fcc::GenJet and creates associations to fcc::MCParticle
   *   If flavor tags are defined, they are also translated and associations are created.
   *   @param[in] delphes: reference to Delphes module
   *   @param[in] mcParticles: MCParticle collection that is used to create associations (FIXME: will be input at some
   * point)
   *   @return status code
   */
  virtual StatusCode saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) final;

private:
  //! Recursive method to find an id of MCParticle related to the given Delphes Candidate object,
  //! if MC particle found (id>=0), its index is then saved to idRefMCPart set,
  //! if relation doesn't exist (id<0), warning is given on output and search for other relations continues
  void findJetPartMC(Candidate* cand, int rangeMCPart, std::set<int>& idRefMCPart);
  /// Handle to the jets to be saved
  DataHandle<fcc::GenJetCollection> m_genJets{"genJets", Gaudi::DataHandle::Writer, this};
  /// Handle to the jets including additional flavour tag information
  DataHandle<fcc::TaggedGenJetCollection> m_taggedGenJets{"taggedGenJets", Gaudi::DataHandle::Writer, this};
  /// Name of the Delphes array that should be converted
  Gaudi::Property<std::string> m_delphesArrayName{this, "delphesArrayName"
                                                        "",
                                                  "Name of the Delphes array that should be converted"};
};

#endif /* SIMDELPHESINTERFACE_DELPHESSAVEGENJETS */
