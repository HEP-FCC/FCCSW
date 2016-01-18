#include "DelphesSimulation.h"

using namespace std;

DECLARE_COMPONENT(DelphesSimulation)

DelphesSimulation::DelphesSimulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) ,
  m_DelphesCard(),
  m_outCollections(),
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
   declareProperty("OutputCollections", m_outCollections , "Names of ouput collections, written out by Delphes: muons, electrons, photons, jets, mets, hts");
   declareProperty("HepMCInputFile"   , m_inHepMCFileName, "Name of HepMC input file, if defined file read in / if not data read in directly from the transient data store");
   declareProperty("ROOTOutputFile"   , m_outRootFileName, "Name of Root output file, if defined file write out / if not data written to the transient data store");

   declareInput("hepmc", m_hepmcHandle);

   declareOutput("allParticles"      , m_allPartclsHandle);
   declareOutput("genPartons"        , m_genPartonsHandle);
   declareOutput("genStableParticles", m_genStablesHandle);
   declareOutput("recMuons"          , m_recMuonHandle);
   declareOutput("recElectrons"      , m_recElecHandle);
   declareOutput("recPhotons"        , m_recPhotHandle);
   declareOutput("recJets"           , m_recJetsHandle);
   declareOutput("recMETs"           , m_recMETHandle);
   declareOutput("recHTs"            , m_recHTSHandle);

   m_stablePartOutArray = nullptr;
   m_allPartOutArray    = nullptr;
   m_partonOutArray     = nullptr;
   m_muonOutArray       = nullptr;
   m_electronOutArray   = nullptr;
   m_photonOutArray     = nullptr;
   m_jetOutArray        = nullptr;
   m_metOutArray        = nullptr;
   m_htOutArray         = nullptr;

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
    /*for(auto ipart=hepMCEvent->particles_begin(); ipart!=hepMCEvent->particles_end(); ++ipart) {

      int motherID                = -1;
      int motherIDRange           = 0;
      int daughterID              = -1;
      int daughterIDRange         = 0;
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
                << " Mothers: "  << setw(3)  << motherID << " -> " << setw(3) << motherID+motherIDRange
                << " Daughters: "<< setw(3)  << daughterID << " -> " << setw(3) << daughterID+daughterIDRange
                << " Stat: "     << setw(2)  << (*ipart)->status()
                << " Px: "       << setw(10) << (*ipart)->momentum().px()
                << " Py: "       << setw(10) << (*ipart)->momentum().py()
                << " Pz: "       << setw(10) << (*ipart)->momentum().pz()
                << " E: "        << setw(10) << (*ipart)->momentum().e()
                << " M: "        << setw(10) << (*ipart)->momentum().m();
      if ((*ipart)->production_vertex()) {
      std::cout << " Vx: "       << setw(10) << (*ipart)->production_vertex()->position().x()
                << " Vy: "       << setw(10) << (*ipart)->production_vertex()->position().y()
                << " Vz: "       << setw(10) << (*ipart)->production_vertex()->position().z();
      }
      std::cout << std::endl;
    }*/
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
  /*ParticleCollection* allParticles       = new ParticleCollection();
  ParticleCollection* genPartons         = new ParticleCollection();
  ParticleCollection* genStableParticles = new ParticleCollection();
  ParticleCollection* recMuons           = nullptr;
  ParticleCollection* recElectrons       = nullptr;
  ParticleCollection* recPhotons         = nullptr;
  GenJetCollection*   recJets            = nullptr;
  ParticleCollection* recMETs            = nullptr;
  ParticleCollection* recHTs             = nullptr;
  for ( unsigned int l=0; l<m_outCollections.size() ; l++){
    if ( m_outCollections[l].find("muons")     != std::string::npos ) recMuons     = new ParticleCollection();
    if ( m_outCollections[l].find("electrons") != std::string::npos ) recElectrons = new ParticleCollection();
    if ( m_outCollections[l].find("photons")   != std::string::npos ) recPhotons   = new ParticleCollection();
    if ( m_outCollections[l].find("jets")      != std::string::npos ) recJets      = new GenJetCollection();
    if ( m_outCollections[l].find("mets")      != std::string::npos ) recMETs      = new ParticleCollection();
    if ( m_outCollections[l].find("hts")       != std::string::npos ) recHTs       = new ParticleCollection();
  }

  // Generated FCC output: collections
  DelphesSimulation::ConvertParticle( m_allPartOutArray   , allParticles      );
  DelphesSimulation::ConvertParticle( m_stablePartOutArray, genStableParticles);
  DelphesSimulation::ConvertParticle( m_partonOutArray    , genPartons        );

  // Reconstructed objects from Delphes
  const char* name;
  for ( unsigned int l=0; l<m_outCollections.size() ; l++) {

    if ( m_outCollections[l].find("muons")!= std::string::npos ) {
      name = m_Delphes->GetString("Branch",m_outCollections[l].c_str() ) ;
      m_muonOutArray = m_Delphes->ImportArray(name);
      DelphesSimulation::ConvertParticle( m_muonOutArray, recMuons );
    }
    if ( m_outCollections[l].find("electrons")!= std::string::npos ) {
      name = m_Delphes->GetString("Branch",m_outCollections[l].c_str() ) ;
      m_electronOutArray = m_Delphes->ImportArray(name);
      DelphesSimulation::ConvertParticle( m_electronOutArray, recElectrons );
    }
    if ( m_outCollections[l].find("photons")!= std::string::npos ) {
      name = m_Delphes->GetString("Branch",m_outCollections[l].c_str() ) ;
      m_photonOutArray = m_Delphes->ImportArray(name);
      DelphesSimulation::ConvertParticle( m_photonOutArray, recPhotons );
    }
    if ( m_outCollections[l].find("jets")!= std::string::npos ) {
      name = m_Delphes->GetString("Branch",m_outCollections[l].c_str() ) ;
      m_jetOutArray = m_Delphes->ImportArray(name);
      DelphesSimulation::ConvertJet( m_jetOutArray, recJets );
    }
    if ( m_outCollections[l].find("MissingET")!= std::string::npos ) {
      name = m_Delphes->GetString("Branch",m_outCollections[l].c_str() ) ;
      m_metOutArray = m_Delphes->ImportArray(name);
      DelphesSimulation::ConvertMET( m_metOutArray, recMETs );
    }
    if ( m_outCollections[l].find("ScalarHT")!= std::string::npos ) {
      name =  m_Delphes->GetString("Branch",m_outCollections[l].c_str() ) ;
      m_htOutArray = m_Delphes->ImportArray(name);
      DelphesSimulation::ConvertParticle( m_htOutArray, recHTs );
    }
  }

  // Save particle collections to FCCSw data store
  m_allPartclsHandle.put(allParticles      );
  m_genPartonsHandle.put(genPartons        );
  m_genStablesHandle.put(genStableParticles);
  for ( unsigned int l=0; l<m_outCollections.size() ; l++){
    if ( m_outCollections[l].find("muons")    != std::string::npos ) m_recMuonHandle.put(recMuons);
    if ( m_outCollections[l].find("electrons")!= std::string::npos ) m_recElecHandle.put(recElectrons);
    if ( m_outCollections[l].find("photons")  != std::string::npos ) m_recPhotHandle.put(recPhotons);
    if ( m_outCollections[l].find("jets")     != std::string::npos ) m_recJetsHandle.put(recJets);
    if ( m_outCollections[l].find("mets")     != std::string::npos ) m_recMETHandle.put(recMETs);
    if ( m_outCollections[l].find("hts")      != std::string::npos ) m_recHTSHandle.put(recHTs);
  }*/

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

