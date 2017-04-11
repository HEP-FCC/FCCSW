#ifndef GENERATION_HEPMCFILEWRITER_H
#define GENERATION_HEPMCFILEWRITER_H

#include "GaudiAlg/GaudiAlgorithm.h"

#include "FWCore/DataHandle.h"

namespace HepMC {
class GenEvent;
class IO_GenEvent;
}

/**
 *
 * \brief Algorithm to write HepMC input to a text file.
 *
 * \attention
 * The HepMC format is text-based, fairly verbose and more suitable
 * for debugging than actual storage of physics result, which should be
 * done in the fccsw event data format.
 */

class HepMCFileWriter : public GaudiAlgorithm {
  friend class AlgFactory<HepMCFileWriter>;

public:
  /// Constructor.
  HepMCFileWriter(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the HepMC to be read
  DataHandle<HepMC::GenEvent> m_hepmchandle{"HepMC", Gaudi::DataHandle::Reader, this};
  Gaudi::Property<std::string> m_filename{this, "Filename", "Output_HepMC.dat", "Name of the HepMC file to write"};
  std::unique_ptr<HepMC::IO_GenEvent> m_file;
};

#endif  // GENERATION_HEPMCFILEWRITER_H
