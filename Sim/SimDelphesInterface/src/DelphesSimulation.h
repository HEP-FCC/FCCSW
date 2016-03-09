#ifndef _DELPHESSIMULATION_H
#define _DELPHESSIMULATION_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"

// HepMC
#include "HepMC/GenEvent.h"

// STL & System
#include <iostream>
#include <sstream>
#include <memory>

// Forward Delphes
class Delphes;
class DelphesFactory;
class DelphesExtHepMCReader;
class Candidate;
class ExRootTreeBranch;
class ExRootConfReader;
class ExRootTreeWriter;

// Forward FCC EDM
namespace fcc {

  class MCParticleCollection;
  class GenVertexCollection;
  class ParticleCollection;
  class ParticleMCParticleAssociationCollection;
  class ParticleTagAssociationCollection;
  class GenJetCollection;
  class GenJetParticleAssociationCollection;
  class GenJetIntTagAssociationCollection;
  class GenJetTagAssociationCollection;
  class JetCollection;
  class JetParticleAssociationCollection;
  class JetIntTagAssociationCollection;
  class JetTagAssociationCollection;
  class IntTagCollection;
  class TagCollection;
  class METCollection;
}

// Forward ROOT
class TFile;
class TObjArray;

/** @class DelphesSimulation
 *
 *  Gaudi algorithm: reads-in the HepMC data events (from file or transient memory), calls internally a Delphes interface
 *                   and performs the Delphes simulation, i.e. calls all Delphes simulation&reconstruction modules as defined
 *                   by a user in a Delphes configuration card (Delphes card). This card defines also a detector response -
 *                   Delphes represents a parametrized simulation&reconstruction. The data on the output can be either
 *                   saved in Delphes-based root format or in a standard FCC-EDM-based format (recommended!). The FCC data
 *                   particle content is defined through an outArray variables (->ImportArray method). If different objects
 *                   of the same type (e.g. electrons before or after isolation) need to be saved, change the name of
 *                   outArray accordingly in a Gaudi configuration file: PythiaDelphes_config.py .
 *
 *  @author: Z. Drasal (CERN), M. de Gruttola (CERN)
 *
 */
class DelphesSimulation: public GaudiAlgorithm {

  friend class AlgFactory<DelphesSimulation> ;

public:

  //! Constructor.
  DelphesSimulation(const std::string& name, ISvcLocator* svcLoc);

  //! Initialize.
  virtual StatusCode initialize();

  //! Execute. This function actually does no simulation,
  //! and simply converts the stable MCParticles in the input collection
  //! into Particles that are written to the output collection.
  virtual StatusCode execute();

  //! Finalize.
  virtual StatusCode finalize();

private:

  // Delphes detector card to be read in
  std::string            m_DelphesCard; //!< Name of Delphes tcl config file with detector and simulation parameters

  // Delphes
  std::unique_ptr<Delphes>               m_Delphes;
  std::unique_ptr<DelphesExtHepMCReader> m_HepMCReader;

  // Names of Delphes output arrays -> needed to correctly write out the Delphes objects into FCC-EDM
  std::string m_DelphesMuonsArrayName;      //!< Delphes muon output array: moduleName/muons
  std::string m_DelphesElectronsArrayName;  //!< Delphes electron output array: moduleName/electrons
  std::string m_DelphesChargedArrayName;    //!< Delphes charged hadron output array: moduleName/chargedHadrons
  std::string m_DelphesNeutralArrayName;    //!< Delphes neutral hadron output array: moduleName/eflowNeutralHadrons
  std::string m_DelphesPhotonsArrayName;    //!< Delphes photons output array: moduleName/photons
  std::string m_DelphesGenJetsArrayName;    //!< Delphes genJets array: moduleName/genJets
  std::string m_DelphesJetsArrayName;       //!< Delphes jets array: moduleName/jets
  std::string m_DelphesMETsArrayName;       //!< Delphes Missing ET output array: moduleName/momentum
  std::string m_DelphesSHTsArrayName;       //!< Delphes Scalar pT sum output array: moduleName/energy

  // Handle for the HepMC to be read in from the data store
  DataHandle<HepMC::GenEvent> m_hepmcHandle;

  // Or read a HepMC file
  FILE*        m_inHepMCFile ;
  std::string  m_inHepMCFileName;   //!< Name of HepMC input file, if defined file read in / if not data read in directly from the transient data store
  long long    m_inHepMCFileLength;

