
#ifndef GENERATION_HEPMCFULLMERGE_H
#define GENERATION_HEPMCFULLMERGE_H

#include "FWCore/DataHandle.h"
#include "Generation/IHepMCMergeTool.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"


/** 
 * Implementation partly due to pilemc https://pilemc.hepforge.org/
 */

class HepMCFullMerge: public GaudiTool, virtual public IHepMCMergeTool {
public:
  HepMCFullMerge( const std::string& type, 
    const std::string& name, const IInterface* parent);

  virtual ~HepMCFullMerge() final;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  /** Merge the events in the container.
   */
  virtual HepMC::GenEvent* merge(const std::vector<HepMC::GenEvent>& eventVector) final;

};

#endif //GENERATION_HEPMCFULLMERGE_H
