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
 *  Regions are created for volumes specified in the job options (\b'volumeNames').
 *  User may define in job options the momentum range (\b'minP', \b'maxP') and the maximum pseudorapidity (\b'maxEta')
 *  for which the fast simulation is triggered (for other particles full simulation is performed).
 *  [For more information please see](@ref md_sim_doc_geant4fastsim).
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
  /**  Get the names of the volumes where fast simulation should be performed.
   *   @return vector of volume names
   */
  inline virtual const std::vector<std::string>& volumeNames() const final {return m_volumeNames;};
  /**  Get the minimum momentum that triggers fast simulation
   *   @return maximum p
   */
  inline virtual double minP() const final {return m_minP;};
  /**  Get the maximum momentum that triggers fast simulation
   *   @return maximum p
   */
  inline virtual double maxP() const final {return m_maxP;};
  /**  Get the maximum pseudorapidity that triggers fast simulation
   *   @return maximum p
   */
  inline virtual double maxEta() const final {return m_maxEta;};

private:
  /// Pointer to a smearing tool, to retrieve tracker configuration (names of volumes)
  ToolHandle<ISimG4ParticleSmearTool> m_smearTool;
  /// Envelopes that are used in a parametric simulation
  /// deleted by the G4RegionStore
  std::vector<G4Region*> m_g4regions;
  /// Fast simulation (parametrisation) models
  std::vector<std::unique_ptr<G4VFastSimulationModel>> m_models;
  /// Names of the parametrised volumes (set by job options)
  std::vector<std::string> m_volumeNames;
  /// minimum P that can be smeared (set by job options)
  double m_minP;
  /// maximum P that can be smeared (set by job options)
  double m_maxP;
  /// maximum eta that can be smeared (set by job options)
  double m_maxEta;

};

#endif /* SIMG4FAST_G4FASTSIMREGIONTRACKER_H */
