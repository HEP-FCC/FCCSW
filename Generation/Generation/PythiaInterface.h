#ifndef _PYTHIAINTERFACE_H_
#define _PYTHIAINTERFACE_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Pythia8/Pythia.h"
// Pythia version 8.1
#include "Pythia8/Pythia8ToHepMC.h"
// Pythia version 8.2
//#include "Pythia8Plugins/HepMC2.h"

#include "HepMC/GenEvent.h"
#include "FWCore/DataHandle.h"

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
