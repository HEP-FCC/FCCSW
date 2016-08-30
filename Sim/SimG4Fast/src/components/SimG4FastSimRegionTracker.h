#ifndef SIMG4FAST_G4FASTSIMREGIONTRACKER_H
#define SIMG4FAST_G4FASTSIMREGIONTRACKER_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/ISimG4RegionTool.h"
#include "SimG4Interface/ISimG4ParticleSmearTool.h"

// Geant
class G4VFastSimulationModel;
class G4Region;


/** @class SimG4FastSimRegionTracker SimG4Fast/src/components/SimG4FastSimRegionTracker.h SimG4FastSimRegionTracker.h
 *
 *  Tool for creating regions for fast simulation, attaching sim::FastSimModelTracker to them.
 *
 *  @author Anna Zaborowska
*/

class SimG4FastSimRegionTracker: public GaudiTool, virtual public ISimG4RegionTool {
public:
  explicit SimG4FastSimRegionTracker(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4FastSimRegionTracker();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /**  Create regions and fast simulation models
   *   @return status code
   */
  virtual StatusCode create() final;

private:
  /// Pointer to a smearing tool, to retrieve tracker configuration (names of volumes)
  ToolHandle<ISimG4ParticleSmearTool> m_smearTool;
  /// Envelopes that are used in a parametric simulation
  /// deleted by the G4RegionStore
  std::vector<G4Region*> m_g4regions;
  /// Fast simulation (parametrisation) models
  std::vector<std::unique_ptr<G4VFastSimulationModel>> m_models;

};

#endif /* SIMG4FAST_G4FASTSIMREGIONTRACKER_H */
