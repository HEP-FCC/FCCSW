#ifndef SIMG4FAST_G4FASTSIMACTIONS_H
#define SIMG4FAST_G4FASTSIMACTIONS_H

// Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/IG4ActionTool.h"
#include "SimG4Interface/IG4ParticleSmearTool.h"


/** @class G4FastSimActions SimG4Fast/src/components/G4FastSimActions.h G4FastSimActions.h
 *
 *  Tool for loading fast simulation user action initialization (list of user actions)
 *
 *  @author Anna Zaborowska
*/

class G4FastSimActions: public AlgTool, virtual public IG4ActionTool {
public:
  explicit G4FastSimActions(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~G4FastSimActions();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /** Get the user action initialization.
   *  @return pointer to G4VUserActionInitialization
   */
  virtual G4VUserActionInitialization* getUserActionInitialization() final;

private:
  /// Pointer to the smearing tool
  ToolHandle<IG4ParticleSmearTool> m_smearTool;

};

#endif /* SIMG4FAST_G4FASTSIMACTIONS_H */
