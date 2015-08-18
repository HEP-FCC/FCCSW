#ifndef _GEANT4SIMULATION_H_
#define _GEANT4SIMULATION_H_

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "DetDesInterfaces/IGeoSvc.h"
#include "HepMC/GenEvent.h"
#include "FWCore/DataHandle.h"

// albers
#include "datamodel/ParticleCollection.h"
#include "datamodel/Particle.h"

// Geant4
#include "G4RunManager.hh"
#include "G4Event.hh"

class Geant4Simulation: public GaudiAlgorithm , private G4RunManager{
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
  /// Handle for the HepMC to be read
  DataHandle<HepMC::GenEvent> m_eventhandle;
  /// Handle for the "reconstructed" to be written
  DataHandle<ParticleCollection> m_recphandle;

  // G4RunManager* m_runManager;
    IGeoSvc* m_geoSvc;

};

#endif
