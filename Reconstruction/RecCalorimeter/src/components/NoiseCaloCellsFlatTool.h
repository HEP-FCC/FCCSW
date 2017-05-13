#ifndef RECCALORIMETER_NOISECALOCELLSFLATTOOL_H
#define RECCALORIMETER_NOISECALOCELLSFLATTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

// FCCSW
#include "RecInterface/INoiseCaloCellsTool.h"

/** @class NoiseCaloCellsFlatTool
 *
 *  Very simple tool for calorimeter noise using a single noise value for all cells
 *  createRandomCellNoise: Create random CaloHits (gaussian distribution) for the vector of cells
 *  filterCellNoise: remove cells with energy bellow threshold*sigma from the vector of cells
 *
 *  @author Jana Faltova
 *  @date   2016-09
 *
 */

class NoiseCaloCellsFlatTool : public GaudiTool, virtual public INoiseCaloCellsTool {
public:
  NoiseCaloCellsFlatTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~NoiseCaloCellsFlatTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /** @brief Create random CaloHits (gaussian distribution) for the vector of cells (aCells).
   * Vector of cells must contain all cells in the calorimeter with their cellIDs.
   */
  virtual void addRandomCellNoise(std::unordered_map<uint64_t, double>& aCells) final;
  /** @brief Remove cells with energy bellow threshold*sigma from the vector of cells
   */
  virtual void filterCellNoise(std::unordered_map<uint64_t, double>& aCells) final;

private:
  /// Sigma of noise -- uniform noise per cell in GeV
  Gaudi::Property<double> m_cellNoise{this, "cellNoise", 0.003, "uniform noise per cell in GeV"};
  /// Energy threshold (Ecell < filterThreshold*m_cellNoise removed)
  Gaudi::Property<double> m_filterThreshold{
      this, "filterNoiseThreshold", 3,
      "remove cells with energy bellow filterThreshold (threshold is multiplied by a cell noise sigma)"};
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
  Rndm::Numbers m_gauss;
};

#endif /* RECCALORIMETER_NOISECALOCELLSFLATTOOL_H */
