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

class IGeantConfigTool : virtual public IAlgTool {
public:
   DeclareInterfaceID(IGeantConfigTool,1,0);

   virtual G4VModularPhysicsList* getPhysicsList() = 0;

   virtual G4VUserActionInitialization* getActionInitialization() = 0;

   virtual StatusCode getOtherSettings() = 0;
};
#endif