void DelphesSimulation::ConvertParticle(   TObjArray *  Input , ParticleCollection *  coll  ){

  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j) {

    cand = static_cast<Candidate *>(Input->At(j));
    Particle outptc = coll->create();
    BareParticle&  core   = outptc.Core();
    if (cand->Momentum.Pt()!=0) { // protection against the boring message Warning in <TVector3::PseudoRapidity>: transvers momentum = 0! return +/- 10e10

      core.Type     = cand->PID;
	    core.Status   = cand->Status;
    	core.P4.Px    = (double) cand->Momentum.X();
	    core.P4.Py    = (double) cand->Momentum.Y();
	    core.P4.Pz    = (double) cand->Momentum.Z();
	    core.P4.Mass  = (double) cand->Mass ;
	    core.Vertex.X = (double) cand->Position.X() ;
	    core.Vertex.Y = (double) cand->Position.Y() ;
	    core.Vertex.Z = (double) cand->Position.Z() ;
    }
  }
}

void DelphesSimulation::ConvertJet(   TObjArray *  Input , GenJetCollection *  coll  ){

  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j) {

    cand = static_cast<Candidate *>(Input->At(j));
    GenJet outptc = coll->create();
    BareJet& core = outptc.Core();
    core.Area     = cand->Area.Mag();
    core.P4.Px    = (double) cand->Momentum.X();
    core.P4.Py    = (double) cand->Momentum.Y();
    core.P4.Pz    = (double) cand->Momentum.Z();
    core.P4.Mass  = (double) cand->Mass ;
  }
}

void DelphesSimulation::ConvertMET(   TObjArray *  Input , ParticleCollection *  coll  ){

  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j) {

    cand = static_cast<Candidate *>(Input->At(j));
    Particle outptc = coll->create();
    BareParticle& core = outptc.Core();
    core.P4.Px         = (double) cand->Momentum.X();
    core.P4.Py         = (double) cand->Momentum.Y();
  }
}

void DelphesSimulation::ConvertHT(   TObjArray *  Input , ParticleCollection *  coll  ){

  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j) {

    cand = static_cast<Candidate *>(Input->At(j));
    Particle outptc = coll->create();
    BareParticle& core = outptc.Core();
    core.P4.Px         = (double) cand->Momentum.X();
    core.P4.Py         = (double) cand->Momentum.Y();
  }
}
