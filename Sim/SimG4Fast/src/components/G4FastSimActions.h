#ifndef SIM_G4FASTSIMACTIONS_H
#define SIM_G4FASTSIMACTIONS_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4ActionTool.h"
class ISmearingTool;


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

private:
  /// Popinter to the smearing tool
  ISmearingTool* m_smearTool;
  /// Name of the ISmearingTool
  std::string m_smearToolName;
};

#endif /* SIM_G4FASTSIMACTIONS_H */
