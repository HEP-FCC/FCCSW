#include "DelphesSimulation.h"


using namespace std;


DECLARE_COMPONENT(DelphesSimulation)

DelphesSimulation::DelphesSimulation(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) ,
  m_filename(),
  m_detectorcard(),
  m_outputcollections()
 {
   declareProperty("filename", m_filename="" , "Name of the HepMC file to read");
   declareProperty("detectorcard", m_detectorcard="" , "Name of the tcl file with detector and simulation configuration");
   declareProperty("outputcollections", m_outputcollections  , "strings of ouput corrections, corresponding to the one wrote by Delphes");
   declareProperty("debug_delphes", m_debug_delphes  , "boolean,if true one runs delphes standalone and a separate output is saved");
   
   declareOutput("genparticles", m_recgphandle);
   declareOutput("partons", m_recparthandle);
   declareOutput("particles", m_recphandle);
   declareOutput("muons", m_recmhandle);
   declareOutput("electrons", m_recehandle);
   declareOutput("photons", m_recphhandle);
   declareOutput("jets", m_recjhandle);
   declareOutput("mets", m_recmethandle);
   declareOutput("hts", m_rechthandle);

}

StatusCode DelphesSimulation::initialize() {
  inputFile = 0;
  outputFile = 0; 
  
  // the input files is an HEPMC file
  info()  << "** Reading " << m_filename << endmsg;
  inputFile = fopen(m_filename.c_str(), "r");
  
  if(inputFile == NULL)
    {
      debug() << "can't open " << m_filename << endmsg;
      return Error ("ERROR, can't open hepmc input file ");
    }
  
  fseek(inputFile, 0L, SEEK_END);     
  length = ftello(inputFile);
  fseek(inputFile, 0L, SEEK_SET);
  info() << "** length of input file " << length << endmsg;
  if(length <= 0)
    {
      fclose(inputFile);
    }
   confReader = new ExRootConfReader;
   confReader->ReadFile(m_detectorcard.c_str());
   
     
  modularDelphes = new Delphes("Delphes");
  modularDelphes->SetConfReader(confReader);

  if (m_debug_delphes) outputFile = TFile::Open("DelphesOutput.root", "RECREATE");

  treeWriter = new ExRootTreeWriter( outputFile , "Delphes"); 

  modularDelphes->SetTreeWriter(treeWriter);
  branchEvent = treeWriter->NewBranch("Event", HepMCEvent::Class());
  
  factory = modularDelphes->GetFactory();
  
  allParticleOutputArray = modularDelphes->ExportArray("allParticles");
  stableParticleOutputArray = modularDelphes->ExportArray("stableParticles");
  partonOutputArray = modularDelphes->ExportArray("partons");
  
  reader = new DelphesHepMCReader;
  
   modularDelphes->InitTask();
   reader->SetInputFile(inputFile);
   TString name;
   //  const ExRootConfReader::ExRootTaskMap *modules = confReader->GetModules();
  //  ExRootConfReader::ExRootTaskMap::const_iterator itModules;
   ExRootConfParam param = confReader->GetParam("::ExecutionPath");
   Long_t k, size = param.GetSize();
   for( k = 0; k < size; ++k)
     {
	name = param[k].GetString();
	info()  << "the cfg contains a delphes module with name " <<  name << endmsg;
     } 
   

   eventCounter = 0 ;
   treeWriter->Clear();
   modularDelphes->Clear();
   reader->Clear();  
 
   return StatusCode::SUCCESS;
}


