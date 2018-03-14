#ifndef SIMG4FULL_SIMG4USERLIMITREGION_H
#define SIMG4FULL_SIMG4USERLIMITREGION_H

#include <cfloat>

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "SimG4Interface/ISimG4ParticleSmearTool.h"
#include "SimG4Interface/ISimG4RegionTool.h"

// Geant
#include "G4UserLimits.hh"
class G4Region;

/** @class SimG4UserLimitRegion SimG4Full/src/components/SimG4UserLimitRegion.h SimG4UserLimitRegion.h
 *
 *  Tool for creating regions with user limits.
 *  It requires SimG4UserLimitPhysics to be used.
 *
 *  @author Anna Zaborowska
*/

class SimG4UserLimitRegion : public GaudiTool, virtual public ISimG4RegionTool {
public:
  explicit SimG4UserLimitRegion(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4UserLimitRegion();
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
  /// Regions used to set user limits
  /// deleted by the G4RegionStore
  std::vector<G4Region*> m_g4regions;
  /// User limits
  std::vector<std::unique_ptr<G4UserLimits>> m_userLimits;
  /// Names of the volumes where user limits should be attached (set by job options)
  Gaudi::Property<std::vector<std::string>> m_volumeNames{this, "volumeNames", {}, "Names of the volumes"};
  /// max allowed Step size in this volume  (set by job options)
  Gaudi::Property<double> m_maxStep{this, "maxStep", DBL_MAX, "maximum step"};
  /// max total track length (set by job options)
  Gaudi::Property<double> m_maxTrack{this, "maxTrack", DBL_MAX, "maximum total track length"};
  /// max time (set by job options)
  Gaudi::Property<double> m_maxTime{this, "maxTime", DBL_MAX, "max time"};
  /// min kinetic energy  (only for charged particles) (set by job options)
  Gaudi::Property<double> m_minKineticEnergy{this, "minKineticEnergy", 0, "min kinetic energy"};
  /// min remaining range (only for charged particles) (set by job options)
  Gaudi::Property<double> m_minRange{this, "minRange", 0, "min remaining range "};
};

#endif /* SIMG4FULL_SIMG4USERLIMITREGION_H */
