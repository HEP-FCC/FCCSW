#include "DelphesSimulation.h"

#include "SimDelphesInterface/ParticleStatus.h"
#include <limits>

// Delphes
#include "ExRootAnalysis/ExRootConfReader.h"
#include "ExRootAnalysis/ExRootTask.h"
#include "ExRootAnalysis/ExRootTreeBranch.h"
#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "HepMCDelphesConverter.h"
#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesModule.h"
#include "modules/Delphes.h"

// FCC EDM
#include "datamodel/FloatCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

// ROOT
#include "TFile.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TStopwatch.h"

DECLARE_COMPONENT(DelphesSimulation)

DelphesSimulation::DelphesSimulation(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hepmc", m_hepmcHandle, "");
  declareProperty("mcEventWeights", m_handleMCEventWeights);
  declareProperty("genParticles", m_handleGenParticles);
}

StatusCode DelphesSimulation::initialize() {
  gRandom->SetSeed(1234);
  // If required, export output directly to root file
  if (m_outRootFileName != "") {

    info() << "Opening ROOT output file: " << m_outRootFileName << endmsg;
    m_outRootFile = new TFile(m_outRootFileName.value().c_str(), "RECREATE");
    if (m_outRootFile->IsZombie()) {

      error() << "Can't open " << m_outRootFileName << endmsg;
      return Error("ERROR, can't open defined ROOT output file.");
    }
  }

  // Read Delphes configuration card (deleted by finalize())
  m_confReader = std::unique_ptr<ExRootConfReader>(new ExRootConfReader);
  m_confReader->ReadFile(m_DelphesCard.value().c_str());

  // Instance of Delphes (deleted by finalize())
  m_Delphes = std::unique_ptr<Delphes>(new Delphes("Delphes"));
  m_Delphes->SetConfReader(m_confReader.get());

  // Delphes needs data structure to be defined (ROOT tree) (deleted by finalize())
  m_treeWriter = new ExRootTreeWriter(m_outRootFile, "DelphesSim");
  m_branchEvent = m_treeWriter->NewBranch("Event", HepMCEvent::Class());
  m_Delphes->SetTreeWriter(m_treeWriter);

  // Define event readers
  //
  //  HepMC reader --> reads either from a file or directly from data store (deleted by finalize())
  m_hepMCConverter = std::unique_ptr<HepMCDelphesConverter>(new HepMCDelphesConverter);

  // Create following arrays of Delphes objects --> starting objects
  m_allParticles = m_Delphes->ExportArray("allParticles");
  m_stableParticles = m_Delphes->ExportArray("stableParticles");
  m_partons = m_Delphes->ExportArray("partons");

  // Init Delphes - read in configuration & define modules to be executed
  m_Delphes->InitTask();

  // Print Delphes modules to be used
  ExRootConfParam param = m_confReader->GetParam("::ExecutionPath");
  Long_t size = param.GetSize();
  info() << "Delphes simulation will use the following modules: " << endmsg;
  for (Long_t k = 0; k < size; ++k) {

    TString name = param[k].GetString();
    info() << "-- Module: " << name << endmsg;
  }

  m_eventCounter = 0;

  m_treeWriter->Clear();
  m_Delphes->Clear();

  for (auto& toolname : m_saveToolNames) {
    m_saveTools.push_back(tool<IDelphesSaveOutputTool>(toolname));
    // FIXME: check StatusCode once the m_saveTools is a ToolHandleArray
    // if (!) {
    //   error() << "Unable to retrieve the output saving tool." << endmsg;
    //   return StatusCode::FAILURE;
    // }
  }

  return StatusCode::SUCCESS;
}

