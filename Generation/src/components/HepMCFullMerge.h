
#ifndef GENERATION_HEPMCFULLMERGE_H
#define GENERATION_HEPMCFULLMERGE_H

#include "FWCore/DataHandle.h"
#include "Generation/IHepMCMergeTool.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"


/** 
 * Merge several HepMC events into one. Keeps all particles of all events,
 * not just final state ones, in contrast to HepMCSimpleMerge
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
   *  ownership of the created event is assumed to be given to the data service
   *  after calling this method.
   */
  virtual HepMC::GenEvent* merge(const std::vector<HepMC::GenEvent>& eventVector) final;

};

#endif //GENERATION_HEPMCFULLMERGE_H
