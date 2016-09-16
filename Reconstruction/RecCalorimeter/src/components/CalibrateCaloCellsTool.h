#ifndef RECCALORIMETER_CALIBRATECALOCELLSTOOL_H
#define RECCALORIMETER_CALIBRATECALOCELLSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ICalibrateCaloCellsTool.h"

#include <vector>

/** @class CalibrateCaloCellsTool
 *
 *  Tool for calorimeter cells calibration to EM scale
 *  1/sampling fraction (Ebeam/Etotal_hit_energy) used for the calibration, same factor for each cell used
 *    - depends on geometry (thickness of active/passive material, materials)
 *  TODO: 
 *     - read 1/sampling fraction from DB when available
 *
 *  @author Jana Faltova
 *  @date   2016-09
 */

class CalibrateCaloCellsTool : public GaudiTool, virtual public ICalibrateCaloCellsTool  {
public:
  CalibrateCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~CalibrateCaloCellsTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /// Calibrate Geant4 hit energy to EM scale
  virtual void Calibrate(std::vector<fcc::CaloHit*>& aCells) final;

private:
  ///Value of 1/sampling fraction
  double m_sf;
  
};

#endif /* RECCALORIMETER_CALIBRATECALOCELLSTOOL_H */
