#include "DelphesSimulation.h"

#include <limits>
#include "ParticleStatus.h"

using namespace std;

DECLARE_COMPONENT(DelphesSimulation)

DelphesSimulation::DelphesSimulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) ,
  m_DelphesCard(),
  m_Delphes(nullptr),
  m_DelphesFactory(nullptr),
  m_HepMCReader(nullptr),
  m_inHepMCFile(nullptr),
  m_inHepMCFileName(""),
  m_inHepMCFileLength(0),
  m_outRootFile(nullptr),
  m_outRootFileName(""),
  m_treeWriter(nullptr),
  m_branchEvent(nullptr),
  m_confReader(nullptr)
 {
   //declareProperty("filename", m_filename="" , "Name of the HepMC file to read");
   declareProperty("DelphesCard"      , m_DelphesCard    , "Name of Delphes tcl config file with detector and simulation parameters");
   declareProperty("HepMCInputFile"   , m_inHepMCFileName, "Name of HepMC input file, if defined file read in / if not data read in directly from the transient data store");
   declareProperty("ROOTOutputFile"   , m_outRootFileName, "Name of Root output file, if defined file write out / if not data written to the transient data store");

   declareInput("hepmc", m_hepmcHandle);
   
   declareOutput("genParticles"      , m_handleGenParticles);
   declareOutput("genVertices"       , m_handleGenVertices);
   declareOutput("recMuons"          , m_handleRecMuons);
   declareOutput("recElectrons"      , m_handleRecElectrons);
   declareOutput("recCharged"        , m_handleRecCharged);
   declareOutput("recNeutral"        , m_handleRecNeutral);
   declareOutput("recPhotons"        , m_handleRecPhotons);
   declareOutput("recJets"           , m_handleRecJets);
   declareOutput("recMETs"           , m_handleRecMETs);

   declareOutput("recMuonsToMC"      , m_handleRecMuonsToMC);
   declareOutput("recElectronsToMC"  , m_handleRecElectronsToMC);
   declareOutput("recChargedToMC"    , m_handleRecChargedToMC);
   declareOutput("recNeutralToMC"    , m_handleRecNeutralToMC);
   declareOutput("recPhotonsToMC"    , m_handleRecPhotonsToMC);
   //declareOutput("recJetsToPart"     , m_handleRecJetsToPart);


   m_stablePartOutArray = nullptr;
   m_allPartOutArray    = nullptr;
   m_partonOutArray     = nullptr;
   m_muonOutArray       = nullptr;
   m_electronOutArray   = nullptr;
   m_chargedOutArray    = nullptr;
   m_neutralOutArray    = nullptr;
   m_photonOutArray     = nullptr;
   m_jetOutArray        = nullptr;
   m_metOutArray        = nullptr;
   m_shtOutArray        = nullptr;

   m_eventCounter = 0;
}