StatusCode DelphesSimulation::execute() {
  // delphes stuff here
 readStopWatch.Start(); 
 bool eventReady = false;

 ParticleCollection* genparticles = new ParticleCollection();
 ParticleCollection* partons = new ParticleCollection();
 ParticleCollection* particles = new ParticleCollection();
 ParticleCollection* muons = new ParticleCollection();
 ParticleCollection* electrons = new ParticleCollection();
 ParticleCollection* photons = new ParticleCollection();
 GenJetCollection* jets = new GenJetCollection();
 ParticleCollection* mets = new ParticleCollection();
 ParticleCollection* hts = new ParticleCollection();
 

 const char *  _string;
 
   if ( ftello(inputFile) == length) {
     info() << "** end of file reached at lenght " << length << endmsg ;
     return StatusCode::SUCCESS;
   }
   
   while(
	 reader->ReadBlock(factory, allParticleOutputArray, stableParticleOutputArray, partonOutputArray) && !eventReady && (ftello(inputFile) != length )  ) 
     {

       if(reader->EventReady())
	 {
	   
	   eventCounter++;
	   eventReady = true;
	   
	   readStopWatch.Stop();
	   
	   
	   procStopWatch.Start();
	   modularDelphes->ProcessTask();
	   procStopWatch.Stop();           
	   reader->AnalyzeEvent(branchEvent, eventCounter, &readStopWatch, &procStopWatch);
	   if (m_debug_delphes) treeWriter->Fill();
	   
	   DelphesSimulation::ConvertParticle( stableParticleOutputArray , particles  );
	   DelphesSimulation::ConvertParticle( allParticleOutputArray , genparticles  );
	   DelphesSimulation::ConvertParticle( partonOutputArray , partons  );
	   
	   for ( unsigned int l=0; l<m_outputcollections.size() ; l++){
	     if ( m_outputcollections[l].find("muons")!= std::string::npos ) {
	       _string =  modularDelphes->GetString("Branch",m_outputcollections[l].c_str() ) ;
	       muonOutputArray = modularDelphes->ImportArray( _string);
	       DelphesSimulation::ConvertParticle( muonOutputArray , muons  );
	     }
	     if ( m_outputcollections[l].find("electrons")!= std::string::npos ) {
	       _string =  modularDelphes->GetString("Branch",m_outputcollections[l].c_str() ) ;
	       electronOutputArray = modularDelphes->ImportArray( _string);
	       DelphesSimulation::ConvertParticle( electronOutputArray , electrons  );
	     }
	     if ( m_outputcollections[l].find("photons")!= std::string::npos ) {
	       _string =  modularDelphes->GetString("Branch",m_outputcollections[l].c_str() ) ;
	       photonOutputArray = modularDelphes->ImportArray( _string);
	       DelphesSimulation::ConvertParticle( photonOutputArray , photons  );
	     }
	     if ( m_outputcollections[l].find("jets")!= std::string::npos ) {
	       _string =  modularDelphes->GetString("Branch",m_outputcollections[l].c_str() ) ;
	       jetOutputArray = modularDelphes->ImportArray( _string);
	       DelphesSimulation::ConvertJet( jetOutputArray , jets  );
	     }
	     if ( m_outputcollections[l].find("MissingET")!= std::string::npos ) {
	       _string =  modularDelphes->GetString("Branch",m_outputcollections[l].c_str() ) ;
	       metOutputArray = modularDelphes->ImportArray( _string);
	       DelphesSimulation::ConvertMET( metOutputArray , mets  );
	     }
	     if ( m_outputcollections[l].find("ScalarHT")!= std::string::npos ) {
	       _string =  modularDelphes->GetString("Branch",m_outputcollections[l].c_str() ) ;
	       htOutputArray = modularDelphes->ImportArray( _string);
	       DelphesSimulation::ConvertParticle( htOutputArray , hts  );
	     }
	     //} 
	     
	   }
	   treeWriter->Clear();
	   modularDelphes->Clear();
	   reader->Clear();
	   readStopWatch.Start();
	   
	 }
       
     }
   

   
   
   m_recgphandle.put(genparticles);
   m_recparthandle.put(partons);
   m_recphandle.put(particles);
   m_recmhandle.put(muons);  
   m_recehandle.put(electrons);  
   m_recphhandle.put(photons);  
   m_recjhandle.put(jets);  
   m_recmethandle.put(mets);  
   m_rechthandle.put(hts);  

   return StatusCode::SUCCESS;
}




StatusCode DelphesSimulation::finalize() {
  fseek(inputFile, 0L, SEEK_END);
  
  if(inputFile != stdin) fclose(inputFile);
  modularDelphes->FinishTask();
  
  if (m_debug_delphes) treeWriter->Write();
  
  info() << "** Exiting..." << endmsg;
  
  delete reader;
  delete modularDelphes;
  delete confReader;
  delete treeWriter;
  delete outputFile;
  
  
  return GaudiAlgorithm::finalize();
}


void DelphesSimulation::ConvertParticle(   TObjArray *  Input , ParticleCollection *  coll  ){
  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j)
    {
      cand = static_cast<Candidate *>(Input->At(j));	   
      ParticleHandle outptc = coll->create();		  
      BareParticle& core = outptc.mod().Core;
      if (cand->Momentum.Pt()!=0) { // protection against the boring message Warning in <TVector3::PseudoRapidity>: transvers momentum = 0! return +/- 10e10
	core.Type = cand->PID; 
	core.Status = cand->Status;
	core.P4.Px = (double  ) cand->Momentum.X();
	core.P4.Py = (double ) cand->Momentum.Y();
	core.P4.Pz = (double ) cand->Momentum.Z();
	core.P4.Mass = (double) cand->Mass ;
	core.Vertex.X = (double) cand->Position.X() ;
	core.Vertex.Y = (double) cand->Position.Y() ;
	core.Vertex.Z = (double) cand->Position.Z() ;	  
      }
    }
  
}   




void DelphesSimulation::ConvertJet(   TObjArray *  Input , GenJetCollection *  coll  ){
  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j)
    {
      cand = static_cast<Candidate *>(Input->At(j));
      GenJetHandle outptc = coll->create();		  
      BareJet& core = outptc.mod().Core;
      core.Area = cand->Area.Mag();
      core.P4.Px = (double  ) cand->Momentum.X();
      core.P4.Py = (double ) cand->Momentum.Y();
      core.P4.Pz = (double ) cand->Momentum.Z();
      core.P4.Mass = (double) cand->Mass ;
      
    }
  
}   




void DelphesSimulation::ConvertMET(   TObjArray *  Input , ParticleCollection *  coll  ){
  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j)
    {
      cand = static_cast<Candidate *>(Input->At(j));	   
      ParticleHandle outptc = coll->create();		  
      BareParticle& core = outptc.mod().Core;
      core.P4.Px = (double  ) cand->Momentum.X();
      core.P4.Py = (double ) cand->Momentum.Y();
      
    }
  
}   


void DelphesSimulation::ConvertHT(   TObjArray *  Input , ParticleCollection *  coll  ){
  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j)
    {
      cand = static_cast<Candidate *>(Input->At(j));	   
      ParticleHandle outptc = coll->create();		  
      BareParticle& core = outptc.mod().Core;
      core.P4.Px = (double  ) cand->Momentum.X();
      core.P4.Py = (double  ) cand->Momentum.Y();
    }
  
}   



          

          
