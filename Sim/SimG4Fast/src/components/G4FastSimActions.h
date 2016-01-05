#ifndef GEANTSIM_GEANTFASTCONFIG_H
#define GEANTSIM_GEANTFASTCONFIG_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4ActionTool.h"


/** @class G4FastSimActions G4FastSimActions.h SimG4Fast/G4FastSimActions.h
 *
 *  implementation of a tool to load Fast Simulation User Action Initialization (list of user actions)
 *
 *  @author Anna Zaborowska
*/

class G4FastSimActions: public GaudiTool, virtual public IG4ActionTool {
public:
   G4FastSimActions(const std::string& type , const std::string& name,
               const IInterface* parent);
   virtual ~G4FastSimActions();

   virtual StatusCode initialize() final;

   virtual G4VUserActionInitialization* getUserActionInitialization() final;
};

#endif