StatusCode DelphesSimulation::initialize() {

  // Open HepMC file if defined
  if (m_inHepMCFileName!="") {

    info()  << "Reading in HepMC file: " << m_inHepMCFileName << endmsg;
    m_inHepMCFile = fopen(m_inHepMCFileName.c_str(), "r");

    if (m_inHepMCFile==nullptr) {

      error() << "Can't open " << m_inHepMCFileName << endmsg;
      return Error ("ERROR, can't open defined HepMC input file.");
    }
  
    fseek(m_inHepMCFile, 0L, SEEK_END);
    m_inHepMCFileLength = ftello(m_inHepMCFile);
    fseek(m_inHepMCFile, 0L, SEEK_SET);
    info() << "Length of HepMC input file: " << m_inHepMCFileLength << endmsg;
    if (m_inHepMCFileLength<=0) {
  
      fclose(m_inHepMCFile);
      return Error ("ERROR, zero length HepMC input file.");
    }
  }
  
  // If required, export output directly to root file
  if (m_outRootFileName!="") {

    info()  << "Opening ROOT output file: " << m_outRootFileName << endmsg;
    m_outRootFile = new TFile(m_outRootFileName.c_str(), "RECREATE");
    if (m_outRootFile->IsZombie()) {

      error() << "Can't open " << m_outRootFileName << endmsg;
      return Error ("ERROR, can't open defined ROOT output file.");
    }
  }

  // Read Delphes configuration card
  m_confReader = new ExRootConfReader;
  m_confReader->ReadFile(m_DelphesCard.c_str());
   
  // Instance of Delphes
  m_Delphes = new Delphes("Delphes");
  m_Delphes->SetConfReader(m_confReader);

  // Get standard Delphes factory
  m_DelphesFactory = m_Delphes->GetFactory();

  // Delphes needs data structure to be defined (ROOT tree)
  m_treeWriter  = new ExRootTreeWriter( m_outRootFile , "DelphesSim");
  m_branchEvent = m_treeWriter->NewBranch("Event", HepMCEvent::Class());
  m_Delphes->SetTreeWriter(m_treeWriter);

  // Define event readers
  //
  //  HepMC reader --> reads either from a file or directly from data store
  m_HepMCReader    = new DelphesExtHepMCReader;
  if (m_inHepMCFile) m_HepMCReader->SetInputFile(m_inHepMCFile);
  
  // Create following arrays of Delphes objects --> starting objects
  m_allPartOutArray    = m_Delphes->ExportArray("allParticles");
  m_stablePartOutArray = m_Delphes->ExportArray("stableParticles");
  m_partonOutArray     = m_Delphes->ExportArray("partons");

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
  
  // Initialize all variables
  m_eventCounter = 0;
  if (m_outRootFile!=nullptr) m_treeWriter->Clear();
  m_Delphes->Clear();
  m_HepMCReader->Clear();
 
  return StatusCode::SUCCESS;
}


