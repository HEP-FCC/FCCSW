
#ifndef GENERATION_HEPMCPILEMERGETOOL_H
#define GENERATION_HEPMCPILEMERGETOOL_H

#include "FWCore/DataHandle.h"
#include "Generation/IHepMCMergeTool.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"

class HepMCSimpleMerge : public GaudiTool, virtual public IHepMCMergeTool {
public:
  HepMCSimpleMerge(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~HepMCSimpleMerge() final;

  StatusCode initialize() override final;

  StatusCode finalize() override final;

  /** Merge the events in the container into the signalEvent
   * For events in the vector, new vertices in signalEvent are created to which the final-state particles are attached.
   *  @param[in/out] signalEvent is the signal event that will be enriched with puEvents from eventVector
   *  @param[in] eventVector is the vector of pile-up GenEvents
   */
  StatusCode merge(HepMC::GenEvent& signalEvent, const std::vector<HepMC::GenEvent>& eventVector) override final;
};

#endif  // GENERATION_HEPMCPILEMERGETOOL_H
