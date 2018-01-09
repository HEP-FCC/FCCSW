#ifndef GENERATION_PYTHIAINTERFACE_H
#define GENERATION_PYTHIAINTERFACE_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiTool.h"
#include "Generation/IHepMCProviderTool.h"
#include "Generation/IVertexSmearingTool.h"
#include <memory>

// Forward HepMC
namespace HepMC {
class GenEvent;
}
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

class PythiaInterface : public GaudiTool, virtual public IHepMCProviderTool {

public:
  /// Constructor.
  PythiaInterface(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode getNextEvent(HepMC::GenEvent& theEvent);

private:
  /// Pythia8 engine
  std::unique_ptr<Pythia8::Pythia> m_pythiaSignal;
  /// Name of Pythia configuration file with Pythia simulation settings & input LHE file (if required)
  Gaudi::Property<std::string> m_parfile{this, "Filename", "Generation/data/Pythia_minbias_pp_100TeV.cmd"
                                                           "Name of the Pythia cmd file"};
  /// Pythia8 engine for jet clustering
  std::unique_ptr<Pythia8::SlowJet> m_slowJet{nullptr};
  // Tool to smear vertices
  ToolHandle<IVertexSmearingTool> m_vertexSmearingTool;
  // Output handle for ME/PS matching variables
  DataHandle<fcc::FloatCollection> m_handleMePsMatchingVars{"mePsMatchingVars", Gaudi::DataHandle::Writer, this};

  int m_nAbort{0};
  int m_iAbort{0};
  int m_iEvent{0};
  bool m_doMePsMatching{false};
  bool m_doMePsMerging{false};

  /// Pythia8 engine for ME/PS matching
  std::unique_ptr<Pythia8::JetMatchingMadgraph> m_matching{nullptr};
  /// Pythia8 engine for NLO ME/PS merging
  std::unique_ptr<Pythia8::amcnlo_unitarised_interface> m_setting{nullptr};
};

#endif  // GENERATION_PYTHIAINTERFACE_H
