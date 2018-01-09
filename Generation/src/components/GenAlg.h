
#ifndef GENERATION_GENALG_H
#define GENERATION_GENALG_H

#include "Generation/IHepMCMergeTool.h"
#include "Generation/IHepMCProviderTool.h"
#include "FWCore/IPileUpTool.h"
#include "Generation/IVertexSmearingTool.h"

#include "FWCore/DataHandle.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace HepMC {
class GenEvent;
}

class GenAlg : public GaudiAlgorithm {
  friend class AlgFactory<GenAlg>;

public:
  /// Constructor.
  GenAlg(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Tools to handle input from HepMC-file
  ToolHandle<IHepMCProviderTool> m_signalProvider{"MomentumRangeParticleGun/HepMCProviderTool", this};
  ToolHandle<IHepMCProviderTool> m_pileUpProvider{"MomentumRangeParticleGun/HepMCProviderTool", this};
  ToolHandle<IPileUpTool> m_pileUpTool{"ConstPileUp/PileUpTool", this};

  /// Tool to merge HepMC events
  ToolHandle<IHepMCMergeTool> m_HepMCMergeTool{"HepMCSimpleMerge/HepMCMergeTool", this};
  // Tool to smear vertices
  ToolHandle<IVertexSmearingTool> m_vertexSmearingTool{"FlatSmearVertex/VertexSmearingTool", this};
  // output handle for finished event
  DataHandle<HepMC::GenEvent> m_hepmchandle{"hepmc", Gaudi::DataHandle::Writer, this};
};

#endif  // GENERATION_GENALG_H
