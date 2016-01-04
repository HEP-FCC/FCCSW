#ifndef SIMG4COMPONENTS_GEANTSVC_H
#define SIMG4COMPONENTS_GEANTSVC_H

// FCCSW
#include "SimG4Common/RunManager.h"
#include "SimG4Interface/IGeantSvc.h"
#include "SimG4Interface/IGeantConfigTool.h"
#include "SimG4Interface/IG4DetectorConstruction.h"
#include "SimG4Interface/IG4PhysicsList.h"

//Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"
class IToolSvc;

class GeantSvc: public extends1<Service, IGeantSvc> {
public:
  /// Standard constructor
  GeantSvc(const std::string& aName, ISvcLocator* aSL);
  /// Standard destructor
  virtual ~GeantSvc();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;
  StatusCode processEvent(G4Event& aEvent);
  StatusCode retrieveEvent(G4Event*& aEvent);
  StatusCode terminateEvent();
private:
  /// Pointer to the tool service
  SmartIF<IToolSvc> m_toolSvc;
  /// Pointer to the detector construction tool
  ToolHandle<IG4DetectorConstruction> m_detectorTool;
  /// Pointer to the Geant configuration tool
  ToolHandle<IGeantConfigTool> m_geantConfigTool;
  /// Pointer to the Geant physics list tool
  ToolHandle<IG4PhysicsList> m_physicsListTool;
  /// Run Manager
  sim::RunManager m_runManager;
};

#endif /* SIMG4COMPONENTS_GEANTSVC_H */
