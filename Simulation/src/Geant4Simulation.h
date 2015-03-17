#ifndef _GEANT4SIMULATION_H_
#define _GEANT4SIMULATION_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DataObjects/GeantEventEntry.h"
#include "GaudiKernel/DataObjectHandle.h"
#include "DataObjects/ParticleCollection.h"
#include "DataObjects/Particle.h"

#include "G4RunManager.hh"

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
private:
  /* /// Pointer to Geant4 geometry */
  /*G4VUser.... * m_g4detector; */
  /// Handle for the G4Event to be read
  DataObjectHandle<GeantEventEntry> m_eventhandle;
  /// Handle for the "reconstructed" to be written
  DataObjectHandle<ParticleCollection> m_recphandle;

  G4RunManager* m_runManager;

};

#endif