StatusCode DelphesSimulation::execute() {

  //
  // Read event & initialize event variables
  TStopwatch readStopWatch;
  readStopWatch.Start();

  StatusCode sc;

  // Read event
  const HepMC::GenEvent* hepMCEvent = m_hepmcHandle.get();

  sc = m_hepMCConverter->hepMCEventToArrays(
      hepMCEvent, *m_Delphes->GetFactory(), *m_allParticles, *m_stableParticles, *m_partons);
  if (!sc.isSuccess()) {
    return sc;
  }
  // Print debug: HepMC event info
  if (msgLevel() <= MSG::DEBUG) {

    for (auto ipart = hepMCEvent->particles_begin(); ipart != hepMCEvent->particles_end(); ++ipart) {

      int motherID = 0;
      int motherIDRange = 0;
      int daughterID = 0;
      int daughterIDRange = 0;
      if ((*ipart)->production_vertex() != nullptr) {

        motherID = (*((*ipart)->production_vertex()->particles_in_const_begin()))->barcode();
        motherIDRange = (*ipart)->production_vertex()->particles_in_size() - 1;
      }
      if ((*ipart)->end_vertex() != nullptr) {

        daughterID = (*((*ipart)->end_vertex()->particles_out_const_begin()))->barcode();
        daughterIDRange = (*ipart)->end_vertex()->particles_out_size() - 1;
      }

      debug() << "HepMC: "
              << " Id: " << std::setw(3) << (*ipart)->barcode() << " Pdg: " << std::setw(5) << (*ipart)->pdg_id()
              << " Mothers: " << std::setw(4) << motherID << " -> " << std::setw(4) << motherID + motherIDRange
              << " Daughters: " << std::setw(4) << daughterID << " -> " << std::setw(4) << daughterID + daughterIDRange
              << " Stat: " << std::setw(2) << (*ipart)->status() << std::scientific << " Px: " << std::setprecision(2)
              << std::setw(9) << (*ipart)->momentum().px() << " Py: " << std::setprecision(2) << std::setw(9)
              << (*ipart)->momentum().py() << " Pz: " << std::setprecision(2) << std::setw(9)
              << (*ipart)->momentum().pz() << " E: " << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().e()
              << " M: " << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().m();
      if ((*ipart)->production_vertex() != nullptr) {
        debug() << " Vx: " << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().x()
                << " Vy: " << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().y()
                << " Vz: " << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().z()
                << " T: " << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().t();
      }
      debug() << std::fixed << endmsg;
    }
  }  // Debug

  // Print debug: Delphes event info
  if (msgLevel() <= MSG::DEBUG) {

    for (auto i = 0; i < m_allParticles->GetEntries(); i++) {

      Candidate* candidate = static_cast<Candidate*>(m_allParticles->At(i));

      debug() << "DelphesMC: "
              << " Id: " << std::setw(3) << i + 1 << " Pdg: " << std::setw(5) << candidate->PID
              << " Mothers: " << std::setw(4) << candidate->M1 + 1 << " -> " << std::setw(4) << candidate->M2 + 1
              << " Daughters: " << std::setw(4) << candidate->D1 + 1 << " -> " << std::setw(4) << candidate->D2 + 1
              << " Stat: " << std::setw(2) << candidate->Status << std::scientific << " Px: " << std::setprecision(2)
              << std::setw(9) << candidate->Momentum.Px() << " Py: " << std::setprecision(2) << std::setw(9)
              << candidate->Momentum.Py() << " Pz: " << std::setprecision(2) << std::setw(9) << candidate->Momentum.Pz()
              << " E: " << std::setprecision(2) << std::setw(9) << candidate->Momentum.E()
              << " M: " << std::setprecision(2) << std::setw(9) << candidate->Mass << " Vx: " << std::setprecision(2)
              << std::setw(9) << candidate->Position.X() << " Vy: " << std::setprecision(2) << std::setw(9)
              << candidate->Position.Y() << " Vz: " << std::setprecision(2) << std::setw(9) << candidate->Position.Z()
              << " T: " << std::setprecision(2) << std::setw(9) << candidate->Position.T() << std::fixed << endmsg;
    }
  }  // Debug

  m_eventCounter++;
  readStopWatch.Stop();

  //
  // Process event
  TStopwatch procStopWatch;

  // Delphes process
  procStopWatch.Start();
  m_Delphes->ProcessTask();
  procStopWatch.Stop();

  // Generate Delphes branch: Event
  // m_hepMCConverter->MakeEventBranch(m_branchEvent, &readStopWatch, &procStopWatch);
  if (m_outRootFile != nullptr) m_treeWriter->Fill();

  // FCC EDM (event-data model) based output
  auto mcEventWeights = m_handleMCEventWeights.createAndPut();

  // loop over HepMC event weights
  for (unsigned int j = 0; j < hepMCEvent->weights().size(); j++) {
    // FIXME: weights are stored as collection. Eventually move to meta-data
    auto weight = mcEventWeights->create();
    weight.value(hepMCEvent->weights()[j]);
  }

  // FCC EDM (event-data model) based output
  auto genParticles = m_handleGenParticles.createAndPut();

  // convert Delphes MC particles to FCC EDM
  if (m_allParticles != nullptr) DelphesSimulation::ConvertMCParticles(m_allParticles, genParticles);

  for (auto saveTool : m_saveTools) {
    saveTool->saveOutput(*m_Delphes, *genParticles);
  }

  // Initialize for next event reading (Will also zero Delphes arrays)
  m_treeWriter->Clear();
  m_Delphes->Clear();

  return StatusCode::SUCCESS;
}

