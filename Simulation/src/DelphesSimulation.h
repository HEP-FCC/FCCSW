#ifndef _DELPHESSIMULATION_H_
#define _DELPHESSIMULATION_H_

#include "TObjArray.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/DataObjectHandle.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/GenJetCollection.h"
#include "datamodel/MissingEnergyCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "TFile.h"
#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "FWCore/DataHandle.h"

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
  bool  m_debug_delphes; // if true one run the standalone delphes as well 

  /// Handle for the MCParticleCollection to be read
  //DataObjectHandle<MCParticleCollection> m_genphandle;
  /// Handle for the "reconstructed" to be written
  DataHandle<ParticleCollection> m_recgphandle; // genparticles particles
  DataHandle<ParticleCollection> m_recparthandle; // partons
  DataHandle<ParticleCollection> m_recphandle; // stable particles
  DataHandle<ParticleCollection> m_recmhandle; // muons
  DataHandle<ParticleCollection> m_recehandle; // electrons
  DataHandle<ParticleCollection> m_recphhandle; // photons
  DataHandle<GenJetCollection> m_recjhandle; // jets
  DataHandle<ParticleCollection> m_recmethandle; // met
  DataHandle<ParticleCollection> m_rechthandle; // ht
};

#endif
