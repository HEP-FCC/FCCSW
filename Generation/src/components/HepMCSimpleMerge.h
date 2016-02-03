
#ifndef GENERATION_HEPMCPILEMERGETOOL_H
#define GENERATION_HEPMCPILEMERGETOOL_H

#include "FWCore/DataHandle.h"
#include "Generation/IHepMCMergeTool.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"

class HepMCSimpleMerge: public GaudiTool, virtual public IHepMCMergeTool {
public:
  HepMCSimpleMerge( const std::string& type, 
    const std::string& name, const IInterface* parent);

  virtual ~HepMCSimpleMerge() final;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  /** Merge the events in the container. The first event is copied in full.
   * For all other events, new vertices are created to which the final-state particles are attached.
   */
  virtual HepMC::GenEvent* merge(const std::vector<HepMC::GenEvent>& eventVector) final;

};

#endif //GENERATION_HEPMCPILEMERGETOOL_H
