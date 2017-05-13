
#ifndef GENERATION_HEPMCFULLMERGE_H
#define GENERATION_HEPMCFULLMERGE_H

#include "FWCore/DataHandle.h"
#include "Generation/IHepMCMergeTool.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"

/**
 * Merge several HepMC events into one. Keeps all particles of all events,
 * not just final state ones, in contrast to HepMCSimpleMerge
 * Implementation partly due to pilemc https://pilemc.hepforge.org/
 */

class HepMCFullMerge : public GaudiTool, virtual public IHepMCMergeTool {
public:
  HepMCFullMerge(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~HepMCFullMerge() final;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  /** Merge the events in the container into the signalEvent
   * For events in the vector, new vertices in signalEvent are created to which all pile-up particles are attached.
   *  @param[in/out] signalEvent is the signal event that will be enriched with puEvents from eventVector
   *  @param[in] eventVector is the vector of pile-up GenEvents
   */
  virtual StatusCode merge(HepMC::GenEvent& signalEvent, const std::vector<HepMC::GenEvent>& eventVector) final;
};

#endif  // GENERATION_HEPMCFULLMERGE_H
