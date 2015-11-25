#ifndef GEANTSIM_GEANTFULLCONFIG_H
#define GEANTSIM_GEANTFULLCONFIG_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "GeantConfig/IGeantConfigTool.h"

/**
   @brief     Geant config tool for full simulation.
   @details   Geant config tool for full simulation.
   @author    Anna Zaborowska
*/

class GeantFullSimConfig: public GaudiTool, virtual public IGeantConfigTool {
public:
   GeantFullSimConfig(const std::string& type , const std::string& name,
               const IInterface* parent);
   virtual ~GeantFullSimConfig();

   virtual StatusCode initialize();

   virtual G4VModularPhysicsList* getPhysicsList();

   virtual G4UserTrackingAction* getTrackingAction();

   virtual StatusCode setOther();
};

#endif
