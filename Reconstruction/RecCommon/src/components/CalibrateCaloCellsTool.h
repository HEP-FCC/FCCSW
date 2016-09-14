#ifndef RECONSTRUCTION_CALIBRATECALOCELLSTOOL_H
#define RECONSTRUCTION_CALIBRATECALOCELLSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ICalibrateCaloCellsTool.h"

#include <vector>

/** @class CalibrateCaloCellsTool
 *
 *  Tool for calorimeter cells calibration to EM scale
 *  1/sampling fraction (Ebeam/Etotal_hit_energy) used for the calibration, same factor for each cell used
 *    - depends on geometry (e.g. thickness of active/passive material)
 *  TODO: 
 *     - read SF from DB when available
 *
 *  @author Jana Faltova
 *  @date   2016-08
 */

class CalibrateCaloCellsTool : public GaudiTool, virtual public ICalibrateCaloCellsTool  {
public:
  CalibrateCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~CalibrateCaloCellsTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  //Calibrate Geant4 hit energy (cellEnergy) to EM scale
  virtual void Calibrate(std::vector<fcc::CaloHit*>& aCells) final;

private:
  //Value of 1/sampling fraction
  double m_sf;
  
};

#endif /* RECONSTRUCTION_CALIBRATECALOCELLSTOOL_H */
