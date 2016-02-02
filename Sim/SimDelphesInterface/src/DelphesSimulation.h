#ifndef _DELPHESSIMULATION_H_
#define _DELPHESSIMULATION_H_

#include "TObjArray.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
//#include "datamodel/GenJetCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"
#include "datamodel/JetCollection.h"
#include "datamodel/JetParticleAssociationCollection.h"
#include "datamodel/METCollection.h"

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
 *                   output to Delphes-based root file or FCC-EDM-based root file (recommended!).
 *
 *  @author: Z. Drasal (CERN), M. de Gruttola (CERN)
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

private:

  // Delphes detector card to be read in
  std::string            m_DelphesCard;

  // Delphes
  Delphes               *m_Delphes ;
  DelphesFactory        *m_DelphesFactory ;
  DelphesExtHepMCReader *m_HepMCReader ;

  // Handle for the HepMC to be read in from data store
  DataHandle<HepMC::GenEvent> m_hepmcHandle;

  // Or read a HepMC file
  FILE*        m_inHepMCFile ;
  std::string  m_inHepMCFileName;
  Long64_t     m_inHepMCFileLength;

  // Handle for the generated or reconstructed objects to be written out
  DataHandle<fcc::MCParticleCollection> m_handleGenParticles;    // Generated particles handle
  DataHandle<fcc::ParticleCollection>   m_handleRecMuons;        // Reconstructed muons
  DataHandle<fcc::ParticleCollection>   m_handleRecElectrons;    // Reconstructed electrons
  //DataHandle<fcc::ParticleCollection>   m_handleRecCharged;      // Reconstructed charged hadrons
  DataHandle<fcc::ParticleCollection>   m_handleRecPhotons;      // Reconstructed photons
  DataHandle<fcc::JetCollection>        m_handleRecJets;         // Reconstructed jets
  DataHandle<fcc::METCollection>        m_handleRecMETs;         // MET

  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecMuonsToMC;     // Relation between muons & MC particle
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecElectronsToMC; // Relation between electrons & MC particle
  //DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecChargedToMC;   // Relation between charged hadrons & MC particle
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecPhotonsToMC;   // Relation between photons & MC particle
  //DataHandle<fcc::JetParticleAssociationCollection>         m_handleRecJetsToPart;    // Relation between jets & particle

  // Convert internal Delphes objects to FCC EDM objects - UPDATE
  void ConvertMCParticles(const TObjArray* Input, fcc::MCParticleCollection* colMCParticles);
  template <class T> void ConvertParticles(const TObjArray*  Input, fcc::ParticleCollection*   colParticles,
                                                                    fcc::MCParticleCollection* colMCParticles,
                                                                    fcc::ParticleMCParticleAssociationCollection* ascColParticlesToMC);
  void ConvertPhotons(const TObjArray* Input, fcc::ParticleCollection*   colParticles,
                                              fcc::MCParticleCollection* colMCParticles,
                                              fcc::ParticleMCParticleAssociationCollection* ascColParticlesToMC);
  void ConvertJets(const TObjArray* Input, fcc::JetCollection* colJets);
  void ConvertMET(const TObjArray* Input, fcc::METCollection * colMET);
  //void  ConvertHT(TObjArray * Input, ParticleCollection * coll );

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
            *m_chargedOutArray ,
            *m_photonOutArray ,
            *m_jetOutArray ,
            *m_metOutArray ,
            *m_htOutArray ;
};

#endif
