#ifndef GENERATION_HEPEVTREADER_H
#define GENERATION_HEPEVTREADER_H


#include "Generation/IHepMCFileReaderTool.h"
#include "Generation/IVertexSmearingTool.h"
#include "Generation/IHepMCMergeTool.h"

#include "FWCore/DataHandle.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "HepMC/IO_GenEvent.h"


namespace HepMC {
  class GenEvent;
}

namespace fcc {
class MCParticleCollection;
class GenVertexCollection;
}

/**@class HepEVTReader HepEVTReader.h HepEVTReader.h
 *
 *  This algorithm reads in events from a HepMC file and puts them into the
 *  transient event store. Various Tools are used to read additional (pileup)
 *  events from a second HepMC file, smear the vertices and join them to a final
 *  event.
 *
 *  @author  nalipour
 *  @version 1.0
 */

class HepEVTReader: public GaudiAlgorithm {
  friend class AlgFactory<HepEVTReader> ;

public:
  /// Constructor.
  HepEVTReader(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:

  std::string m_filename;
  std::ifstream m_input;
  int NHEP;
  int m_format;

  /// Handle for the genparticles to be written
  DataHandle<fcc::MCParticleCollection> m_genphandle {"genparticles", Gaudi::DataHandle::Writer, this};
  /// Handle for the genvertices to be written
  DataHandle<fcc::GenVertexCollection> m_genvhandle {"genparticles", Gaudi::DataHandle::Writer, this};


  /// Tools to handle input from HepMC-file
  ToolHandle<IHepMCFileReaderTool> m_signalFileReader;
  ToolHandle<IHepMCFileReaderTool> m_pileupFileReader;
  
  // Pileup Interface Tool
  // // //  ToolHandle<IPileUpTool> m_pileUpTool;
  /// Tool to merge HepMC events
  ToolHandle<IHepMCMergeTool> m_HepMCMergeTool;
  // Tool to smear vertices
  ToolHandle<IVertexSmearingTool> m_vertexSmearingTool;
};

#endif //GENERATION_HEPEVTREADER_H