StatusCode DelphesSimulation::finalize() {

  // Finish Delphes task
  m_Delphes->FinishTask();

  // Write output to Root file
  if (m_outRootFile != nullptr) {

    m_treeWriter->Write();
    m_outRootFile->Close();

    if (m_outRootFile != nullptr) {
      delete m_outRootFile;
      m_outRootFile = nullptr;
    }
  }

  info() << "Exiting Delphes..." << endmsg;

  // Clear memory
  if (m_hepMCConverter.get() != nullptr) m_hepMCConverter.reset();  // Releases also the memory allocated by inHepMCFile
  if (m_Delphes.get() != nullptr) m_Delphes.reset();                // Releases also the memory allocated by treeWriter
  if (m_confReader.get() != nullptr) m_confReader.reset();

  return GaudiAlgorithm::finalize();
}

//
// Convert internal Delphes objects: MCParticles to FCC EDM: MCParticle
//
void DelphesSimulation::ConvertMCParticles(const TObjArray* Input, fcc::MCParticleCollection* colMCParticles) {

  // Save MC particles and vertices
  for (int j = 0; j < Input->GetEntries(); j++) {

    auto cand = static_cast<Candidate*>(Input->At(j));

    ///////////////////////////////////////
    // filter only interesting particles //
    ///////////////////////////////////////

    bool pass = false;

    auto status = cand->Status;
    auto pdgCode = TMath::Abs(cand->PID);
    auto pt = cand->Momentum.Pt();

    // hard scattering particles (first condition for Py6, second for Py8)
    if (status == 3) pass = true;
    if (status > 20 && status < 30) pass = true;

    // electrons, muons, taus and neutrinos
    if (pdgCode > 10 && pdgCode < 17) pass = true;

    // heavy quarks
    if (pdgCode == 4 || pdgCode == 5 || pdgCode == 6) pass = true;

    // Gauge bosons and other fundamental bosons
    if (pdgCode > 22 && pdgCode < 43) pass = true;

    // retain only stable particles with high enough transverse momentum
    if (status == 1 && pt > 5.0) pass = true;

    if (!pass && m_applyGenFilter) continue;

    //////////////////////////////
    // store filtered particles //
    //////////////////////////////

    auto particle = colMCParticles->create();

    auto& barePart = particle.core();
    barePart.pdgId = cand->PID;
    barePart.status = cand->Status;
    barePart.p4.px = cand->Momentum.Px();
    barePart.p4.py = cand->Momentum.Py();
    barePart.p4.pz = cand->Momentum.Pz();
    barePart.p4.mass = cand->Momentum.M();
    barePart.charge = cand->Charge;
    barePart.vertex.x = cand->Position.X();
    barePart.vertex.y = cand->Position.Y();
    barePart.vertex.z = cand->Position.Z();

    // need to store Delphes UniqueID in order to store MC association
    barePart.bits = cand->GetUniqueID();

    // Debug: print FCC-EDM MCParticle and GenVertex
    if (msgLevel() <= MSG::DEBUG) {

      double partE = sqrt(particle.p4().px * particle.p4().px + particle.p4().py * particle.p4().py +
                          particle.p4().pz * particle.p4().pz + particle.p4().mass * particle.p4().mass);

      debug() << "MCParticle: "
              << " Id: " << std::setw(3) << j + 1 << " Pdg: " << std::setw(5) << particle.pdgId()
              << " Stat: " << std::setw(2) << particle.status() << " Bits: " << std::setw(2) << particle.bits()
              << std::scientific << " Px: " << std::setprecision(2) << std::setw(9) << particle.p4().px
              << " Py: " << std::setprecision(2) << std::setw(9) << particle.p4().py << " Pz: " << std::setprecision(2)
              << std::setw(9) << particle.p4().pz << " E: " << std::setprecision(2) << std::setw(9) << partE
              << " M: " << std::setprecision(2) << std::setw(9) << particle.p4().mass;
      debug() << std::fixed << endmsg;

    }  // Debug
  }
}
