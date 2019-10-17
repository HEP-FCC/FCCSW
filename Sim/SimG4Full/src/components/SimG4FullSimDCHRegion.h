#ifndef SIMG4FAST_SIMG4FULLSIMDCHREGION_H
#define SIMG4FAST_SIMG4FULLSIMDCHREGION_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/ISimG4ParticleSmearTool.h"
#include "SimG4Interface/ISimG4RegionTool.h"
#include "G4UserLimits.hh"

// Geant
//class G4VFastSimulationModel;
class G4Region;

/** @class SimG4FullSimDCHRegion SimG4Fast/src/components/SimG4FullSimDCHRegion.h SimG4FullSimDCHRegion.h
 *
 *  Tool for creating regions for fast simulation, attaching sim::FastSimModelTracker to them.
 *  Regions are created for volumes specified in the job options (\b'volumeNames').
 *  User may define in job options the momentum range (\b'minP', \b'maxP') and the maximum pseudorapidity (\b'maxEta')
 *  for which the fast simulation is triggered (for other particles full simulation is performed).
 *  [For more information please see](@ref md_sim_doc_geant4fastsim).
 *
 *  @author nalipour
*/

class SimG4FullSimDCHRegion : public GaudiTool, virtual public ISimG4RegionTool {
public:
  explicit SimG4FullSimDCHRegion(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4FullSimDCHRegion();
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
  inline virtual const std::vector<std::string>& volumeNames() const final { return m_volumeNames; };
 

private:
  /// Envelopes that are used in a parametric simulation
  /// deleted by the G4RegionStore
  std::vector<G4Region*> m_g4regions;
  //  std::vector<std::unique_ptr<G4UserLimits>> m_models;
  /// Names of the parametrised volumes (set by job options)
  Gaudi::Property<std::vector<std::string>> m_volumeNames{this, "volumeNames", {}, "Names of the parametrised volumes"};
  Gaudi::Property<double> m_maxStepLength{this, "max_step_length", 0, "Step length for the region."};
  G4UserLimits*      fStepLimit;       // pointer to user step limits
};

#endif /* SIMG4FAST_SIMG4FULLSIMDCHREGION_H */