  // Handle for the generated or reconstructed objects to be written out
  DataHandle<fcc::MCParticleCollection> m_handleGenParticles;    //!< Generated particles handle
  DataHandle<fcc::GenVertexCollection>  m_handleGenVertices;     //!< Handle to vertices of generated particles
  DataHandle<fcc::GenJetCollection>     m_handleGenJets;         //!< Handle to generated jets
  DataHandle<fcc::IntTagCollection>     m_handleGenJetsFlavor;   //!< Flavour tag of generated jets (PDG of leading constituent)
  DataHandle<fcc::ParticleCollection>   m_handleRecMuons;        //!< Reconstructed muons
  DataHandle<fcc::TagCollection>        m_handleRecITagMuons;    //!< Reconstructed isolation tag of muons
  DataHandle<fcc::ParticleCollection>   m_handleRecElectrons;    //!< Reconstructed electrons
  DataHandle<fcc::TagCollection>        m_handleRecITagElectrons;//!< Reconstructed isolation tag of electrons
  DataHandle<fcc::ParticleCollection>   m_handleRecCharged;      //!< Reconstructed charged hadrons
  DataHandle<fcc::ParticleCollection>   m_handleRecNeutral;      //!< Reconstructed neutral hadrons
  DataHandle<fcc::ParticleCollection>   m_handleRecPhotons;      //!< Reconstructed photons
  DataHandle<fcc::TagCollection>        m_handleRecITagPhotons;  //!< Reconstructed isolation tag of photons
  DataHandle<fcc::JetCollection>        m_handleRecJets;         //!< Reconstructed jets
  DataHandle<fcc::ParticleCollection>   m_handleRecJetParts;     //!< Constituents of reconstructed jets
  DataHandle<fcc::IntTagCollection>     m_handleRecJetsFlavor;   //!< Flavour tag of reconstructed jets (PDG of leading constituent)
  DataHandle<fcc::TagCollection>        m_handleRecBTags;        //!< Reconstructed b tag information - used Jet to tag relation
  DataHandle<fcc::TagCollection>        m_handleRecCTags;        //!< Reconstructed c tag information - used Jet to tag relation
  DataHandle<fcc::TagCollection>        m_handleRecTauTags;      //!< Reconstructed tau tag information - used Jet to tag relation
  DataHandle<fcc::METCollection>        m_handleRecMETs;         //!< MET

  DataHandle<fcc::GenJetParticleAssociationCollection>      m_handleGenJetsToMC;        //!< Relation between generated jets & MC particle
  DataHandle<fcc::GenJetIntTagAssociationCollection>        m_handleGenJetsToFlavor;    //!< Relation between generated jets & flavour tags (PDG of leading constituent)
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecMuonsToMC;       //!< Relation between muons & MC particle
  DataHandle<fcc::ParticleTagAssociationCollection>         m_handleRecMuonsToITags;    //!< Relation between muons & isolation tag
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecElectronsToMC;   //!< Relation between electrons & MC particle
  DataHandle<fcc::ParticleTagAssociationCollection>         m_handleRecElectronsToITags;//!< Relation between electrons & isolation tag
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecChargedToMC;     //!< Relation between charged hadrons & MC particle
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecNeutralToMC;     //!< Relation between neutral hadrons & MC particle
  DataHandle<fcc::ParticleMCParticleAssociationCollection>  m_handleRecPhotonsToMC;     //!< Relation between photons & MC particle
  DataHandle<fcc::ParticleTagAssociationCollection>         m_handleRecPhotonsToITags;  //!< Relation between photons & isolation tag
  DataHandle<fcc::JetParticleAssociationCollection>         m_handleRecJetsToParts;     //!< Relation between jets & particle constituents
  DataHandle<fcc::JetIntTagAssociationCollection>           m_handleRecJetsToFlavor;    //!< Relation between jets & flavour tags (PDG of leading constituent)
  DataHandle<fcc::JetTagAssociationCollection>              m_handleRecJetsToBTags;     //!< Relation between jets & b tag info
  DataHandle<fcc::JetTagAssociationCollection>              m_handleRecJetsToCTags;     //!< Relation between jets & c tag info
  DataHandle<fcc::JetTagAssociationCollection>              m_handleRecJetsToTauTags;   //!< Relation between jets & tau tag info

