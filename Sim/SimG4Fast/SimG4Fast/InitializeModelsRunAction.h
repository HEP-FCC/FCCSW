#ifndef SIMG4FAST_INITIALIZEMODELSRUNACTION_H
#define SIMG4FAST_INITIALIZEMODELSRUNACTION_H

//FCCSW
class ISimG4ParticleSmearTool;

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
class IToolSvc;

// Geant
#include "G4UserRunAction.hh"
class G4VFastSimulationModel;
class G4Region;

/** @class InitializeModelsRunAction SimG4Fast/SimG4Fast/InitializeModelsRunAction.h InitializeModelsRunAction.h
 *
 *  Run action (invoked before/after Geant run initialization).
 *  Defines the action at the start and at the end of run processing.
 *  It initializes models needed by the the fast simulation (attaches envelopes to the detector volumes).
 *
 *  @author Anna Zaborowska
 */

namespace sim {
class InitializeModelsRunAction : public G4UserRunAction {
public:
  /** Constructor.
   *  @param aSmearToolName Name of the implementation of ISimG4ParticleSmearTool to be passed to models.
   */
  explicit InitializeModelsRunAction(const std::string& aSmearingToolName);
  virtual ~InitializeModelsRunAction();
  /* Defines the actions at the end of processing the track.
   * It scans the world volume and attaches FastSimModelTracker to the detector with a name m_trackerName.
   */
  virtual void  BeginOfRunAction(const G4Run*) final;

private:
  /// Message Service
  ServiceHandle<IMessageSvc> m_msgSvc;
  /// Message Stream
  MsgStream m_log;
  /// Tool Service
  ServiceHandle<IToolSvc> m_toolSvc;
  /// Pointer to a smearing tool, to retrieve tracker configuration (names of volumes)
  ISimG4ParticleSmearTool* m_smearTool;
  /// Envelopes that are used in a parametric simulation
  /// deleted by the G4RegionStore
  std::vector<G4Region*> m_g4regions;
  /// Fast simulation (parametrisation) models
  std::vector<std::unique_ptr<G4VFastSimulationModel>> m_models;
  /// Name of the used implementation of the ISimG4ParticleSmearTool
  std::string m_smearToolName;
};
}

#endif /* SIMG4FAST_INITIALIZEMODELSRUNACTION_H */


