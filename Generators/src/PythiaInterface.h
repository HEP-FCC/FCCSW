#ifndef _PYTHIAINTERFACE_H_
#define _PYTHIAINTERFACE_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Pythia8/Pythia.h"
#include "Pythia8/Pythia8ToHepMC.h"

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
  /// the name of the Pythia parameter input file
  std::string       m_parfile;
  /// The output handle for what is being produced
  DataHandle<HepMC::GenEvent> m_hepmchandle;
  int nAbort;
  int iAbort;

};

#endif
