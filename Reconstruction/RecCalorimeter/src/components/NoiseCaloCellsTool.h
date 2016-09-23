#ifndef RECCALORIMETER_NOISECALOCELLSTOOL_H
#define RECCALORIMETER_NOISECALOCELLSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

// FCCSW
#include "RecInterface/INoiseCaloCellsTool.h"

/** @class NoiseCaloCellsTool
 *
 *  Tool for calorimeter noise
 *  Energy units are MeV (calibrated to EM scale) --> cannot be merged directly with Geant4 energy deposits for sampling calorimeters!!!
 *  CreateRandomCellNoise: Create random CaloHits (gaussian distribution) for the vector of cells
 *  FilterCellNoise: remove cells with energy bellow threshold*sigma from the vector of cells
 *  Noise defined by a single value - sigma of the noise, same for each cell
 *  TODO: 
 *     - noise dependence on the size of the cell, position in eta & r
 *     - read noise from DB when available
 *
 *  @author Jana Faltova
 *  @date   2016-09
 *
 */

class NoiseCaloCellsTool : public GaudiTool, virtual public INoiseCaloCellsTool 
{
public:
  NoiseCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent);
  virtual ~NoiseCaloCellsTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /// Create random caloHits 
  virtual void CreateRandomCellNoise(std::vector<fcc::CaloHit*>& aCells) final; 
  /// Remove cells with energy bellow threshold
  virtual void FilterCellNoise(std::vector<fcc::CaloHit*>& aCells) final; 

private:
  /// Sigma of noise
  double m_cellNoise;
  /// Energy threshold (Ecell < filterThreshold*m_cellNoise removed)
  double m_filterThreshold;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  Rndm::Numbers m_gauss;
};

#endif /* RECCALORIMETER_NOISECALOCELLSTOOL_H */
