#ifndef RECCALORIMETER_READNOISEFROMFILETOOL_H
#define RECCALORIMETER_READNOISEFROMFILETOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "RecInterface/ICalorimeterTool.h"
#include "RecInterface/INoiseConstTool.h"
#include "RecInterface/ICellPositionsTool.h"

class IGeoSvc;

// Root
class TH1F;

/** @class ReadNoiseFromFileTool
 *
 *  Tool to read the stored noise constant per cell in the calorimeters
 *  Access noise constants from TH1F histogram (noise vs. |eta|)
 *
 *  @author Jana Faltova, Coralie Neubueser
 *  @date   2018-01
 *
 */

class ReadNoiseFromFileTool : public GaudiTool, virtual public INoiseConstTool {
public:
  ReadNoiseFromFileTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ReadNoiseFromFileTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  /// Open file and read noise histograms in the memory
  StatusCode initNoiseFromFile();
  /// Find the appropriate noise constant from the histogram
  double getNoiseConstantPerCell(uint64_t aCellID);
  double getNoiseOffsetPerCell(uint64_t aCellID);

private:
  /// Handle for tool to get positions in ECal Barrel
  ToolHandle<ICellPositionsTool> m_cellPositionsTool{"CellPositionsTool", this};

  /// Add pileup contribution to the electronics noise? (only if read from file)
  Gaudi::Property<bool> m_addPileup{this, "addPileup", true,
                                    "Add pileup contribution to the electronics noise? (only if read from file)"};
  /// Noise offset, if false, mean is set to 0
  Gaudi::Property<bool> m_setNoiseOffset{this, "setNoiseOffset", true, "Set a noise offset per cell"};

  /// Name of the file with noise constants
  Gaudi::Property<std::string> m_noiseFileName{this, "noiseFileName", "", "Name of the file with noise constants"};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "ECalHitsPhiEta", "Name of the detector readout"};
  /// Name of active layers for sampling calorimeter
  Gaudi::Property<std::string> m_activeFieldName{this, "activeFieldName", "active_layer",
                                                 "Name of active layers for sampling calorimeter"};
  /// Name of pileup histogram
  Gaudi::Property<std::string> m_pileupHistoName{this, "pileupHistoName", "h_pileup_layer", "Name of pileup histogram"};
  /// Name of electronics noise histogram
  Gaudi::Property<std::string> m_elecNoiseHistoName{this, "elecNoiseHistoName", "h_elecNoise_layer",
                                                    "Name of electronics noise histogram"};
  /// Name of electronics noise offset histogram
  Gaudi::Property<std::string> m_elecNoiseOffsetHistoName{this, "elecNoiseOffsetHistoName", "h_mean_pileup_layer",
                                                    "Name of electronics noise offset histogram"};
  /// Name of pileup offset histogram
  Gaudi::Property<std::string> m_pileupOffsetHistoName{this, "pileupOffsetHistoName", "h_pileup_layer", "Name of pileup offset histogram"};

  /// Number of radial layers
  Gaudi::Property<uint> m_numRadialLayers{this, "numRadialLayers", 3, "Number of radial layers"};

  /// Histograms with pileup constants (index in array - radial layer)
  std::vector<TH1F> m_histoPileupConst;
  /// Histograms with electronics noise constants (index in array - radial layer)
  std::vector<TH1F> m_histoElecNoiseConst;

  /// Histograms with pileup offset (index in array - radial layer)
  std::vector<TH1F> m_histoPileupOffset;
  /// Histograms with electronics noise offset (index in array - radial layer)
  std::vector<TH1F> m_histoElecNoiseOffset;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// PhiEta segmentation
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_segmentation;
  // Decoder
  dd4hep::DDSegmentation::BitField64* m_decoder;

};

#endif /* RECCALORIMETER_READNOISEFROMFILETOOL_H */
