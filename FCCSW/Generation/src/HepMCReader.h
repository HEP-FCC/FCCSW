#ifndef _HEPMCREADER_H_
#define _HEPMCREADER_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Generation/HepMCEntry.h"


//---------------------------------------------------------------------------

/**@class HepMCReader HepMCReader.h Generation/HepMCReader.h
 *
 *  This algorithm reads-in events from a HepMC file and puts them into the
 *  transient event store.
 *
 *  @author  Benedikt Hegner
 *  @version 1.0
 */
class HepMCReader: public GaudiAlgorithm {
  friend class AlgFactory<HepMCReader> ;

public:
  /// Constructor.
  HepMCReader(const std::string& name, ISvcLocator* svcLoc);
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
  /// The output handle for what is being produced
  DataObjectHandle<HepMCEntry> m_hepmchandle;
};

#endif
