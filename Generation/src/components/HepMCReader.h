
#ifndef GENERATION_HEPMCREADER_H
#define GENERATION_HEPMCREADER_H

#include "Generation/IHepMCFileReaderTool.h"
#include "Generation/IVertexSmearingTool.h"
#include "Generation/IPileUpTool.h"
#include "Generation/IHepMCMergeTool.h"

#include "FWCore/DataHandle.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace HepMC {
  class GenEvent;
}

/**@class HepMCReader HepMCReader.h HepMCReader.h
 *
 *  This algorithm reads in events from a HepMC file and puts them into the
 *  transient event store. Various Tools are used to read additional (pileup)
 *  events from a second HepMC file, smear the vertices and join them to a final
 *  event.
 *
 *  @author  Benedikt Hegner
 *  @author  Valentin Volkl
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
  /// Tools to handle input from HepMC-file
  ToolHandle<IHepMCFileReaderTool> m_signalFileReader;
  ToolHandle<IHepMCFileReaderTool> m_pileupFileReader;
  
  // Pileup Interface Tool
  ToolHandle<IPileUpTool> m_pileUpTool;
  /// Tool to merge HepMC events
  ToolHandle<IHepMCMergeTool> m_HepMCMergeTool;
  // Tool to smear vertices
  ToolHandle<IVertexSmearingTool> m_vertexSmearingTool;
  // output handle for finished event
  DataHandle<HepMC::GenEvent> m_hepmchandle;
};

#endif //GENERATION_HEPMCREADER_H
