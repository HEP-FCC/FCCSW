#ifndef GENERATION_PYTHIAINTERFACE_H
#define GENERATION_PYTHIAINTERFACE_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Generation/IPileUpTool.h"
#include "Generation/IVertexSmearingTool.h"
#include "Generation/IHepMCMergeTool.h"
#include "FWCore/DataHandle.h"
#include <memory>

// HepMC
#include "HepMC/GenEvent.h"

// Forward Pythia
namespace Pythia8 {
  class Pythia;
  class SlowJet;
  class JetMatchingMadgraph;
  class amcnlo_unitarised_interface;
}

// Forward FCC EDM
namespace fcc {
  class FloatCollection;
}

class PythiaInterface: public GaudiAlgorithm {
  friend class AlgFactory<PythiaInterface> ;

public:
  /// Constructor.
  PythiaInterface(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:

  // Pythia8 engine
  std::unique_ptr<Pythia8::Pythia> m_pythiaSignal;
  // Pythia8 engine for pileup events
  std::unique_ptr<Pythia8::Pythia> m_pythiaPileup;
  // Pythia8 engine for ME/PS matching
  std::unique_ptr<Pythia8::JetMatchingMadgraph> m_matching;
  // Pythia8 engine for NLO ME/PS merging
  std::unique_ptr<Pythia8::amcnlo_unitarised_interface> m_setting;
  // Pythia8 engine for jet clustering
  std::unique_ptr<Pythia8::SlowJet> m_slowJet;
  // Name of Pythia configuration input file
  std::string       m_parfile; //!< Name of Pythia configuration file with Pythia simulation settings & input LHE file (if required)
  // Pileup Interface Tool
  ToolHandle<IPileUpTool> m_pileUpTool;
  /// Tool to merge HepMC events
  ToolHandle<IHepMCMergeTool> m_HepMCMergeTool;
  // Tool to smear vertices
  ToolHandle<IVertexSmearingTool> m_vertexSmearingTool;
  // Output handle for HepMC event
  DataHandle<HepMC::GenEvent> m_hepmchandle;
  // Output handle for ME/PS matching variables
  DataHandle<fcc::FloatCollection> m_handleMePsMatchingVars;
  
  int m_nAbort;
  int m_iAbort;
  int m_iEvent;
  bool m_doMePsMatching;
  bool m_doMePsMerging;
};

#endif // GENERATION_PYTHIAINTERFACE_H
