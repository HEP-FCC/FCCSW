#ifndef GENERATION_PYTHIAINTERFACE_H
#define GENERATION_PYTHIAINTERFACE_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "Generation/IHepMCMergeTool.h"
#include "Generation/IPileUpTool.h"
#include "Generation/IVertexSmearingTool.h"
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

class PythiaInterface : public GaudiAlgorithm {
  friend class AlgFactory<PythiaInterface>;

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
  std::unique_ptr<Pythia8::Pythia> m_pythiaSignal{nullptr};
  // Pythia8 engine for pileup events
  std::unique_ptr<Pythia8::Pythia> m_pythiaPileup{nullptr};
  // Name of Pythia configuration file with Pythia simulation settings & input LHE file (if required)
  Gaudi::Property<std::string> m_parfile{this, "Filename", "", "Name of the Pythia parameter file to read"};
  // Pythia8 engine for ME/PS matching
  std::unique_ptr<Pythia8::JetMatchingMadgraph> m_matching{nullptr};
  // Pythia8 engine for NLO ME/PS merging
  std::unique_ptr<Pythia8::amcnlo_unitarised_interface> m_setting{nullptr};
  // Pythia8 engine for jet clustering
  std::unique_ptr<Pythia8::SlowJet> m_slowJet{nullptr};
  // Pileup Interface Tool
  ToolHandle<IPileUpTool> m_pileUpTool{"ConstPileUp/PileUpTool", this};
  // Output handle for HepMC event
  DataHandle<HepMC::GenEvent> m_hepmchandle{"HepMC", Gaudi::DataHandle::Reader, this};
  // Output handle for ME/PS matching variables
  DataHandle<fcc::FloatCollection> m_handleMePsMatchingVars{"mePsMatchingVars", Gaudi::DataHandle::Writer, this};

  int m_nAbort{0};
  int m_iAbort{0};
  int m_iEvent{0};
  bool m_doMePsMatching{false};
  bool m_doMePsMerging{false};
};

#endif  // GENERATION_PYTHIAINTERFACE_H
