#include "DelphesSimulation.h"

#include <limits>
#include "SimDelphesInterface/ParticleStatus.h"

// Delphes
#include "modules/Delphes.h"
#include "classes/DelphesModule.h"
#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "HepMCDelphesConverter.h"
#include "ExRootAnalysis/ExRootConfReader.h"
#include "ExRootAnalysis/ExRootTask.h"
#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "ExRootAnalysis/ExRootTreeBranch.h"

// FCC EDM
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"

// ROOT
#include "TFile.h"
#include "TObjArray.h"
#include "TStopwatch.h"
#include "TRandom.h"

DECLARE_COMPONENT(DelphesSimulation)

DelphesSimulation::DelphesSimulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) ,
  m_DelphesCard(),
  m_Delphes(nullptr),
  m_hepMCConverter(nullptr),
  m_eventCounter(0),
  m_outRootFile(nullptr),
  m_outRootFileName(""),
  m_treeWriter(nullptr),
  m_branchEvent(nullptr),
  m_confReader(nullptr),
  m_allParticles(nullptr),
  m_stableParticles(nullptr),
  m_partons(nullptr)
{
  //declareProperty("filename", m_filename="" , "Name of the HepMC file to read");
  declareProperty("DelphesCard"      , m_DelphesCard              , "Name of Delphes tcl config file with detector and simulation parameters");
  declareProperty("ROOTOutputFile"   , m_outRootFileName          , "Name of Delphes Root output file, if defined, the Delphes standard tree write out (in addition to FCC-EDM based output to transient data store)");

  declareInput("hepmc", m_hepmcHandle);
  declareProperty("outputs", m_saveToolNames);

  declareOutput("genParticles"      , m_handleGenParticles, "genParticles");
  declareOutput("genVertices"       , m_handleGenVertices, "genVertices");
}