StatusCode DelphesSimulation::execute() {

  //
  // Read event & initialize event variables
  TStopwatch readStopWatch;
  readStopWatch.Start();

  bool isEventReady = false;

  if (m_inHepMCFile) {

    // Test end-of-file
    if ( ftello(m_inHepMCFile) == m_inHepMCFileLength) {

      info() << "End of file reached at lenght " << m_inHepMCFileLength << endmsg;
      return StatusCode::SUCCESS;
    }

    // Read event - read line-by-line until event complete
    isEventReady = m_HepMCReader->ReadEventFromFile(m_DelphesFactory, m_allPartOutArray, m_stablePartOutArray, m_partonOutArray);
  }
  else {

    // Read event
    const HepMC::GenEvent *hepMCEvent = m_hepmcHandle.get();
    isEventReady = m_HepMCReader->ReadEventFromStore(hepMCEvent, m_DelphesFactory, m_allPartOutArray, m_stablePartOutArray, m_partonOutArray);

    // Print HepMC event info
    for(auto ipart=hepMCEvent->particles_begin(); ipart!=hepMCEvent->particles_end(); ++ipart) {

      int motherID        = 0;
      int motherIDRange   = 0;
      int daughterID      = 0;
      int daughterIDRange = 0;
      if ((*ipart)->production_vertex()!=nullptr) {

        motherID      = (*((*ipart)->production_vertex()->particles_in_const_begin()))->barcode();//(*((*ipart)->production_vertex()->particles_begin()))->barcode();
        motherIDRange = (*ipart)->production_vertex()->particles_in_size() -1;
      }
      if ((*ipart)->end_vertex()!=nullptr) {

        daughterID      = (*((*ipart)->end_vertex()->particles_out_const_begin()))->barcode();//(*((*ipart)->production_vertex()->particles_begin()))->barcode();
        daughterIDRange = (*ipart)->end_vertex()->particles_out_size() -1;
      }

      std::cout << "Delphes HepMC: "
                << " Id: "       << setw(3)  << (*ipart)->barcode()
                << " Pdg: "      << setw(5)  << (*ipart)->pdg_id()
                << " Mothers: "  << setw(3)  << motherID   << " -> " << setw(3) << motherID  +motherIDRange
                << " Daughters: "<< setw(3)  << daughterID << " -> " << setw(3) << daughterID+daughterIDRange
                << " Stat: "     << setw(2)  << (*ipart)->status()
                << " Px: "       << setw(10) << (*ipart)->momentum().px()
                << " Py: "       << setw(10) << (*ipart)->momentum().py()
                << " Pz: "       << setw(10) << (*ipart)->momentum().pz()
                << " E: "        << setw(10) << (*ipart)->momentum().e()
                << " M: "        << setw(10) << (*ipart)->momentum().m();
      if ((*ipart)->production_vertex()!=nullptr) {
      std::cout << " Vx: "       << setw(10) << (*ipart)->production_vertex()->position().x()
                << " Vy: "       << setw(10) << (*ipart)->production_vertex()->position().y()
                << " Vz: "       << setw(10) << (*ipart)->production_vertex()->position().z()
                << " T: "        << setw(10) << (*ipart)->production_vertex()->position().t();
      }
      std::cout << std::endl;
    }
  }

  if (!isEventReady) return StatusCode::FAILURE;

  // Print Delphes event info
  /*for (auto i=0; i<m_allPartOutArray->GetEntries(); i++) {

    Candidate *candidate = static_cast<Candidate *>(m_allPartOutArray->At(i));

    std::cout << "Delphes Object: "
              << " Id: "       << setw(3)  << i+1
              << " Pdg: "      << setw(5)  << candidate->PID
              << " Mothers: "  << setw(3)  << candidate->M1+1 << " -> " << setw(3) << candidate->M2+1
              << " Daughters: "<< setw(3)  << candidate->D1+1 << " -> " << setw(3) << candidate->D2+1
              << " Stat: "     << setw(2)  << candidate->Status
              << " Px: "       << setw(10) << candidate->Momentum.Px()
              << " Py: "       << setw(10) << candidate->Momentum.Py()
              << " Pz: "       << setw(10) << candidate->Momentum.Pz()
              << " E: "        << setw(10) << candidate->Momentum.E()
              << " M: "        << setw(10) << candidate->Mass
              << " Vx: "       << setw(10) << candidate->Position.X()
              << " Vy: "       << setw(10) << candidate->Position.Y()
              << " Vz: "       << setw(10) << candidate->Position.Z()
              << " T: "        << setw(10) << candidate->Position.T()
              << std::endl;
  }*/

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
  m_HepMCReader->MakeEventBranch(m_branchEvent, &readStopWatch, &procStopWatch);
  if (m_outRootFile!=nullptr) m_treeWriter->Fill();

  // FCC EDM (event-data model) based output
  auto genParticles     = new fcc::MCParticleCollection();
  auto genVertices      = new fcc::GenVertexCollection();
  auto recMuons         = new fcc::ParticleCollection();
  auto recElectrons     = new fcc::ParticleCollection();
  auto recCharged       = new fcc::ParticleCollection();
  auto recNeutral       = new fcc::ParticleCollection();
  auto recPhotons       = new fcc::ParticleCollection();
  auto recJets          = new fcc::JetCollection();
  auto recMETs          = new fcc::METCollection();

  auto recMuonsToMC     = new fcc::ParticleMCParticleAssociationCollection();
  auto recElectronsToMC = new fcc::ParticleMCParticleAssociationCollection();
  auto recChargedToMC   = new fcc::ParticleMCParticleAssociationCollection();
  auto recNeutralToMC   = new fcc::ParticleMCParticleAssociationCollection();
  auto recPhotonsToMC   = new fcc::ParticleMCParticleAssociationCollection();
  //auto recJetsToPart    = new fcc::JetParticleAssociationCollection();

  // Fill FCC collections
  m_muonOutArray     = m_Delphes->ImportArray("MuonMomentumSmearing/muons"); //m_muonOutArray     = m_Delphes->ImportArray("MuonIsolation/muons");
  m_electronOutArray = m_Delphes->ImportArray("ElectronEnergySmearing/electrons"); //m_electronOutArray = m_Delphes->ImportArray("ElectronIsolation/electrons");
  m_chargedOutArray  = m_Delphes->ImportArray("ChargedHadronMomentumSmearing/chargedHadrons");
  m_neutralOutArray  = m_Delphes->ImportArray("Hcal/eflowNeutralHadrons");
  m_photonOutArray   = m_Delphes->ImportArray("PhotonEfficiency/photons"); //m_photonOutArray   = m_Delphes->ImportArray("PhotonIsolation/photons");
  m_jetOutArray      = m_Delphes->ImportArray("JetEnergyScale/jets");
  m_metOutArray      = m_Delphes->ImportArray("MissingET/momentum");
  m_shtOutArray      = m_Delphes->ImportArray("ScalarHT/energy");

  DelphesSimulation::ConvertMCParticles(m_allPartOutArray , genParticles , genVertices);
  DelphesSimulation::ConvertTracks(     m_muonOutArray    , recMuons     , genParticles, recMuonsToMC    );
  DelphesSimulation::ConvertTracks(     m_electronOutArray, recElectrons , genParticles, recElectronsToMC);
  DelphesSimulation::ConvertTracks(     m_chargedOutArray , recCharged   , genParticles, recChargedToMC  );
  DelphesSimulation::ConvertTowers(     m_neutralOutArray , recNeutral   , genParticles, recNeutralToMC  );
  DelphesSimulation::ConvertTowers(     m_photonOutArray  , recPhotons   , genParticles, recPhotonsToMC  );
//  DelphesSimulation::ConvertJets(       m_jetOutArray     , recJets);
  DelphesSimulation::ConvertMET(        m_metOutArray     , m_shtOutArray, recMETs);

  // Save FCC-EDM collections to FCCSw data store
  m_handleGenParticles.put(genParticles);
  m_handleGenVertices.put(genVertices);
  m_handleRecMuons.put(recMuons);
  m_handleRecMuonsToMC.put(recMuonsToMC);
  m_handleRecElectrons.put(recElectrons);
  m_handleRecElectronsToMC.put(recElectronsToMC);
  m_handleRecCharged.put(recCharged);
  m_handleRecChargedToMC.put(recChargedToMC);
  m_handleRecNeutral.put(recNeutral);
  m_handleRecNeutralToMC.put(recNeutralToMC);
  m_handleRecPhotons.put(recPhotons);
  m_handleRecPhotonsToMC.put(recPhotonsToMC);
  m_handleRecJets.put(recJets);
  //m_handleRecJetsToPart.put(recJetsToPart);
  m_handleRecMETs.put(recMETs);

  // Initialize for next event reading (Will also zero Delphes arrays)
  if (m_outRootFile!=nullptr) m_treeWriter->Clear();
  m_Delphes->Clear();
  m_HepMCReader->Clear();

  return StatusCode::SUCCESS;
}

