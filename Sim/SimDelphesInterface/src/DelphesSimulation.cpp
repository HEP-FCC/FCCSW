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

    auto barePart     = fcc::BareParticle();
    barePart.Type     = cand->PID;
    barePart.Status   = cand->Status;
    barePart.P4.Px    = cand->Momentum.Px();
    barePart.P4.Py    = cand->Momentum.Py();
    barePart.P4.Pz    = cand->Momentum.Pz();
    barePart.P4.Mass  = cand->Momentum.M();
    barePart.Charge   = cand->Charge;
    barePart.Vertex.X = cand->Position.X();
    barePart.Vertex.Y = cand->Position.Y();
    barePart.Vertex.Z = cand->Position.Z();

    if (cand->M1==-1)      barePart.Bits = static_cast<unsigned>(ParticleStatus::kBeam);
    else if (cand->D1==-1) barePart.Bits = static_cast<unsigned>(ParticleStatus::kStable);
    else                   barePart.Bits = static_cast<unsigned>(ParticleStatus::kDecayed);

    particle.Core(barePart);

    // Mapping the vertices
    int& idPartStartVertex = vecPartProdVtxIDDecVtxID[j].first;
    int& idPartEndVertex   = vecPartProdVtxIDDecVtxID[j].second;

    // Production vertex
    if (cand->M1!=-1) {
      if (idPartStartVertex!=-1) {
        particle.StartVertex(colMCParticles->at(idPartStartVertex).EndVertex());
      }
      else {
        fcc::Point point;
        point.X = cand->Position.X();
        point.Y = cand->Position.Y();
        point.Z = cand->Position.Z();

        auto vertex = colGenVertices->create();
        vertex.Position(point);
        vertex.Ctau(cand->Position.T());
        particle.StartVertex(vertex);

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
        particle.EndVertex(colMCParticles->at(idPartEndVertex).StartVertex());
      }
      else {
        fcc::Point point;
        point.X  = daughter->Position.X();
        point.Y  = daughter->Position.Y();
        point.Z  = daughter->Position.Z();

        auto vertex = colGenVertices->create();
        vertex.Position(point);
        vertex.Ctau(cand->Position.T());
        particle.EndVertex(vertex);

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

      double partE = sqrt(particle.Core().P4.Px*particle.Core().P4.Px +
                          particle.Core().P4.Py*particle.Core().P4.Py +
                          particle.Core().P4.Pz*particle.Core().P4.Pz +
                          particle.Core().P4.Mass*particle.Core().P4.Mass);

      debug() << "MCParticle: "
              << " Id: "       << std::setw(3)  << j+1
              << " Pdg: "      << std::setw(5)  << particle.Core().Type
              << " Stat: "     << std::setw(2)  << particle.Core().Status
              << " Bits: "     << std::setw(2)  << particle.Core().Bits
              << std::scientific
              << " Px: "       << std::setprecision(2) << std::setw(9) << particle.Core().P4.Px
              << " Py: "       << std::setprecision(2) << std::setw(9) << particle.Core().P4.Py
              << " Pz: "       << std::setprecision(2) << std::setw(9) << particle.Core().P4.Pz
              << " E: "        << std::setprecision(2) << std::setw(9) << partE
              << " M: "        << std::setprecision(2) << std::setw(9) << particle.Core().P4.Mass;
      if (particle.StartVertex().isAvailable()) {
        debug() << " VSId: "     << std::setw(3)  << vecPartProdVtxIDDecVtxID[j].first+1;
                //<< " Vx: "       << std::setprecision(2) << std::setw(9) << particle.StartVertex().Position().X
                //<< " Vy: "       << std::setprecision(2) << std::setw(9) << particle.StartVertex().Position().Y
                //<< " Vz: "       << std::setprecision(2) << std::setw(9) << particle.StartVertex().Position().Z
                //<< " T: "        << std::setprecision(2) << std::setw(9) << particle.StartVertex().Ctau();
      }
      if (particle.EndVertex().isAvailable()) {
        debug() << " VEId: "     << std::setw(3)  << vecPartProdVtxIDDecVtxID[j].second+1;
                //<< " Vx: "       << std::setprecision(2) << std::setw(9) << particle.EndVertex().Position().X
                //<< " Vy: "       << std::setprecision(2) << std::setw(9) << particle.EndVertex().Position().Y
                //<< " Vz: "       << std::setprecision(2) << std::setw(9) << particle.EndVertex().Position().Z
                //<< " T: "        << std::setprecision(2) << std::setw(9) << particle.EndVertex().Ctau();
      }
      debug() << std::fixed << endmsg;

    } // Debug
  }
}


