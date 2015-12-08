#ifndef SIM_IGEANTCONFIGTOOL_H
#define SIM_IGEANTCONFIGTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant4
class G4VModularPhysicsList;
class G4VUserActionInitialization;

/**
   @brief     Interface to the Geant configuration tool.
   @author    Anna Zaborowska
*/

static const InterfaceID IID_IGeantConfigTool("IGeantConfigTool",1,0);

class IGeantConfigTool : virtual public IAlgTool {
public:
   static const InterfaceID& interfaceID() { return IID_IGeantConfigTool; }

   virtual G4VModularPhysicsList* getPhysicsList() = 0;

   virtual G4VUserActionInitialization* getActionInitialization() = 0;

   virtual StatusCode getOtherSettings() = 0;
};
#endif