  //! Convert internal Delphes objects: MCParticles to FCC EDM: MCParticle & GenVertices
  void ConvertMCParticles(const TObjArray* Input,
                          fcc::MCParticleCollection* colMCParticles,
                          fcc::GenVertexCollection* colGenVertices);

  //! Convert internal Delphes objects: GenJets to FCC EDM: GenJets & GenJets<->MCParticles association & GenJets<->PDG of leading constituent
  void ConvertGenJets(const TObjArray* Input,
                      const fcc::MCParticleCollection* colMCParticles,
                      fcc::GenJetCollection* colJets,
                      fcc::GenJetParticleAssociationCollection* ascColJetsToMC,
                      fcc::IntTagCollection* colJetsFlavor,
                      fcc::GenJetIntTagAssociationCollection* ascColJetsToFlavor);

  //! Convert internal Delphes objects: Muons, electrons, charged hadrons to FCC EDM: Particles & Particles<->MCParticles association
  //! If isolation tag defined, add also isolation tag and Particles<->Isolation tag association
  void ConvertTracks(const TObjArray* Input,
                     const fcc::MCParticleCollection* colMCParticles,
                     fcc::ParticleCollection* colParticles,
                     fcc::ParticleMCParticleAssociationCollection* ascColParticlesToMC,
                     fcc::TagCollection* colITags,
                     fcc::ParticleTagAssociationCollection* ascColParticlesToITags);


  //! Convert internal Delphes objects: Photons, neutral hadrons to FCC EDM: Particles & Particles<->MCParticles association
  //! If isolation tag defined, add also isolation tag and Particles<->Isolation tag association
  void ConvertTowers(const TObjArray* Input,
                     const fcc::MCParticleCollection* colMCParticles,
                     fcc::ParticleCollection* colParticles,
                     fcc::ParticleMCParticleAssociationCollection* ascColParticlesToMC,
                     fcc::TagCollection* colITags,
                     fcc::ParticleTagAssociationCollection* ascColParticlesToITags);

  //! Convert internal Delphes objects: Jets to FCC EDM: Jets & Jets<->Jets constituents association
  //! Add PDG of leading constituent & corresponding relation
  //! Add b-tagging c-tagging and/or tau-tagging information
  void ConvertJets(const TObjArray* Input,
                   fcc::JetCollection* colJets,
                   fcc::ParticleCollection* colJetParts,
                   fcc::JetParticleAssociationCollection* ascColJetsToPart,
                   fcc::IntTagCollection* colJetsFlavor,
                   fcc::JetIntTagAssociationCollection* ascColJetsToFlavor,
                   fcc::TagCollection* colBTags,
                   fcc::JetTagAssociationCollection* ascColJetsToBTags,
                   fcc::TagCollection* colCTags,
                   fcc::JetTagAssociationCollection* ascColJetsToCTags,
                   fcc::TagCollection* colTauTags,
                   fcc::JetTagAssociationCollection* ascColJetsToTauTags);

  //! Recursive method to find an id of MCParticle related to the given Delphes Candidate object,
  //! if MC particle found (id>=0), its index is then saved to idRefMCPart set,
  //! if relation doesn't exist (id<0), warning is given on output and search for other relations continues
  void findJetPartMC(Candidate* cand, int rangeMCPart, std::set<int>& idRefMCPart);

  //! Convert internal Delphes objects: Missing ETs and scalar pT sums to FCC EDM: METs
  void ConvertMET(const TObjArray* InputMET,
                  const TObjArray* InputSHT,
                  fcc::METCollection* colMET);

  int m_eventCounter;

  // Delphes ROOT output
  TFile*                            m_outRootFile;
  std::string                       m_outRootFileName; //!< Name of Delphes Root output file, if defined, the Delphes standard tree write out in addition to FCC output
  ExRootTreeWriter*                 m_treeWriter;
  ExRootTreeBranch*                 m_branchEvent;
  std::unique_ptr<ExRootConfReader> m_confReader;

  // Internal Delphes object arrays
  TObjArray *m_stablePartOutArray,
            *m_allPartOutArray,
            *m_partonOutArray,
            *m_genJetOutArray,
            *m_muonOutArray,
            *m_electronOutArray,
            *m_chargedOutArray,
            *m_neutralOutArray,
            *m_photonOutArray,
            *m_jetOutArray,
            *m_metOutArray,
            *m_shtOutArray;

};

#endif // #define _DELPHESSIMULATION_H