StatusCode DelphesSimulation::finalize() {

  // Finish Delphes task
  m_Delphes->FinishTask();

  // Close HepMC input file if defined
  if (m_inHepMCFile!=nullptr) {

    fclose(m_inHepMCFile);
  }

  // Write output to Root file
  if (m_outRootFile!=nullptr) {

    m_treeWriter->Write();
    m_outRootFile->Close();
    if (m_outRootFile){delete m_outRootFile; m_outRootFile = nullptr;}
  }
  
  info() << "Exiting Delphes..." << endmsg;
  
  // Clear memory
  if (m_HepMCReader) {delete m_HepMCReader; m_HepMCReader = nullptr; } // Releases also the memory allocated by inHepMCFile
  if (m_Delphes)     {delete m_Delphes;     m_Delphes     = nullptr; } // Releases also the memory allocated by treeWriter
  if (m_confReader)  {delete m_confReader;  m_confReader  = nullptr; }
  
  return GaudiAlgorithm::finalize();
}

void DelphesSimulation::ConvertMCParticles(const TObjArray* Input ,
                                           fcc::MCParticleCollection* colMCParticles, fcc::GenVertexCollection* colGenVertices)
{
  //MC particle vertex mapping: production & decay vertex
  std::vector<std::pair<int, int>> m_vecPartProdVtxIDDecVtxID;

  m_vecPartProdVtxIDDecVtxID.resize(Input->GetEntries());
  for(int j=0; j<Input->GetEntries(); j++) {
    m_vecPartProdVtxIDDecVtxID[j].first  = -1;
    m_vecPartProdVtxIDDecVtxID[j].second = -1;
  }

  Candidate* cand = nullptr;
  for(int j=0; j<Input->GetEntries(); j++) {

    cand = static_cast<Candidate *>(m_allPartOutArray->At(j));

    auto particle      = colMCParticles->create();
    auto barePart      = new fcc::BareParticle();
    barePart->Type     = cand->PID;
    barePart->Status   = cand->Status;
    barePart->P4.Px    = cand->Momentum.Px();
    barePart->P4.Py    = cand->Momentum.Py();
    barePart->P4.Pz    = cand->Momentum.Pz();
    barePart->P4.Mass  = cand->Momentum.M();
    barePart->Charge   = cand->Charge;
    barePart->Vertex.X = cand->Position.X();
    barePart->Vertex.Y = cand->Position.Y();
    barePart->Vertex.Z = cand->Position.Z();

    if (cand->M1==-1)      barePart->Bits = ParticleStatus::Beam;
    else if (cand->D1==-1) barePart->Bits = ParticleStatus::Stable;
    else                   barePart->Bits = ParticleStatus::Decayed;

    particle.Core(*barePart);
    if (barePart!=nullptr) delete barePart;

    // Mapping the vertices
    int idPartStartVertex = m_vecPartProdVtxIDDecVtxID[j].first;
    int idPartEndVertex   = m_vecPartProdVtxIDDecVtxID[j].second;

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
      }
      for (int iMother=cand->M1; iMother<=cand->M2; iMother++) {
        if (m_vecPartProdVtxIDDecVtxID[iMother].second==-1) m_vecPartProdVtxIDDecVtxID[iMother].second = j;
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
      }
      for (int iDaughter=cand->D1; iDaughter<=cand->D2; iDaughter++) {
        if (iDaughter>=0 && m_vecPartProdVtxIDDecVtxID[iDaughter].second==-1) m_vecPartProdVtxIDDecVtxID[iDaughter].first = j;
      }
    }
    // Test print
    /*std::cout << "Delphes MCParticle: "
              << " Id: "       << setw(3)  << j+1
              << " Pdg: "      << setw(5)  << particle.Core().Type
              << " Stat: "     << setw(2)  << particle.Core().Status
              << " Bits: "     << setw(2)  << particle.Core().Bits
              << " Px: "       << setw(10) << particle.Core().P4.Px
              << " Py: "       << setw(10) << particle.Core().P4.Py
              << " Pz: "       << setw(10) << particle.Core().P4.Pz
              << " E: "        << setw(10) << sqrt(particle.Core().P4.Px*particle.Core().P4.Px + particle.Core().P4.Py*particle.Core().P4.Py + particle.Core().P4.Pz*particle.Core().P4.Pz + particle.Core().P4.Mass*particle.Core().P4.Mass)
              << " M: "        << setw(10) << particle.Core().P4.Mass;
    if (particle.StartVertex().isAvailable()) {
    std::cout << " VId: "      << setw(3)  << particle.StartVertex().getObjectID().index
              << " Vx: "       << setw(10) << particle.StartVertex().Position().X
              << " Vy: "       << setw(10) << particle.StartVertex().Position().Y
              << " Vz: "       << setw(10) << particle.StartVertex().Position().Z
              << " T: "        << setw(10) << particle.StartVertex().Ctau();
    }
    if (particle.EndVertex().isAvailable()) {
    std::cout << " VId: "      << setw(3)  << particle.EndVertex().getObjectID().index
              << " Vx: "       << setw(10) << particle.EndVertex().Position().X
              << " Vy: "       << setw(10) << particle.EndVertex().Position().Y
              << " Vz: "       << setw(10) << particle.EndVertex().Position().Z
              << " T: "        << setw(10) << particle.EndVertex().Ctau();
    }
    std::cout << std::endl;*/
  }
}   

