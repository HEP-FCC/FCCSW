#ifndef RECCALORIMETER_PREPAREPILEUP_H
#define RECCALORIMETER_PREPAREPILEUP_H

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ICalorimeterTool.h"
#include "RecInterface/ITowerTool.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"
class IGeoSvc;

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// EDM
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

class TH2F;
class TH1F;
class ITHistSvc;

/** @class PreparePileup
 *
 *  Algorithm for calculating pileup noise per cell.
 *  Tube geometry with PhiEta segmentation expected.
 *
 *
 *  Flow of the program:
 *  1/ Create a map of empty cells (m_geoTool)
 *  2/ Merge pileup signal events with empty cells
 *  3/ Fill histograms - energy, |eta| in TH2 histogram
 *
 *  Tools called:
 *    - TubeLayerPhiEtaCaloTool
 *
 *  @author Jana Faltova, Anna Zaborowska
 *  @date   2018-01
 *
 */

class PreparePileup : public GaudiAlgorithm {

public:
  PreparePileup(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

  dd4hep::DDSegmentation::BitField64* m_decoder;
  
private:
  /**  Correct way to access the neighbour of the phi tower, taking into account the full coverage in phi.
   *   Full coverage means that first tower in phi, with ID = 0 is a direct neighbour
   *   of the last tower in phi with ID = m_nPhiTower - 1).
   *   @param[in] aIPhi requested ID of a phi tower, may be < 0 or >= m_nPhiTower
   *   @return  ID of a tower - shifted and corrected (in [0, m_nPhiTower) range)
   */
  unsigned int phiNeighbour(int aIPhi) const;
  /// Handle for geometry tool (used to prepare map of all existing cellIDs for the system)
  ToolHandle<ICalorimeterTool> m_geoTool{"TubeLayerPhiEtaCaloTool", this};
  /// Handle for the tower building tool
  ToolHandle<ITowerTool> m_towerTool;
  // calorimeter towers
  std::vector<std::vector<float>> m_towers;
  /// number of towers in eta (calculated from m_deltaEtaTower and the eta size of the first layer)
  int m_nEtaTower;
  /// Number of towers in phi (calculated from m_deltaPhiTower)
  int m_nPhiTower;

  /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloHitCollection> m_hits{"hits", Gaudi::DataHandle::Reader, this};

  /// Map of cell IDs (corresponding to DD4hep IDs) and energy
  std::unordered_map<uint64_t, double> m_cellsMap;
  /// Sum of energy in each cell per file
  std::unordered_map<uint64_t, double> m_sumEnergyCellsMap;

  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// 2D histogram with abs(eta) on x-axis and energy per cell per event on y-axis
  std::vector<TH2F*> m_energyVsAbsEta;
  /// 2D histogram with abs(eta) on x-axis and energy per cell per file on y-axis
  std::vector<TH2F*> m_energyAllEventsVsAbsEta;
  /// 2D histogram with abs(eta) on x-axis and energy per cluster(s) per event on y-axis
  std::vector<TH2F*> m_energyVsAbsEtaClusters;
  /// 2D histogram with abs(eta) on x-axis and energy per cluster(s) per file on y-axis
  std::vector<TH2F*> m_energyAllEventsVsAbsEtaClusters;

  /// Maximum energy in the m_energyVsAbsEta histogram, in GeV 
  Gaudi::Property<uint> m_maxEnergy{this, "maxEnergy", 20., "Maximum energy in the pile-up plot"};
  /// Name of the pileup histograms
  Gaudi::Property<std::string> m_histogramName{this, "histogramName", "energyVsAbsEta", "Name of 2D histogram"};
  /// Name of the layer field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "layer", "Name of layer"};
  /// Number of layers
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 8, "Number of layers"};
   /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the readout"};
  /// Number of layersSize of cluster(s) in eta
  Gaudi::Property<std::vector<uint>> m_etaSizes{this, "etaSize", {7}, "Size of cluster(s) in eta"};
  /// Number of layersSize of cluster(s) in eta
  Gaudi::Property<std::vector<uint>> m_phiSizes{this, "phiSize", {17}, "Size of cluster(s) in phi"};
  /// PhiEta segmentation (owned by DD4hep)
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_segmentation;
};

#endif /* RECCALORIMETER_PREPAREPILEUP_H */
