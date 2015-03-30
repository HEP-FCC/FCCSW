#ifndef _GEANT4SIMULATION_H_
#define _GEANT4SIMULATION_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "HepMC/GenEvent.h"
#include "FWCore/DataHandle.h"
#include "DataObjects/ParticleCollection.h"
#include "DataObjects/Particle.h"

#include "G4RunManager.hh"
#include "G4Event.hh"

class Geant4Simulation: public GaudiAlgorithm {
  friend class AlgFactory<Geant4Simulation> ;

public:
  /// Constructor.
  Geant4Simulation(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();
  /// Converter between HepMS::GenEvent and G4Event
  void HepMC2G4(const HepMC::GenEvent* aHepMCEvent, G4Event* aG4Event);
private:
  /* /// Pointer to Geant4 geometry */
  /*G4VUser.... * m_g4detector; */
  /// Handle for the HepMC to be read
  DataHandle<HepMC::GenEvent> m_eventhandle;
  /// Handle for the "reconstructed" to be written
  DataObjectHandle<ParticleCollection> m_recphandle;

  G4RunManager* m_runManager;

};

#endif
