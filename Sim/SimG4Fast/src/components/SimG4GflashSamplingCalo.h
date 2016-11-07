#ifndef SIMG4FAST_SIMG4GFLASHSAMPLINGCALO_H
#define SIMG4FAST_SIMG4GFLASHSAMPLINGCALO_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/ISimG4GflashTool.h"


/** @class SimG4GflashSamplingCalo SimG4Fast/src/components/SimG4GflashSamplingCalo.h SimG4GflashSamplingCalo.h
 *
 *  Tool creating a parametrisation of a sampling calorimeter.
 *  The original parameters from arXiv:hep-ex/0001020v1 are taken.
 *  Materials of the active and passive layers of the calorimeter are set in a property '\b materials' (length: 2).
 *  Respective thicknesses of the layers are set in a property '\b thickness'.
 *
 *  @author Anna Zaborowska
*/

class SimG4GflashSamplingCalo: public GaudiTool, virtual public ISimG4GflashTool {
public:
  explicit SimG4GflashSamplingCalo(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4GflashSamplingCalo();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /**  Get the parametrisation
   *   @return unique pointer to the parametrisation
   */
  virtual std::unique_ptr<GVFlashShowerParameterisation> parametrisation() final;

private:
  /// Material names of the layers in the sampling calorimeter (to be searched for in Geant NIST table)
  std::vector<std::string> m_materials;

  /// Thicknesses of the layers
  std::vector<double> m_thickness;

};

#endif /* SIMG4FAST_SIMG4GFLASHSAMPLINGCALO_H */
