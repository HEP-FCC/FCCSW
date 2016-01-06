#ifndef GEANTSIM_GEANTFASTCONFIG_H
#define GEANTSIM_GEANTFASTCONFIG_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IGeantConfigTool.h"
class ISmearingTool;

// Geant
class G4VFastSimulationModel;
class G4Region;

/**
   @brief     Geant config tool for fast simulation.
   @details   Geant config tool for fast simulation.
   @author    Anna Zaborowska
*/

class GeantFastSimConfig: public GaudiTool, virtual public IGeantConfigTool {
public:
   GeantFastSimConfig(const std::string& type , const std::string& name,
               const IInterface* parent);
   virtual ~GeantFastSimConfig();

   virtual StatusCode initialize() final;

   virtual G4VModularPhysicsList* getPhysicsList() final;

   virtual G4VUserActionInitialization* getActionInitialization() final;

   virtual StatusCode getOtherSettings() final;
private:
   /// Envelopes that are used in a parametric simulation
   /// deleted by the G4RegionStore
   std::vector<G4Region*> m_g4regions;
   /// Fast Simulation Models
   std::vector<std::unique_ptr<G4VFastSimulationModel>> m_models;
   /// Pointer to the smearing tool
   ISmearingTool* m_smearTool;
   /// Name of the ISmearingTool (set by options)
   std::string m_smearToolName ;
   G4VUserActionInitialization* m_actions;
};

#endif
