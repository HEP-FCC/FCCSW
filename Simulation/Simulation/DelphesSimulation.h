#ifndef _DELPHESSIMULATION_H_
#define _DELPHESSIMULATION_H_

#include "TObjArray.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/GenJetCollection.h"
#include "datamodel/MissingEnergyCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "TFile.h"
#include "FWCore/DataHandle.h"
#include "HepMC/GenEvent.h"

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
#include "DelphesExtHepMCReader.h"
#include "ExRootAnalysis/ExRootConfReader.h"
#include "ExRootAnalysis/ExRootTask.h"
#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "ExRootAnalysis/ExRootTreeBranch.h"

#include "modules/ParticlePropagator.h"

// STL
#include <iostream>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <signal.h>

/** @class DelphesSimulation
 *
 *  Gaudi algorithm: reads-in HepMC event (from file or memory), calls Dephes and performs Delphes simulation,
 *                   output to root file (output to memory still missing).
 *
 *  @author: Z. Drasal (CERN) & M. de Gruttola (CERN)
 *
 */
class DelphesSimulation: public GaudiAlgorithm {
  friend class AlgFactory<DelphesSimulation> ;

public:

  // Constructor.
  DelphesSimulation(const std::string& name, ISvcLocator* svcLoc);

  // Initialize.
  virtual StatusCode initialize();

  // Execute. This function actually does no simulation,
  // and simply converts the stable MCParticles in the input collection
  // into Particles that are written to the output collection.
  virtual StatusCode execute();

  // Finalize.
  virtual StatusCode finalize();

  // Convert internal Delphes objects to FCC EDM objects - UPDATE
  void  ConvertParticle(TObjArray *  Input, ParticleCollection * coll );
  void  ConvertJet(     TObjArray *  Input, GenJetCollection   * coll );
  void  ConvertMET(     TObjArray *  Input, ParticleCollection * coll );
  void  ConvertHT(      TObjArray *  Input, ParticleCollection * coll );

private:

  // Delphes detector card to be read in
  std::string              m_DelphesCard;

  // Output collections to be write out
  std::vector<std::string> m_outCollections;

  // Delphes
  Delphes               *m_Delphes ;
  DelphesFactory        *m_DelphesFactory ;
  DelphesExtHepMCReader *m_HepMCReader ;

  //Long64_t length, eventCounter; // fixme

  // Handle for the HepMC to be read in from data store
  DataHandle<HepMC::GenEvent> m_hepmcHandle;

  // Or read a HepMC file
  FILE        *m_inHepMCFile ;
  std::string  m_inHepMCFileName;
  Long64_t     m_inHepMCFileLength;

  // Handle for the "reconstructed" objects to be written out
  DataHandle<ParticleCollection> m_allPartclsHandle; // All particles
  DataHandle<ParticleCollection> m_genPartonsHandle; // Partons
  DataHandle<ParticleCollection> m_genStablesHandle; // Stable particles
  DataHandle<ParticleCollection> m_recMuonHandle;    // Muons
  DataHandle<ParticleCollection> m_recElecHandle;    // Electrons
  DataHandle<ParticleCollection> m_recPhotHandle;    // Photons
  DataHandle<GenJetCollection>   m_recJetsHandle;    // Jets
  DataHandle<ParticleCollection> m_recMETHandle;     // MET
  DataHandle<ParticleCollection> m_recHTSHandle;     // Scalar HT

  int m_eventCounter;

  // ROOT output
  TFile            *m_outRootFile;
  std::string       m_outRootFileName;
  ExRootTreeWriter *m_treeWriter;
  ExRootTreeBranch *m_branchEvent;
  ExRootConfReader *m_confReader;

  // Internal Delphes object arrays
  TObjArray *m_stablePartOutArray,
            *m_allPartOutArray ,
            *m_partonOutArray ,
            *m_muonOutArray ,
            *m_electronOutArray ,
            *m_photonOutArray ,
            *m_jetOutArray ,
            *m_metOutArray ,
            *m_htOutArray ;

};

#endif
