#ifndef GENERATION_PYTHIAINTERFACE_H
#define GENERATION_PYTHIAINTERFACE_H

#include <memory>
#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiTool.h"
#include "Generation/IHepMCProviderTool.h"
#include "Generation/IVertexSmearingTool.h"
#include "ResonanceDecayFilterHook.h"
#include "Pythia8Plugins/PowhegHooks.h"
#include "Pythia8Plugins/HepMC2.h"

class EvtGenDecays;

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

namespace fcc {
  class FloatValueCollection;
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
  /// Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC m_pythiaToHepMC;
  /// Name of Pythia configuration file with Pythia simulation settings & input LHE file (if required)
  Gaudi::Property<std::string> m_parfile{this, "Filename", "Generation/data/Pythia_minbias_pp_100TeV.cmd"
                                                           "Name of the Pythia cmd file"};
  /// Pythia8 engine for jet clustering
  std::unique_ptr<Pythia8::SlowJet> m_slowJet{nullptr};
  // Tool to smear vertices
  ToolHandle<IVertexSmearingTool> m_vertexSmearingTool;
  // Output handle for ME/PS matching variables
  DataHandle<fcc::FloatValueCollection> m_handleMePsMatchingVars{"mePsMatchingVars", Gaudi::DataHandle::Writer, this};

  int m_nAbort{0};
  int m_iAbort{0};
  int m_iEvent{0};

  // -- aMCatNLO
  bool m_doMePsMatching{false};
  bool m_doMePsMerging{false};
  /// Pythia8 engine for ME/PS matching
  std::unique_ptr<Pythia8::JetMatchingMadgraph> m_matching{nullptr};
  /// Pythia8 engine for NLO ME/PS merging
  std::unique_ptr<Pythia8::amcnlo_unitarised_interface> m_setting{nullptr};

  // Powheg
  bool m_doPowheg{false};
  unsigned long int m_nISRveto{0};
  unsigned long int m_nFSRveto{0};    
  /// Pythia8 engine for Powheg ME/PS merging
  std::shared_ptr<Pythia8::PowhegHooks> m_powhegHooks{nullptr};

  std::shared_ptr<ResonanceDecayFilterHook> m_resonanceDecayFilterHook{nullptr};

  /// flag for additional printouts
  Gaudi::Property<bool> m_printPythiaStatistics{this, "printPythiaStatistics", false,
                                                           "Print Pythia Statistics"};

  Gaudi::Property<bool> m_doEvtGenDecays{this, "doEvtGenDecays", false,
                                                           "Do decays with EvtGen"};
  Gaudi::Property<std::string> m_EvtGenDecayFile{this, "EvtGenDecayFile", "Generation/data/EVTGEN.DEC",
                                                           "Name of the global EvtGen Decay File"};
  Gaudi::Property<std::string> m_UserDecayFile{this, "UserDecayFile", "",
                                                           "Name of the  EvtGen User Decay File"};
  Gaudi::Property<std::string> m_EvtGenParticleDataFile{this, "EvtGenParticleDataFile", "Generation/data/evt.pdl",
                                                           "Name of the EvtGen Particle Data File"};

  Gaudi::Property<std::vector<int>> m_evtGenExcludes{this, "EvtGenExcludes", false,
                                                           "Pdg IDs of particles not to decay with EvtGen"};
  EvtGenDecays* m_evtgen = nullptr;
};

#endif  // GENERATION_PYTHIAINTERFACE_H