void DelphesSimulation::ConvertTracks(const TObjArray*  Input,
                                      fcc::ParticleCollection*   colParticles,
                                      fcc::MCParticleCollection* colMCParticles,
                                      fcc::ParticleMCParticleAssociationCollection* ascColParticlesToMC)
{
  Candidate* cand = nullptr;
  for(int j=0; j<Input->GetEntries(); j++) {

    cand = static_cast<Candidate *>(Input->At(j));

    auto particle = colParticles->create();
    auto relation = ascColParticlesToMC->create();

    auto barePart      = new fcc::BareParticle();
    barePart->Type     = cand->PID;
    barePart->Status   = cand->Status;
    barePart->P4.Px    = cand->Momentum.Px();
    barePart->P4.Py    = cand->Momentum.Py();
    barePart->P4.Pz    = cand->Momentum.Pz();
    barePart->P4.Mass  = cand->Momentum.M();
    barePart->Charge   = cand->Charge;
    barePart->Vertex.X = cand->Position.X();
    barePart->Vertex.Y = cand->Position.Y();
    barePart->Vertex.Z = cand->Position.Z();

    // Reference to MC - Delphes holds references to all objects related to the <T> object, only one relates to MC particle
    Candidate* refCand = nullptr;
    int idRefMCPart    = -1;
    if (cand->GetCandidates()->GetEntries()>0) {

      refCand     = static_cast<Candidate*>(cand->GetCandidates()->At(0));
      idRefMCPart = refCand->GetUniqueID()-1;     // Use C numbering from 0
      if (idRefMCPart<colMCParticles->size()) {

        barePart->Bits = ParticleStatus::Matched;
        particle.Core(*barePart);
        relation.Rec(particle);
        relation.Sim(colMCParticles->at(idRefMCPart));
      }
      else {
        barePart->Bits = ParticleStatus::Unmatched;
        particle.Core(*barePart);
        std::cout << "WARNING: Can't build relation from Electron/Muon/ChHadron to MC particle!" << std::endl;
      }
    }
    else {
      barePart->Bits = ParticleStatus::Unmatched;
      particle.Core(*barePart);
      std::cout << "WARNING: Can't build relation from Electron/Muon/ChHadron to MC particle!" << std::endl;
    }
    if (barePart!=nullptr) delete barePart;

    // Test print
    /*double energy = sqrt(particle.Core().P4.Px*particle.Core().P4.Px +
                         particle.Core().P4.Py*particle.Core().P4.Py +
                         particle.Core().P4.Pz*particle.Core().P4.Pz +
                         particle.Core().P4.Mass*particle.Core().P4.Mass);
    double recE   = sqrt(relation.Rec().Core().P4.Px*relation.Rec().Core().P4.Px +
                         relation.Rec().Core().P4.Py*relation.Rec().Core().P4.Py +
                         relation.Rec().Core().P4.Pz*relation.Rec().Core().P4.Pz +
                         relation.Rec().Core().P4.Mass*relation.Rec().Core().P4.Mass);
    double simE   = sqrt(relation.Sim().Core().P4.Px*relation.Sim().Core().P4.Px +
                         relation.Sim().Core().P4.Py*relation.Sim().Core().P4.Py +
                         relation.Sim().Core().P4.Pz*relation.Sim().Core().P4.Pz +
                         relation.Sim().Core().P4.Mass*relation.Sim().Core().P4.Mass);

    std::cout << "Delphes Track: "
              << " Id: "       << setw(3)  << j+1
              << " Pdg: "      << setw(5)  << particle.Core().Type
              << " Stat: "     << setw(2)  << particle.Core().Status
              << " Bits: "     << setw(2)  << particle.Core().Bits
              << " Px: "       << setw(10) << particle.Core().P4.Px
              << " Py: "       << setw(10) << particle.Core().P4.Py
              << " Pz: "       << setw(10) << particle.Core().P4.Pz
              << " E: "        << setw(10) << energy
              << " M: "        << setw(10) << particle.Core().P4.Mass
              << " Vx: "       << setw(10) << particle.Core().Vertex.X
              << " Vy: "       << setw(10) << particle.Core().Vertex.Y
              << " Vz: "       << setw(10) << particle.Core().Vertex.Z
              << " RefId: "    << setw(3)  << idRefMCPart+1
              << " Rel E: "    << setw(10) << simE << " " << recE;
    std::cout << std::endl;*/
  }
}

