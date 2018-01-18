#ifndef RECCALORIMETER_READNOISEFROMFILETOOL_H
#define RECCALORIMETER_READNOISEFROMFILETOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

// FCCSW
#include "DetSegmentation/GridPhiEta.h"
#include "RecInterface/IReadNoiseFileTool.h"
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

class ReadNoiseFromFileTool : public GaudiTool, virtual public IReadNoiseFileTool {
public:
  ReadNoiseFromFileTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ReadNoiseFromFileTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  /// Open file and read noise histograms in the memory
  StatusCode initNoiseFromFile();
  /// Find the appropriate noise constant from the histogram
  double getNoiseConstantPerCell(int64_t aCellID);

private:
  /// Add pileup contribution to the electronics noise? (only if read from file)
  Gaudi::Property<bool> m_addPileup{this, "addPileup", true,
                                    "Add pileup contribution to the electronics noise? (only if read from file)"};
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
  /// Energy threshold (cells with Ecell < filterThreshold*m_cellNoise removed)
  Gaudi::Property<double> m_filterThreshold{
      this, "filterNoiseThreshold", 3, " Energy threshold (cells with Ecell < filterThreshold*m_cellNoise removed)"};
  /// Number of radial layers
  Gaudi::Property<uint> m_numRadialLayers{this, "numRadialLayers", 3, "Number of radial layers"};

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

#endif /* RECCALORIMETER_READNOISEFROMFILETOOL_H */
