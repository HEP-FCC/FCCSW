#ifndef SIMG4COMPONENTS_GEANTSVC_H
#define SIMG4COMPONENTS_GEANTSVC_H

// FCCSW
#include "SimG4Interface/IGeantSvc.h"
#include "SimG4Common/RunManager.h"
class IGeoSvc;
class IGeantConfigTool;

//Gaudi
#include "GaudiKernel/Service.h"
class IToolSvc;

namespace sim {
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
  IToolSvc* m_toolSvc;
  /// Pointer to the interface of geometry service
  IGeoSvc* m_geoSvc;
  /// Pointer to the Geant configuration tool
  IGeantConfigTool* m_geantConfigTool;
  /// Name of the IGeantConfigName (set by options)
  std::string m_geantConfigName;
  /// Run Manager
  RunManager m_runManager;
  /* std::unique_ptr<RunManager> m_runManager; */
  };
}

#endif /* SIMG4COMPONENTS_GEANTSVC_H */
