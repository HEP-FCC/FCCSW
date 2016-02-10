#ifndef GENERATION_PYTHIA8PRODUCER_H
#define GENERATION_PYTHIA8PRODUCER_H

#include "GaudiAlg/GaudiAlgorithm.h"

#include "HepMC/IO_GenEvent.h"

class Pythia8Producer: public GaudiAlgorithm {
public:
  /// Constructor.
  Pythia8Producer(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// the name of the input file
  std::string m_filename;
  /// the input file
  HepMC::IO_GenEvent* m_file;
};

#endif // GENERATION_PYTHIA8PRODUCER_H