void DelphesSimulation::ConvertTowers(const TObjArray*  Input,
                                      fcc::ParticleCollection*   colParticles,
                                      fcc::MCParticleCollection* colMCParticles,
                                      fcc::ParticleMCParticleAssociationCollection* ascColParticlesToMC)
{
  Candidate* cand = nullptr;
  for(int j=0; j<Input->GetEntries(); j++) {

    cand = static_cast<Candidate *>(Input->At(j));

    auto particle = colParticles->create();

    auto barePart      = new fcc::BareParticle();
    barePart->Type     = cand->PID;
    barePart->Status   = cand->Status;
    barePart->P4.Px    = cand->Momentum.Px();
    barePart->P4.Py    = cand->Momentum.Py();
    barePart->P4.Pz    = cand->Momentum.Pz();
    barePart->P4.Mass  = cand->Momentum.M();
    barePart->Charge   = cand->Charge;
    barePart->Vertex.X = cand->Position.X();
    barePart->Vertex.Y = cand->Position.Y();
    barePart->Vertex.Z = cand->Position.Z();
    particle.Core(*barePart);

    // Reference to MC - Delphes holds references to all objects related to the Photon object, several relations might exist for gammas
    TIter it1(cand->GetCandidates());
    it1.Reset();

    Candidate* refCand  = nullptr;
    Candidate* ref2Cand = nullptr;

    // Test print
    /*double energy = sqrt(particle.Core().P4.Px*particle.Core().P4.Px +
                         particle.Core().P4.Py*particle.Core().P4.Py +
                         particle.Core().P4.Pz*particle.Core().P4.Pz +
                         particle.Core().P4.Mass*particle.Core().P4.Mass);
    std::cout << "Delphes Tower: "
              << " Id: "       << setw(3)  << j+1
              << " Pdg: "      << setw(5)  << particle.Core().Type
              << " Stat: "     << setw(2)  << particle.Core().Status
              << " Bits: "     << setw(2)  << particle.Core().Bits
              << " Px: "       << setw(10) << particle.Core().P4.Px
              << " Py: "       << setw(10) << particle.Core().P4.Py
              << " Pz: "       << setw(10) << particle.Core().P4.Pz
              << " E: "        << setw(10) << energy
              << " M: "        << setw(10) << particle.Core().P4.Mass
              << " Vx: "       << setw(10) << particle.Core().Vertex.X
              << " Vy: "       << setw(10) << particle.Core().Vertex.Y
              << " Vz: "       << setw(10) << particle.Core().Vertex.Z;*/

    while((refCand = static_cast<Candidate*>(it1.Next()))) {

      TIter it2(refCand->GetCandidates());
      it2.Reset();
      while((ref2Cand = static_cast<Candidate*>(it2.Next()))) {

        int idRefMCPart = ref2Cand->GetUniqueID()-1;
        if (idRefMCPart<colMCParticles->size()) {

          auto relation = ascColParticlesToMC->create();
          relation.Rec(particle);
          relation.Sim(colMCParticles->at(idRefMCPart));

          // Test print
          /*double recE   = sqrt(relation.Rec().Core().P4.Px*relation.Rec().Core().P4.Px +
                               relation.Rec().Core().P4.Py*relation.Rec().Core().P4.Py +
                               relation.Rec().Core().P4.Pz*relation.Rec().Core().P4.Pz +
                               relation.Rec().Core().P4.Mass*relation.Rec().Core().P4.Mass);
          double simE   = sqrt(relation.Sim().Core().P4.Px*relation.Sim().Core().P4.Px +
                               relation.Sim().Core().P4.Py*relation.Sim().Core().P4.Py +
                               relation.Sim().Core().P4.Pz*relation.Sim().Core().P4.Pz +
                               relation.Sim().Core().P4.Mass*relation.Sim().Core().P4.Mass);
          std::cout << " RefId: "    << setw(3)  << idRefMCPart+1
                    << " Rel E: "    << setw(10) << simE << " " << recE;
          if (colMCParticles->at(idRefMCPart).Core().Type ==22) std::cout << " Gamma";
          if (colMCParticles->at(idRefMCPart).Core().Charge==0) std::cout << " Neutral";*/

        }
        else {
          std::cout << "WARNING: Can't build one of the relations from Photon/NHadron to MC particle!" << std::endl;
        }
      }
    }
    // Test print
    /*std::cout << std::endl;*/
  }
}

