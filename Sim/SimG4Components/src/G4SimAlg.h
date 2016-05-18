#ifndef SIMG4COMPONENTS_G4SIMALG_H
#define SIMG4COMPONENTS_G4SIMALG_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/IG4SaveOutputTool.h"
class IG4SimSvc;

// datamodel
namespace fcc {
class MCParticleCollection;
}

// Geant
class G4Event;

/** @class G4SimAlg SimG4Components/src/G4SimAlg.h G4SimAlg.h
 *
 *  Geant simulation algorithm.
 *  Controls the event flow: translates the EDM event to G4Event, passes it to G4SimSvc,
 *  retrieves it after the finished simulation, and stores the output as specified in tools.
 *  It takes MCParticleCollection (\b'genParticles') as the input
 *  as well as a list of names of tools that define the EDM output (\b'outputs').
 *
 *  @author Anna Zaborowska
 */

class G4SimAlg: public GaudiAlgorithm {
public:
  explicit G4SimAlg(const std::string&, ISvcLocator*);
  virtual ~G4SimAlg();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Execute the simulation.
   *   Translation of MCParticleCollection to G4Event is done using EDM2G4() method.
   *   Then, G4Event is passed to G4SimSvc for the simulation and retrieved afterwards.
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
  DataHandle<fcc::MCParticleCollection> m_genParticles;
  /// Pointer to the interface of Geant simulation service
  SmartIF<IG4SimSvc> m_geantSvc;
  /// Handle to the tools saving the output
  /// to be replaced with the ToolHandleArray<IG4SaveOutputTool> m_saveTools
  std::vector<IG4SaveOutputTool*> m_saveTools;
  /// Names for the saving tools
  /// to be deleted once the ToolHandleArray<IG4SaveOutputTool> m_saveTools is in place
  std::vector<std::string> m_saveToolNames;
};
#endif /* SIMG4COMPONENTS_G4SIMALG_H */
