#ifndef _HEPMCTOGEANTCONVERTER_H_
#define _HEPMCTOGEANTCONVERTER_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DataObjects/HepMCEntry.h"
#include "DataObjects/GeantEventEntry.h"
#include "GaudiKernel/DataObjectHandle.h"

class HepMCToGeantConverter: public GaudiAlgorithm {
  friend class AlgFactory<HepMCToGeantConverter> ;

public:
  /// Constructor.
  HepMCToGeantConverter(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();
  /// Converter between HepMS::GenEvent and G4Event
  void HepMC2G4(const HepMC::GenEvent* aHepMCEvent, G4Event* aG4Event);
private:
  /// Handle for the HepMC to be read
  DataObjectHandle<HepMCEntry> m_hepmchandle;
  /// Handle for the genparticles to be written
  DataObjectHandle<GeantEventEntry> m_eventhandle;
};

#endif
