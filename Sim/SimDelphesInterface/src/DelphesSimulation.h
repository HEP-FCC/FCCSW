#ifndef _DELPHESSIMULATION_H
#define _DELPHESSIMULATION_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"
#include "SimDelphesInterface/IDelphesSaveOutputTool.h"
// HepMC
#include "HepMC/GenEvent.h"

// STL & System
#include <iostream>
#include <sstream>
#include <memory>

// Forward Delphes
class Delphes;
class DelphesFactory;
class HepMCDelphesConverter;
class Candidate;
class ExRootTreeBranch;
class ExRootConfReader;
class ExRootTreeWriter;

// Forward FCC EDM
namespace fcc {
  class MCParticleCollection;
  class GenVertexCollection;
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
  std::unique_ptr<HepMCDelphesConverter> m_hepMCConverter;

  // Handle for the HepMC to be read in from the data store
  DataHandle<HepMC::GenEvent> m_hepmcHandle;

  // Handle for the generated or reconstructed objects to be written out
  DataHandle<fcc::MCParticleCollection> m_handleGenParticles;    //!< Generated particles handle
  DataHandle<fcc::GenVertexCollection>  m_handleGenVertices;     //!< Handle to vertices of generated particles

  //! Recursive method to find an id of MCParticle related to the given Delphes Candidate object,
  //! if MC particle found (id>=0), its index is then saved to idRefMCPart set,
  //! if relation doesn't exist (id<0), warning is given on output and search for other relations continues
  void findJetPartMC(Candidate* cand, int rangeMCPart, std::set<int>& idRefMCPart);

  void ConvertMCParticles(const TObjArray* Input,
                          fcc::MCParticleCollection* colMCParticles,
                          fcc::GenVertexCollection* colGenVertices);
  int m_eventCounter;

  // Delphes ROOT output
  TFile*                            m_outRootFile;
  std::string                       m_outRootFileName; //!< Name of Delphes Root output file, if defined, the Delphes standard tree write out in addition to FCC output
  ExRootTreeWriter*                 m_treeWriter;
  ExRootTreeBranch*                 m_branchEvent;
  std::unique_ptr<ExRootConfReader> m_confReader;
  std::vector<std::string> m_saveToolNames;
  std::vector<IDelphesSaveOutputTool*> m_saveTools;

  // Arrays used by Delphes and internally for initial particles
  TObjArray* m_allParticles;
  TObjArray* m_stableParticles;
  TObjArray* m_partons;
};

#endif // #define _DELPHESSIMULATION_H
