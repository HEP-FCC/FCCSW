#ifndef GENERATION_PYTHIAINTERFACE_H
#define GENERATION_PYTHIAINTERFACE_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"
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
  // Name of Pythia configuration file with Pythia simulation settings & input LHE file (if required)
  Gaudi::Property<std::string> m_parfile{this, "Filename", "", "Name of the Pythia parameter file to read"};
  // Pileup Interface Tool
  ToolHandle<IPileUpTool> m_pileUpTool;
  /// Tool to merge HepMC events
  ToolHandle<IHepMCMergeTool> m_HepMCMergeTool;
  // Tool to smear vertices
  ToolHandle<IVertexSmearingTool> m_vertexSmearingTool;
  // Output handle for HepMC event
  DataHandle<HepMC::GenEvent> m_hepmchandle;

  int m_nAbort;
  int m_iAbort;
  int m_iEvent;

};

#endif // GENERATION_PYTHIAINTERFACE_H
