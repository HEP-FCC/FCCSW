#ifndef SIMG4COMPONENTS_G4SIMALG_H
#define SIMG4COMPONENTS_G4SIMALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4EventProviderTool.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"

// Forward declarations:
// Interfaces
class ISimG4Svc;

// datamodel
namespace fcc {
class MCParticleCollection;
}
// Geant
class G4Event;

/** @class SimG4Alg SimG4Components/src/SimG4Alg.h SimG4Alg.h
 *
 *  Geant simulation algorithm.
 *  Controls the event flow: translates the EDM event to G4Event, passes it to SimG4Svc,
 *  retrieves it after the finished simulation, and stores the output as specified in tools.
 *  It takes MCParticleCollection (\b'genParticles') as the input
 *  as well as a list of names of tools that define the EDM output (\b'outputs').
 *  [For more information please see](@ref md_sim_doc_geant4fullsim).
 *
 *  @author Anna Zaborowska
 */

class SimG4Alg : public GaudiAlgorithm {
public:
  explicit SimG4Alg(const std::string&, ISvcLocator*);
  virtual ~SimG4Alg();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Execute the simulation.
   *   Translation of MCParticleCollection to G4Event is done using EDM2G4() method.
   *   Then, G4Event is passed to SimG4Svc for the simulation and retrieved afterwards.
   *   The tools m_saveTools are used to save the output from the retrieved events.
   *   Finally, the event is terminated.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

private:
  /// Pointer to the interface of Geant simulation service
  SmartIF<ISimG4Svc> m_geantSvc;
  /// Handle to the tools saving the output
  /// to be replaced with the ToolHandleArray<ISimG4SaveOutputTool> m_saveTools
  std::vector<ISimG4SaveOutputTool*> m_saveTools;
  /// Names for the saving tools
  /// to be deleted once the ToolHandleArray<ISimG4SaveOutputTool> m_saveTools is in place
  Gaudi::Property<std::vector<std::string>> m_saveToolNames{this, "outputs", {}, "Names for the saving tools"};
  /// Handle for tool that creates the G4Event
  ToolHandle<ISimG4EventProviderTool> m_eventTool{"SimG4PrimariesFromEdmTool", this};
};
#endif /* SIMG4COMPONENTS_G4SIMALG_H */
