#ifndef SIM_G4FULLSIMACTIONS_H
#define SIM_G4FULLSIMACTIONS_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4ActionTool.h"

/** @class G4FullSimActions G4FullSimActions.h SimG4Full/G4FullSimActions.h
 *
 *  implementation of a tool to load Full Simulation User Action Initialization (list of user actions)
 *
 *  @author Anna Zaborowska
 */

class G4FullSimActions: public GaudiTool, virtual public IG4ActionTool {
public:
   G4FullSimActions(const std::string& type , const std::string& name,
               const IInterface* parent);
   virtual ~G4FullSimActions();

   virtual StatusCode initialize() final;

   virtual G4VUserActionInitialization* getUserActionInitialization() final;
};

#endif /* SIM_G4FULLSIMACTIONS_H */
