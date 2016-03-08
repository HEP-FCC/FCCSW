#ifndef SIMG4COMPONENTS_G4SIMSVC_H
#define SIMG4COMPONENTS_G4SIMSVC_H

// FCCSW
#include "SimG4Common/RunManager.h"
#include "SimG4Interface/IG4SimSvc.h"
#include "SimG4Interface/IG4DetectorConstruction.h"
#include "SimG4Interface/IG4PhysicsList.h"
#include "SimG4Interface/IG4ActionTool.h"
#include "SimG4Interface/IG4MagneticFieldTool.h"
#include "SimG4Interface/IG4ParticleGeneratorTool.h"

//Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"

/** @class G4SimSvc SimG4Components/SimG4Components/G4SimSvc.h G4SimSvc.h
 *
 *  Main Geant simulation service.
 *  It handles Geant initialization (via tools) and communication with the G4RunManager.
 *
 *  @author Anna Zaborowska
 */


class G4SimSvc: public extends1<Service, IG4SimSvc> {
public:
  /// Standard constructor
  explicit G4SimSvc(const std::string& aName, ISvcLocator* aSL);
  /// Standard destructor
  virtual ~G4SimSvc();
  /**  Initialize the Geant simulation service.
   *   Tools set in the configuration file need to specify the detector construction,
   *   physics list and user action initialization to initialize G4RunManager.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize the Geant simulation service.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /**  Simulate the event with Geant.
   *   @param[in] aEvent An event to be processed.
   *   @return status code
   */
  StatusCode processEvent(G4Event& aEvent);
  /**  Retrieve the processed event.
   *   @param[out] aEvent The processed event.
   *   @return status code
   */
  StatusCode retrieveEvent(G4Event*& aEvent);
  /**  Terminate the event simulation.
   *   @return status code
   */
  StatusCode terminateEvent();
private:
   /// Pointer to the tool service
  SmartIF<IToolSvc> m_toolSvc;
  /// Handle for the detector construction tool
  ToolHandle<IG4DetectorConstruction> m_detectorTool;
  /// Handle for the Geant physics list tool
  ToolHandle<IG4PhysicsList> m_physicsListTool;
  /// Handle for the user action initialization
  ToolHandle<IG4ActionTool> m_actionsTool;
  /// Handle for the magnetic field initialization
  ToolHandle<IG4MagneticFieldTool> m_magneticFieldTool;
/// Handle for the (G4 style) particle generator action initialization
  ToolHandle<IG4ParticleGeneratorTool> m_particleGeneratorTool; 
  
  /// Geant4 commands to be executed
  std::vector<std::string> m_g4Commands;
   
  /// Run Manager
  sim::RunManager m_runManager;
};

#endif /* SIMG4COMPONENTS_G4SIMSVC_H */