void DelphesSimulation::ConvertJets(const TObjArray* Input, fcc::JetCollection* colJets)
{
  Jet* cand = nullptr;
  for(int j = 0; j < Input->GetEntries(); ++j) {
      
    cand = static_cast<Jet *>(Input->At(j));

    auto jet = colJets->create();

    auto bareJet      = new fcc::BareJet();
    bareJet->Area     = -1;
    bareJet->P4.Px    = (double) cand->P4().X();
    bareJet->P4.Py    = (double) cand->P4().Y();
    bareJet->P4.Pz    = (double) cand->P4().Z();
    bareJet->P4.Mass  = (double) cand->Mass ;

    jet.Core(*bareJet);
    delete bareJet;
  }
}   

void DelphesSimulation::ConvertMET(const TObjArray* InputMET , const TObjArray* InputSHT, fcc::METCollection*  colMET){

  Candidate* candMET;
  Candidate* candSHT;

  bool saveSHT = true;
  if (InputMET->GetEntries()!=InputSHT->GetEntries()) {

    saveSHT = false;
    std::cout << "WARNING: Can't save in a common FCC-EDM MET object both information from Delphes MET & scalarHT. Only MET will be saved!" << std::endl;
  }

  for(int j = 0; j < InputMET->GetEntries(); ++j) {
      
    candMET = static_cast<Candidate *>(InputMET->At(j));
    if (saveSHT) candSHT = static_cast<Candidate *>(InputSHT->At(j));

    auto met = colMET->create();

    met.Magnitude(candMET->Momentum.Pt());
    met.Phi((-(candMET->Momentum)).Phi());
    if (saveSHT) met.ScalarSum(candSHT->Momentum.Pt());
    else         met.ScalarSum(-1);

    // Test
    /*std::cout << " MET: " << met.Magnitude()
              << " Phi: " << met.Phi()
              << " sHT: " << met.ScalarSum()
              << std::endl;*/
  }
}   
