#ifndef RECONSTRUCTION_NOISECALOCELLSTOOL_H
#define RECONSTRUCTION_NOISECALOCELLSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

// FCCSW
#include "RecInterface/INoiseCaloCellsTool.h"

/** @class NoiseCaloCellsTool
 *
 *  Tool for calorimeter noise: Adds gaussian noise to each cell
 *  Noise defined by a single value - rms of the noise, same for each cell
 *  TODO: 
 *     - noise dependence on the size of the cell
 *     - read noise from DB when available
 *
 *  @author Jana Faltova
 *  @date   2016-08
 */

class NoiseCaloCellsTool : public GaudiTool, virtual public INoiseCaloCellsTool 
{
public:
  NoiseCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent);
  virtual ~NoiseCaloCellsTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  //Add noise to the cell 
  virtual void AddNoise(std::vector<fcc::CaloHit*>& aCells) final; 

private:
  //Value of cell noise (to be read from DB)
  double m_cellNoise;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  Rndm::Numbers m_gauss;
};

#endif /* RECONSTRUCTION_NOISECALOCELLSTOOL_H */
