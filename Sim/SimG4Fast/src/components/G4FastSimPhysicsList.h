#ifndef SIM_G4FASTSIMPHYSICSLIST_H
#define SIM_G4FASTSIMPHYSICSLIST_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/IG4PhysicsList.h"

class G4FastSimPhysicsList: public GaudiTool, virtual public IG4PhysicsList {
public:
  G4FastSimPhysicsList(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4FastSimPhysicsList();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual G4VModularPhysicsList* getPhysicsList();

private:
   /// Handle for the full physics list tool
   ToolHandle<IG4PhysicsList> m_physicsListTool;

};

#endif /* SIM_G4FASTSIMPHYSICSLIST_H */
