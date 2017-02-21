#ifndef RECCALORIMETER_NOISECALOCELLSFROMFILETOOL_H
#define RECCALORIMETER_NOISECALOCELLSFROMFILETOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

// FCCSW
#include "RecInterface/INoiseCaloCellsTool.h"
#include "DetSegmentation/GridPhiEta.h"
class IGeoSvc;

// Root
class TH1F;

/** @class NoiseCaloCellsFromFileTool
 *
 *  Tool for calorimeter noise
 *  Access noise constants from TH1F histogram (noise vs. |eta|)
 *  createRandomCellNoise: Create random CaloHits (gaussian distribution) for the vector of cells
 *  filterCellNoise: remove cells with energy bellow threshold*sigma from the vector of cells
 *
 *  @author Jana Faltova
 *  @date   2016-09
 *
 */

class NoiseCaloCellsFromFileTool : public GaudiTool, virtual public INoiseCaloCellsTool {
public:
  NoiseCaloCellsFromFileTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~NoiseCaloCellsFromFileTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /** @brief Create random CaloHits (gaussian distribution) for the vector of cells (aCells).
   * Vector of cells must contain all cells in the calorimeter with their cellIDs.
   */
  virtual void addRandomCellNoise(std::unordered_map<uint64_t, double>& aCells) final;
  /** @brief Remove cells with energy bellow threshold*sigma from the vector of cells
   */
  virtual void filterCellNoise(std::unordered_map<uint64_t, double>& aCells) final;

  /// Open file and read noise histograms in the memory
  StatusCode initNoiseFromFile();
  /// Find the appropriate noise constant from the histogram
  double getNoiseConstantPerCell(int64_t aCellID);

private:
  /// Add pileup contribution to the electronics noise? (only if read from file)
  bool m_addPileup;
  /// Name of the file with noise constants
  std::string m_noiseFileName;
  /// Name of the detector readout
  std::string m_readoutName;
  /// Name of active layers for sampling calorimeter
  std::string m_activeFieldName;
  /// Name of pileup histogram
  std::string m_pileupHistoName;
  /// Name of electronics noise histogram
  std::string m_elecNoiseHistoName;
  /// Energy threshold (cells with Ecell < filterThreshold*m_cellNoise removed)
  double m_filterThreshold;
  /// Number of radial layers
  uint m_numRadialLayers;

  /// Histograms with pileup constants (index in array - radial layer)
  std::vector<TH1F> m_histoPileupConst;
  /// Histograms with electronics noise constants (index in array - radial layer)
  std::vector<TH1F> m_histoElecNoiseConst;

  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for the generation of random noise hits
  Rndm::Numbers m_gauss;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// PhiEta segmentation
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
};

#endif /* RECCALORIMETER_NOISECALOCELLSCOMPLEXTOOL_H */
