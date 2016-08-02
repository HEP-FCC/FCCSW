#include "DelphesSaveMet.h"

// Delphes
#include "modules/Delphes.h"
#include "classes/DelphesClasses.h"
// datamodel
#include "datamodel/METCollection.h"
#include "datamodel/MCParticleCollection.h"

// ROOT
#include "TObjArray.h"

DECLARE_TOOL_FACTORY(DelphesSaveMet)

DelphesSaveMet::DelphesSaveMet(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);
  declareOutput("missingEt", m_mets);
  declareProperty("delphesMETArrayName", m_delphesMETArrayName);
  declareProperty("delphesSHTArrayName", m_delphesSHTArrayName);
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

DelphesSaveMet::~DelphesSaveMet() {}

StatusCode DelphesSaveMet::initialize() {
  return GaudiTool::initialize();
}

StatusCode DelphesSaveMet::finalize() {
  return GaudiTool::finalize();
}

StatusCode DelphesSaveMet::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) {
  // Create the collections
  auto colMET = m_mets.createAndPut();
  const TObjArray* delphesMETColl = delphes.ImportArray(m_delphesMETArrayName.c_str());
  const TObjArray* delphesSHTColl = delphes.ImportArray(m_delphesSHTArrayName.c_str());
  if (delphesSHTColl == nullptr || delphesMETColl == nullptr) {
    warning() << "Delphes collection " << m_delphesMETArrayName << " or " << m_delphesSHTArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }
  bool saveSHT = true;
  if (delphesMETColl->GetEntries()!=delphesSHTColl->GetEntries()) {

    saveSHT = false;
    warning() << "Can't save in a common FCC-EDM MET object both information from Delphes MET & scalarHT. Only MET will be saved!" << std::endl;
  }

  for(int j = 0; j < delphesMETColl->GetEntries(); ++j) {

    Candidate* candSHT = nullptr;

    auto candMET = static_cast<Candidate *>(delphesMETColl->At(j));
    if (saveSHT) candSHT = static_cast<Candidate *>(delphesSHTColl->At(j));

    auto met = colMET->create();

    met.Magnitude(candMET->Momentum.Pt());
    met.Phi((-(candMET->Momentum)).Phi());
    if (saveSHT) met.ScalarSum(candSHT->Momentum.Pt());
    else         met.ScalarSum(-1);

    // Debug: print FCC-EDM MET info
    if (msgLevel() <= MSG::DEBUG) {

      debug() << "MET Info: "
              << std::scientific
              << " MET: " << std::setprecision(2) << std::setw(9) << met.Magnitude()
              << " Phi: " << std::setprecision(2) << std::setw(9) << met.Phi()
              << " sHT: " << std::setprecision(2) << std::setw(9) << met.ScalarSum()
              << std::fixed
              << endmsg;
    } // Debug
  }
  return StatusCode::SUCCESS;
}
