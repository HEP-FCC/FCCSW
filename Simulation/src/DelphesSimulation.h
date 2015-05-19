#ifndef _DELPHESSIMULATION_H_
#define _DELPHESSIMULATION_H_

#include "TObjArray.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/DataObjectHandle.h"
#include "DataObjects/ParticleCollection.h"
#include "DataObjects/GenJetCollection.h"
#include "DataObjects/MissingEnergyCollection.h"
#include "DataObjects/MCParticleCollection.h"
#include "TFile.h"
#include "ExRootAnalysis/ExRootTreeWriter.h"


// for Delphes

// ROOT
#include "TLorentzVector.h"
#include "TBranch.h"
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


class DelphesSimulation: public GaudiAlgorithm {
  friend class AlgFactory<DelphesSimulation> ;

public:
  /// Constructor.
  DelphesSimulation(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute. This function actually does no simulation,
  /// and simply converts the stable MCParticles in the input collection
  /// into Particles that are written to the output collection. 
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

 void  ConvertParticle(TObjArray *  Input, ParticleCollection * coll ); 
 void  ConvertJet(TObjArray *  Input, GenJetCollection * coll ); 
 void  ConvertMET(TObjArray *  Input, ParticleCollection * coll ); 
 void  ConvertHT(TObjArray *  Input, ParticleCollection * coll ); 



private:



  // detector card to be read
  std::string m_filename;
  std::string m_detectorcard;
  std::vector<std::string> m_outputcollections;

  // for delphes initialization
  FILE *inputFile ;
  TFile *outputFile ; 
  ExRootTreeWriter *treeWriter ; 
  ExRootTreeBranch *branchEvent ;
  ExRootConfReader *confReader ;
  Delphes *modularDelphes ;
  DelphesFactory *factory ;

  TObjArray *stableParticleOutputArray,  *allParticleOutputArray , *partonOutputArray , *muonOutputArray ,  *electronOutputArray , *photonOutputArray , *jetOutputArray ,  *metOutputArray ,  *htOutputArray ;
  
  DelphesHepMCReader *reader ;
  TStopwatch readStopWatch, procStopWatch;
  
  Long64_t length, eventCounter; // fixme 
  ExRootProgressBar *progressBar;

  bool  m_debug_delphes; // if true one run the standalone delphes as well 

  /// Handle for the MCParticleCollection to be read
  //DataObjectHandle<MCParticleCollection> m_genphandle;
  /// Handle for the "reconstructed" to be written
  DataObjectHandle<ParticleCollection> m_recgphandle; // genparticles particles
  DataObjectHandle<ParticleCollection> m_recparthandle; // partons
  DataObjectHandle<ParticleCollection> m_recphandle; // stable particles
  DataObjectHandle<ParticleCollection> m_recmhandle; // muons
  DataObjectHandle<ParticleCollection> m_recehandle; // electrons
  DataObjectHandle<ParticleCollection> m_recphhandle; // photons
  DataObjectHandle<GenJetCollection> m_recjhandle; // jets
  DataObjectHandle<ParticleCollection> m_recmethandle; // met
  DataObjectHandle<ParticleCollection> m_rechthandle; // ht
};

#endif
