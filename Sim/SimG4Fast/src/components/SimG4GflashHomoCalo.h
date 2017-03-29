#ifndef SIMG4FAST_SIMG4GFLASHHOMOCALO_H
#define SIMG4FAST_SIMG4GFLASHHOMOCALO_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/ISimG4GflashTool.h"

/** @class SimG4GflashHomoCalo SimG4Fast/src/components/SimG4GflashHomoCalo.h SimG4GflashHomoCalo.h
 *
 *  Tool creating a parametrisation of a homogenous calorimeter.
 *  The original parameters from arXiv:hep-ex/0001020v1 are taken.
 *  Material of the calorimeter is set in a property '\b material'.
 *
 *  @author Anna Zaborowska
*/

class SimG4GflashHomoCalo : public GaudiTool, virtual public ISimG4GflashTool {
public:
  explicit SimG4GflashHomoCalo(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SimG4GflashHomoCalo();
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
  /// Material name of the homogenous calorimeter (to be searched for in Geant NIST table)
  Gaudi::Property<std::string> m_material{
      this, "material", "", "Material name of the homogenous calorimeter (to be searched for in Geant NIST table)"};
};

#endif /* SIMG4FAST_SIMG4GFLASHHOMOCALO_H */
