#ifndef SIMG4COMPONENTS_GEANTSVC_H
#define SIMG4COMPONENTS_GEANTSVC_H

// FCCSW
#include "SimG4Interface/IGeantSvc.h"
#include "SimG4Interface/IGeantConfigTool.h"
#include "SimG4Common/RunManager.h"
class IGeoSvc;

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
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  StatusCode processEvent(G4Event* aEvent);
  StatusCode retrieveEvent(const G4Event*& aEvent);
  StatusCode terminateEvent();
private:
  /// Pointer to the tool service
  SmartIF<IToolSvc> m_toolSvc;
  /// Pointer to the interface of geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Pointer to the Geant configuration tool
  ToolHandle<IGeantConfigTool> m_geantConfigTool;
  /// Run Manager
  sim::RunManager m_runManager;
};

#endif /* SIMG4COMPONENTS_GEANTSVC_H */
