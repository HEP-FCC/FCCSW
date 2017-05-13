#ifndef SIMG4FAST_SIMG4FASTSIMCALORIMETERREGION_H
#define SIMG4FAST_SIMG4FASTSIMCALORIMETERREGION_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/ISimG4GflashTool.h"
#include "SimG4Interface/ISimG4RegionTool.h"

// Geant
#include "GFlashHitMaker.hh"
#include "GFlashParticleBounds.hh"
#include "GVFlashShowerParameterisation.hh"
class G4VFastSimulationModel;
class G4Region;

/** @class SimG4FastSimCalorimeterRegion SimG4Fast/src/components/SimG4FastSimCalorimeterRegion.h
 * SimG4FastSimCalorimeterRegion.h
 *
 *  Tool for creating regions for fast simulation, attaching GFlashModel to them.
 *  Regions are created for volumes specified in the job options (\b'volumeNames').
 *  Details on the parametrisation of shower profiles is set by tool '\b parametrisation'
 *  [For more information please see](@ref md_sim_doc_geant4fastsim).
 *
 *  @author Anna Zaborowska
*/

class SimG4FastSimCalorimeterRegion : public GaudiTool, virtual public ISimG4RegionTool {
public:
  explicit SimG4FastSimCalorimeterRegion(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4FastSimCalorimeterRegion();
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
  /// Pointer to a parametrisation tool, to retrieve calorimeter parametrisation
  ToolHandle<ISimG4GflashTool> m_parametrisationTool{"SimG4GflashHomo", this, true};
  /// Envelopes that are used in a parametric simulation
  /// deleted by the G4RegionStore
  std::vector<G4Region*> m_g4regions;
  /// Fast simulation (parametrisation) models
  std::vector<std::unique_ptr<G4VFastSimulationModel>> m_models;
  /// GFlash model parametrisation (retrieved from the m_parametrisationTool)
  std::unique_ptr<GVFlashShowerParameterisation> m_parametrisation{nullptr};
  /// GFlash model configuration
  std::unique_ptr<GFlashParticleBounds> m_particleBounds{nullptr};
  /// GFlash hit maker
  std::unique_ptr<GFlashHitMaker> m_hitMaker{nullptr};
  /// Names of the parametrised volumes (set by job options)
  Gaudi::Property<std::vector<std::string>> m_volumeNames{
      this, "volumeNames", {}, "Names of the parametrised volumes (set by job options)"};
  /// minimum energy of the electron (positron) that triggers the model
  Gaudi::Property<double> m_minTriggerEnergy{this, "minEnergy", 0.1 * Gaudi::Units::GeV,
                                             "minimum energy of the electron (positron) that triggers the model"};
  /// maximum energy of the electron (positron) that triggers the model
  Gaudi::Property<double> m_maxTriggerEnergy{this, "minEnergy", 10 * Gaudi::Units::TeV,
                                             "maximum energy of the electron (positron) that triggers the model"};
  /// threshold below which the electrons (positrons) are killed
  Gaudi::Property<double> m_energyToKill{this, "minEnergy", 0.1 * Gaudi::Units::GeV,
                                         "threshold below which the electrons (positrons) are killed"};
};

#endif /* SIMG4FAST_SIMG4FASTSIMCALORIMETERREGION_H */
