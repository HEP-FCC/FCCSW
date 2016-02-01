
#ifndef GENERATION_HEPMCFILEREADER_H 
#define GENERATION_HEPMCFILEREADER_H

#include "GaudiAlg/GaudiTool.h"
#include "IHepMCFileReaderTool.h"

#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"

class HepMCFileReader: public GaudiTool, virtual public IHepMCFileReaderTool {
public:
  HepMCFileReader( const std::string& type, const std::string& name,
    const IInterface* parent) ;
  virtual ~HepMCFileReader(); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /// Wrapper for HepMC's fill_next_event()
  virtual HepMC::GenEvent* readNextEvent();
  /// Wrapper for HepMC file io.
  virtual StatusCode open(const std::string& filename);

private:
  void close();
  std::string m_filename;
  HepMC::IO_GenEvent* m_file;
};

#endif // GENERATION_HEPMCFILEREADER_H
