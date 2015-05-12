#include "DelphesSimulation.h"
// for Delphes

// ROOT
#include "TLorentzVector.h"
#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"


#include "TApplication.h"


#include "TStopwatch.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "Math/GenVector/LorentzVector.h"

#include "classes/DelphesModule.h"

//#include "SortableObject.h"
#include "classes/DelphesClasses.h"
#include "modules/Delphes.h"


#include "classes/DelphesFactory.h"
#include "classes/DelphesHepMCReader.h"
#include "ExRootAnalysis/ExRootConfReader.h"
#include "ExRootAnalysis/ExRootTreeWriter.h"

#include "ExRootAnalysis/ExRootTask.h"
#include "ExRootAnalysis/ExRootTreeBranch.h"
#include "ExRootAnalysis/ExRootProgressBar.h"

#include "modules/ParticlePropagator.h"
// STL
#include <iostream>
#include <vector>

#include <stdexcept>
#include <sstream>

#include <signal.h>

static bool interrupted = false;
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
  return StatusCode::SUCCESS;
}

StatusCode DelphesSimulation::execute() {
  // delphes stuff here
  ExRootTreeWriter *treeWriter = 0; 
  ExRootTreeBranch *branchEvent = 0;
  ExRootConfReader *confReader = 0;
  Delphes *modularDelphes = 0;
  DelphesFactory *factory = 0;

  TObjArray *stableParticleOutputArray = 0, *allParticleOutputArray = 0, *partonOutputArray = 0, *muonOutputArray =0,  *electronOutputArray =0, *photonOutputArray =0, *jetOutputArray =0,  *metOutputArray =0,  *htOutputArray =0;
  
  DelphesHepMCReader *reader = 0;
  Int_t  maxEvents, skipEvents;
  Long64_t length, eventCounter;
  FILE *inputFile = 0;
  // FILE *detectorFile = 0;
  TFile *outputFile = 0; 
  stringstream message;
  TStopwatch readStopWatch, procStopWatch;
  
  
  
  // the input files is an HEPMC file
  cout << "** Reading " << m_filename << endl;
  inputFile = fopen(m_filename.c_str(), "r");
  
  if(inputFile == NULL)
    {
      message << "can't open " << m_filename;
      throw runtime_error(message.str());
    }
  
  fseek(inputFile, 0L, SEEK_END);     
  length = ftello(inputFile);
  fseek(inputFile, 0L, SEEK_SET);
  cout << "** length of input file " << length << endl;
  if(length <= 0)
    {
      fclose(inputFile);
    }
  // now read delphes card
  confReader = new ExRootConfReader;
  confReader->ReadFile(m_detectorcard.c_str());
  maxEvents = confReader->GetInt("::MaxEvents", 0);
  skipEvents = confReader->GetInt("::SkipEvents", 0);
  if(maxEvents < 0)
    {
      throw runtime_error("MaxEvents must be zero or positive");
    }
  if(skipEvents < 0)
    {
      throw runtime_error("SkipEvents must be zero or positive");
    }
  
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
 // now access delphes recontructed particles
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
	std::cout << "delphes modules with name " <<  name << std::endl;
      } 

  ExRootProgressBar progressBar(length);


  // Loop over all objects                                                                              
  eventCounter = 0;
  treeWriter->Clear();
  modularDelphes->Clear();
  reader->Clear();
  

  readStopWatch.Start();
  
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

 
  while((maxEvents <= 0 || eventCounter - skipEvents < maxEvents) &&
	reader->ReadBlock(factory, allParticleOutputArray,
  			  stableParticleOutputArray, partonOutputArray ) && !interrupted)
    {
      if(reader->EventReady())
        {
          ++eventCounter;
	
          readStopWatch.Stop();
	  
          if(eventCounter > skipEvents)
	    {
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
	      } 
	      treeWriter->Clear();
	    }
	  
	  modularDelphes->Clear();
	  reader->Clear();
	  readStopWatch.Start();
	}
      progressBar.Update(ftello(inputFile), eventCounter);
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
  
  fseek(inputFile, 0L, SEEK_END);
  progressBar.Update(ftello(inputFile), eventCounter, kTRUE);
  progressBar.Finish();
  
  if(inputFile != stdin) fclose(inputFile);
  modularDelphes->FinishTask();

  if (m_debug_delphes) treeWriter->Write();
  
  cout << "** Exiting..." << endl;
  delete reader;
  delete modularDelphes;
  delete confReader;
  delete treeWriter;
  delete outputFile;

  return StatusCode::SUCCESS;
}




StatusCode DelphesSimulation::finalize() {
  return GaudiAlgorithm::finalize();
}


void DelphesSimulation::ConvertParticle(   TObjArray *  Input , ParticleCollection *  coll  ){
  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j)
    {
      cand = static_cast<Candidate *>(Input->At(j));	   
      ParticleHandle& outptc = coll->create();		  
      BareParticle& core = outptc.mod().Core;
      if (cand->Momentum.Pt()!=0) { // protection against the boring message Warning in <TVector3::PseudoRapidity>: transvers momentum = 0! return +/- 10e10
      core.Type = cand->PID; 
      core.Status = cand->Status;
      core.P4.Pt = (double  ) cand->Momentum.Pt();
      core.P4.Eta = (double ) cand->Momentum.Eta();
      core.P4.Phi = (double ) cand->Momentum.Phi();
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
      GenJetHandle& outptc = coll->create();		  
      BareJet& core = outptc.mod().Core;
      core.Area = cand->Area.Mag();
      core.P4.Pt = (double  ) cand->Momentum.Pt();
      core.P4.Eta = (double ) cand->Momentum.Eta();
      core.P4.Phi = (double ) cand->Momentum.Phi();
      core.P4.Mass = (double) cand->Mass ;
      
    }
  
}   




void DelphesSimulation::ConvertMET(   TObjArray *  Input , ParticleCollection *  coll  ){
  Candidate * cand;
  for(int j = 0; j < Input->GetEntries(); ++j)
    {
      cand = static_cast<Candidate *>(Input->At(j));	   
      ParticleHandle& outptc = coll->create();		  
      BareParticle& core = outptc.mod().Core;
      core.P4.Pt = (double  ) cand->Momentum.Pt();
      core.P4.Phi = (double ) cand->Momentum.Phi();
      
    }
  
}   


void DelphesSimulation::ConvertHT(   TObjArray *  Input , ParticleCollection *  coll  ){
  Candidate * cand;
        for(int j = 0; j < Input->GetEntries(); ++j)
	  {
	    cand = static_cast<Candidate *>(Input->At(j));	   
	    ParticleHandle& outptc = coll->create();		  
	    BareParticle& core = outptc.mod().Core;
	    core.P4.Pt = (double  ) cand->Momentum.Pt();
	  }

}   



          

          
