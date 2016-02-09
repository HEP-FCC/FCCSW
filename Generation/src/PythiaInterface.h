#ifndef _PYTHIAINTERFACE_H_
#define _PYTHIAINTERFACE_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"

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
  Pythia8::Pythia * m_pythia;

  // Name of Pythia configuration input file
  std::string       m_parfile;

  // Output handle for HepMC event
  DataHandle<HepMC::GenEvent> m_hepmchandle;

  int m_nAbort;
  int m_iAbort;
  int m_iEvent;

};

#endif
