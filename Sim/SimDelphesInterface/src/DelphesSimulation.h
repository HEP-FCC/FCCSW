#ifndef _DELPHESSIMULATION_H_
#define _DELPHESSIMULATION_H_

#include "TObjArray.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
//#include "datamodel/GenJetCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"
#include "datamodel/GenJetCollection.h"
#include "datamodel/GenJetParticleAssociationCollection.h"
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
 *  Gaudi algorithm: reads-in HepMC events (from file or memory), calls Dephes interface and performs the Delphes simulation.
 *                   Data on output saved in Delphes-based root format or in FCC-EDM-based format (standard & recommended!).
 *                   The data output is defined through an outArray variables (->ImportArray method). If different objects
 *                   of the same type (e.g. muon) provided by different Delphes module needs to be saved in FCC-EDM, change
 *                   its name in the code accordingly.
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

  // Names of Delphes output arrays -> needed to correctly write out Delphes objects into FCC-EDM
  std::string m_DelphesMuonsArrayName;
  std::string m_DelphesElectronsArrayName;
  std::string m_DelphesChargedArrayName;
  std::string m_DelphesNeutralArrayName;
  std::string m_DelphesPhotonsArrayName;
  std::string m_DelphesJetsArrayName;
  std::string m_DelphesMETsArrayName;
  std::string m_DelphesSHTsArrayName;

  // Handle for the HepMC to be read in from data store
  DataHandle<HepMC::GenEvent> m_hepmcHandle;

  // Or read a HepMC file
  FILE*        m_inHepMCFile ;
  std::string  m_inHepMCFileName;
  Long64_t     m_inHepMCFileLength;

  // Handle for the generated or reconstructed objects to be written out
  DataHandle<fcc::MCParticleCollection> m_handleGenParticles;    // Generated particles handle
  DataHandle<fcc::GenVertexCollection>  m_handleGenVertices;     // Handle to vertices of generated particles
  DataHandle<fcc::ParticleCollection>   m_handleRecMuons;        // Reconstructed muons
  DataHandle<fcc::ParticleCollection>   m_handleRecElectrons;    // Reconstructed electrons
  DataHandle<fcc::ParticleCollection>   m_handleRecCharged;      // Reconstructed charged hadrons
  DataHandle<fcc::ParticleCollection>   m_handleRecNeutral;      // Reconstructed neutral hadrons
  DataHandle<fcc::ParticleCollection>   m_handleRecPhotons;      // Reconstructed photons
  DataHandle<fcc::GenJetCollection>     m_handleRecJets;         // Reconstructed jets - used GenJet class due to needed MCParticle relation
  DataHandle<fcc::METCollection>        m_handleRecMETs;         // MET

  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecMuonsToMC;     // Relation between muons & MC particle
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecElectronsToMC; // Relation between electrons & MC particle
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecChargedToMC;   // Relation between charged hadrons & MC particle
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecNeutralToMC;   // Relation between neutral hadrons & MC particle
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecPhotonsToMC;   // Relation between photons & MC particle
  DataHandle<fcc::GenJetParticleAssociationCollection>      m_handleRecJetsToMC;      // Relation between jets & MC particle

  // Convert internal Delphes objects to FCC EDM objects - UPDATE
  void ConvertMCParticles(const TObjArray* Input,    fcc::MCParticleCollection* colMCParticles,
                                                     fcc::GenVertexCollection*  colGenVertices);
  void ConvertTracks(     const TObjArray* Input,    fcc::ParticleCollection*   colParticles,
                                                     fcc::MCParticleCollection* colMCParticles,
                                                     fcc::ParticleMCParticleAssociationCollection* ascColParticlesToMC);
  void ConvertTowers(     const TObjArray* Input,    fcc::ParticleCollection*   colParticles,
                                                     fcc::MCParticleCollection* colMCParticles,
                                                     fcc::ParticleMCParticleAssociationCollection* ascColParticlesToMC);
  void ConvertJets(       const TObjArray* Input   , fcc::GenJetCollection*     colJets,
                                                     fcc::MCParticleCollection* colMCParticles,
                                                     fcc::GenJetParticleAssociationCollection* ascColJetsToMC);
  void ConvertMET(        const TObjArray* InputMET,
                          const TObjArray* InputSHT, fcc::METCollection* colMET);

  int m_eventCounter;

  // ROOT output
  TFile            *m_outRootFile;
  std::string       m_outRootFileName;
  ExRootTreeWriter *m_treeWriter;
  ExRootTreeBranch *m_branchEvent;
  ExRootConfReader *m_confReader;

  // Internal Delphes object arrays
  TObjArray *m_stablePartOutArray,
            *m_allPartOutArray,
            *m_partonOutArray,
            *m_muonOutArray,
            *m_electronOutArray,
            *m_chargedOutArray,
            *m_neutralOutArray,
            *m_photonOutArray,
            *m_jetOutArray,
            *m_metOutArray,
            *m_shtOutArray;

};

#endif