StatusCode DelphesSimulation::initialize() {
  gRandom->SetSeed(1234);
  // If required, export output directly to root file
  if (m_outRootFileName!="") {

    info()  << "Opening ROOT output file: " << m_outRootFileName << endmsg;
    m_outRootFile = new TFile(m_outRootFileName.c_str(), "RECREATE");
    if (m_outRootFile->IsZombie()) {

      error() << "Can't open " << m_outRootFileName << endmsg;
      return Error ("ERROR, can't open defined ROOT output file.");
    }
  }

  // Read Delphes configuration card (deleted by finalize())
  m_confReader = std::unique_ptr<ExRootConfReader>(new ExRootConfReader);
  m_confReader->ReadFile(m_DelphesCard.c_str());

  // Instance of Delphes (deleted by finalize())
  m_Delphes = std::unique_ptr<Delphes>(new Delphes("Delphes"));
  m_Delphes->SetConfReader(m_confReader.get());

  // Delphes needs data structure to be defined (ROOT tree) (deleted by finalize())
  m_treeWriter  = new ExRootTreeWriter( m_outRootFile, "DelphesSim");
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
  Long_t          size  = param.GetSize();
  info()  << "Delphes simulation will use the following modules: " << endmsg;
  for( Long_t k = 0; k < size; ++k) {

    TString name = param[k].GetString();
    info()  << "-- Module: " <<  name << endmsg;
  }

  m_eventCounter     = 0;

  m_treeWriter->Clear();
  m_Delphes->Clear();

  for(auto& toolname: m_saveToolNames) {
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
  const HepMC::GenEvent *hepMCEvent = m_hepmcHandle.get();
  sc = m_hepMCConverter->hepMCEventToArrays(hepMCEvent, *m_Delphes->GetFactory(), *m_allParticles, *m_stableParticles, *m_partons);
  if (!sc.isSuccess()) {
    return sc;
  }
  // Print debug: HepMC event info
  if (msgLevel() <= MSG::DEBUG) {

    for (auto ipart=hepMCEvent->particles_begin(); ipart!=hepMCEvent->particles_end(); ++ipart) {

      int motherID        = 0;
      int motherIDRange   = 0;
      int daughterID      = 0;
      int daughterIDRange = 0;
      if ((*ipart)->production_vertex()!=nullptr) {

        motherID      = (*((*ipart)->production_vertex()->particles_in_const_begin()))->barcode();
        motherIDRange = (*ipart)->production_vertex()->particles_in_size() -1;
      }
      if ((*ipart)->end_vertex()!=nullptr) {

        daughterID      = (*((*ipart)->end_vertex()->particles_out_const_begin()))->barcode();
        daughterIDRange = (*ipart)->end_vertex()->particles_out_size() -1;
      }

      debug() << "HepMC: "
              << " Id: "       << std::setw(3)  << (*ipart)->barcode()
              << " Pdg: "      << std::setw(5)  << (*ipart)->pdg_id()
              << " Mothers: "  << std::setw(4)  << motherID   << " -> " << std::setw(4) << motherID  +motherIDRange
              << " Daughters: "<< std::setw(4)  << daughterID << " -> " << std::setw(4) << daughterID+daughterIDRange
              << " Stat: "     << std::setw(2)  << (*ipart)->status()
              << std::scientific
              << " Px: "       << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().px()
              << " Py: "       << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().py()
              << " Pz: "       << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().pz()
              << " E: "        << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().e()
              << " M: "        << std::setprecision(2) << std::setw(9) << (*ipart)->momentum().m();
      if ((*ipart)->production_vertex()!=nullptr) {
        debug() << " Vx: "       << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().x()
                << " Vy: "       << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().y()
                << " Vz: "       << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().z()
                << " T: "        << std::setprecision(2) << std::setw(9) << (*ipart)->production_vertex()->position().t();
      }
      debug() << std::fixed << endmsg;
    }
  } // Debug

  // Print debug: Delphes event info
  if (msgLevel() <= MSG::DEBUG) {

    for (auto i=0; i<m_allParticles->GetEntries(); i++) {

      Candidate *candidate = static_cast<Candidate *>(m_allParticles->At(i));

      debug() << "DelphesMC: "
              << " Id: "       << std::setw(3)  << i+1
              << " Pdg: "      << std::setw(5)  << candidate->PID
              << " Mothers: "  << std::setw(4)  << candidate->M1+1 << " -> " << std::setw(4) << candidate->M2+1
              << " Daughters: "<< std::setw(4)  << candidate->D1+1 << " -> " << std::setw(4) << candidate->D2+1
              << " Stat: "     << std::setw(2)  << candidate->Status
              << std::scientific
              << " Px: "       << std::setprecision(2) << std::setw(9) << candidate->Momentum.Px()
              << " Py: "       << std::setprecision(2) << std::setw(9) << candidate->Momentum.Py()
              << " Pz: "       << std::setprecision(2) << std::setw(9) << candidate->Momentum.Pz()
              << " E: "        << std::setprecision(2) << std::setw(9) << candidate->Momentum.E()
              << " M: "        << std::setprecision(2) << std::setw(9) << candidate->Mass
              << " Vx: "       << std::setprecision(2) << std::setw(9) << candidate->Position.X()
              << " Vy: "       << std::setprecision(2) << std::setw(9) << candidate->Position.Y()
              << " Vz: "       << std::setprecision(2) << std::setw(9) << candidate->Position.Z()
              << " T: "        << std::setprecision(2) << std::setw(9) << candidate->Position.T()
              << std::fixed
              << endmsg;
    }
  } // Debug

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
  if (m_outRootFile!=nullptr) m_treeWriter->Fill();

  // FCC EDM (event-data model) based output
  auto genParticles       = m_handleGenParticles.createAndPut();
  auto genVertices        = m_handleGenVertices.createAndPut();

  if (m_allParticles !=nullptr) DelphesSimulation::ConvertMCParticles(m_allParticles , genParticles  , genVertices);

  for(auto saveTool : m_saveTools) {
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
  if (m_outRootFile!=nullptr) {

    m_treeWriter->Write();
    m_outRootFile->Close();

    if (m_outRootFile!=nullptr) {delete m_outRootFile; m_outRootFile=nullptr;}
  }

  info() << "Exiting Delphes..." << endmsg;

  // Clear memory
  if (m_hepMCConverter.get()!=nullptr) m_hepMCConverter.reset(); // Releases also the memory allocated by inHepMCFile
  if (m_Delphes.get()    !=nullptr) m_Delphes.reset();     // Releases also the memory allocated by treeWriter
  if (m_confReader.get() !=nullptr) m_confReader.reset();

  return GaudiAlgorithm::finalize();
}

//
// Convert internal Delphes objects: MCParticles to FCC EDM: MCParticle & GenVertices
//
void DelphesSimulation::ConvertMCParticles(const TObjArray* Input,
                                           fcc::MCParticleCollection* colMCParticles,
                                           fcc::GenVertexCollection* colGenVertices) {

  //Initialize MC particle vertex mapping: production & decay vertex
  std::vector<std::pair<int, int>> vecPartProdVtxIDDecVtxID;

  vecPartProdVtxIDDecVtxID.resize(Input->GetEntries());
  for(int j=0; j<Input->GetEntries(); j++) {
    vecPartProdVtxIDDecVtxID[j].first  = -1;
    vecPartProdVtxIDDecVtxID[j].second = -1;
  }

  // Find true daughters of the colliding particles (necessary fix for missing links
  // between primary colliding particles and their daughters if LHE file used within Pythia)
  std::set<int> primary1Daughters;
  std::set<int> primary2Daughters;

  for(int j=0; j<Input->GetEntries(); j++) {

    auto cand = static_cast<Candidate *>(m_allParticles->At(j));

    // Go through all not primary particles
    if (cand->M1!=-1) {
      for (int iMother=cand->M1; iMother<=cand->M2; iMother++) {

        if (iMother==0) primary1Daughters.insert(j);
        if (iMother==1) primary2Daughters.insert(j);
      }
    }
  } // Fix

  // Save MC particles and vertices
  for(int j=0; j<Input->GetEntries(); j++) {

    auto cand     = static_cast<Candidate *>(m_allParticles->At(j));
    auto particle = colMCParticles->create();

    auto& barePart    = particle.core();
    barePart.pdgId     = cand->PID;
    barePart.status   = cand->Status;
    barePart.p4.px    = cand->Momentum.Px();
    barePart.p4.py    = cand->Momentum.Py();
    barePart.p4.pz    = cand->Momentum.Pz();
    barePart.p4.mass  = cand->Momentum.M();
    barePart.charge   = cand->Charge;
    barePart.vertex.x = cand->Position.X();
    barePart.vertex.y = cand->Position.Y();
    barePart.vertex.z = cand->Position.Z();

    if (cand->M1==-1)      barePart.bits = static_cast<unsigned>(ParticleStatus::kBeam);
    else if (cand->D1==-1) barePart.bits = static_cast<unsigned>(ParticleStatus::kStable);
    else                   barePart.bits = static_cast<unsigned>(ParticleStatus::kDecayed);

    // Mapping the vertices
    int& idPartStartVertex = vecPartProdVtxIDDecVtxID[j].first;
    int& idPartEndVertex   = vecPartProdVtxIDDecVtxID[j].second;

    // Production vertex
    if (cand->M1!=-1) {
      if (idPartStartVertex!=-1) {
        particle.startVertex(colMCParticles->at(idPartStartVertex).endVertex());
      }
      else {
        fcc::Point point;
        point.x = cand->Position.X();
        point.y = cand->Position.Y();
        point.z = cand->Position.Z();

        auto vertex = colGenVertices->create();
        vertex.position(point);
        vertex.ctau(cand->Position.T());
        particle.startVertex(vertex);

        idPartStartVertex = j;
      }
      for (int iMother=cand->M1; iMother<=cand->M2; iMother++) {
        if (vecPartProdVtxIDDecVtxID[iMother].second==-1) vecPartProdVtxIDDecVtxID[iMother].second = j;
      }
    }
    // Decay vertex
    if (cand->D1!=-1) {
      Candidate* daughter  = static_cast<Candidate *>(Input->At(cand->D1));

      if (idPartEndVertex!=-1) {
        particle.endVertex(colMCParticles->at(idPartEndVertex).startVertex());
      }
      else {
        fcc::Point point;
        point.x  = daughter->Position.X();
        point.y  = daughter->Position.Y();
        point.z  = daughter->Position.Z();

        auto vertex = colGenVertices->create();
        vertex.position(point);
        vertex.ctau(cand->Position.T());
        particle.endVertex(vertex);

        idPartEndVertex = cand->D1;
      }

      // Option for colliding particles -> broken daughters range -> use only D1, which is correctly set (D2 & D2-D1 is wrong!!!)
      if (cand->M1==-1) {

        // Primary particle 0 correction
        if (j==0) for (const int& iDaughter : primary1Daughters) {

          if (iDaughter>=0 && vecPartProdVtxIDDecVtxID[iDaughter].first==-1) vecPartProdVtxIDDecVtxID[iDaughter].first = j;
        }
        // Primary particle 1 correction
        else if (j==1) for (const int& iDaughter : primary2Daughters) {

          if (iDaughter>=0 && vecPartProdVtxIDDecVtxID[iDaughter].first==-1) vecPartProdVtxIDDecVtxID[iDaughter].first = j;
        }
      }
      // Option for all other particles
      else {
        for (int iDaughter=cand->D1; iDaughter<=cand->D2; iDaughter++) {

          if (iDaughter>=0 && vecPartProdVtxIDDecVtxID[iDaughter].first==-1) vecPartProdVtxIDDecVtxID[iDaughter].first = j;
        }
      }
    }

    // Debug: print FCC-EDM MCParticle and GenVertex
    if (msgLevel() <= MSG::DEBUG) {

      double partE = sqrt(particle.p4().px*particle.p4().px +
                          particle.p4().py*particle.p4().py +
                          particle.p4().pz*particle.p4().pz +
                          particle.p4().mass*particle.p4().mass);

      debug() << "MCParticle: "
              << " Id: "       << std::setw(3)  << j+1
              << " Pdg: "      << std::setw(5)  << particle.pdgId()
              << " Stat: "     << std::setw(2)  << particle.status()
              << " Bits: "     << std::setw(2)  << particle.bits()
              << std::scientific
              << " Px: "       << std::setprecision(2) << std::setw(9) << particle.p4().px
              << " Py: "       << std::setprecision(2) << std::setw(9) << particle.p4().py
              << " Pz: "       << std::setprecision(2) << std::setw(9) << particle.p4().pz
              << " E: "        << std::setprecision(2) << std::setw(9) << partE
              << " M: "        << std::setprecision(2) << std::setw(9) << particle.p4().mass;
      if (particle.startVertex().isAvailable()) {
        debug() << " VSId: "     << std::setw(3)  << vecPartProdVtxIDDecVtxID[j].first+1;
                //<< " Vx: "       << std::setprecision(2) << std::setw(9) << particle.startVertex().Position().X
                //<< " Vy: "       << std::setprecision(2) << std::setw(9) << particle.startVertex().Position().Y
                //<< " Vz: "       << std::setprecision(2) << std::setw(9) << particle.startVertex().Position().Z
                //<< " T: "        << std::setprecision(2) << std::setw(9) << particle.startVertex().Ctau();
      }
      if (particle.endVertex().isAvailable()) {
        debug() << " VEId: "     << std::setw(3)  << vecPartProdVtxIDDecVtxID[j].second+1;
                //<< " Vx: "       << std::setprecision(2) << std::setw(9) << particle.endVertex().Position().X
                //<< " Vy: "       << std::setprecision(2) << std::setw(9) << particle.endVertex().Position().Y
                //<< " Vz: "       << std::setprecision(2) << std::setw(9) << particle.endVertex().Position().Z
                //<< " T: "        << std::setprecision(2) << std::setw(9) << particle.endVertex().Ctau();
      }
      debug() << std::fixed << endmsg;

    } // Debug
  }
}


