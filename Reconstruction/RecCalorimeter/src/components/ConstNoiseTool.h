#ifndef RECCALORIMETER_CONSTNOISETOOL_H
#define RECCALORIMETER_CONSTNOISETOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
class IRndmGenSvc;

// FCCSW
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "RecInterface/INoiseConstTool.h"
class IGeoSvc;

#include <map>

/** @class ConstNoiseTool
 *
 *  Tool to set noise constant for all cells in Calorimeters.
 *  By default, set to rough estimates from ATLAS, can be changed in arguments for each Calo sub-system.
 *  
 *  @author Coralie Neubueser
 *  @date   2018-01
 *
 */

class ConstNoiseTool : public GaudiTool, virtual public INoiseConstTool {
public:
  ConstNoiseTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ConstNoiseTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  /// Find the appropriate noise constant from the histogram
  double getNoiseConstantPerCell(uint64_t aCellID);
  double getNoiseOffsetPerCell(uint64_t aCellID);

private:
  std::map<uint,double> m_systemNoiseConstMap;
  std::map<uint,double> m_systemNoiseOffsetMap;

  /// Cell thresholds (1Sigma) in GeV:
  /// default values estimates for the LAr and TileCal from ATLAS Barrel: ATL-LARG-PUB-2008_002
  /// effective seed thresholds in topo-clustering of 7.5 and 11.5MeV in LAr and TileCal
  /// EM Barrel Calorimeter
  Gaudi::Property<double> m_ECalBThreshold{this, "ecalBarrelThreshold", 0.0075/4., "Cell threshold of EM Barrel Calorimeter in GeV"};
  /// EM Endcap Calorimeter
  Gaudi::Property<double> m_ECalECThreshold{this, "ecalEndcapThreshold", 0.0075/4., "Cell threshold of EM Endcap Calorimeter in GeV"};
  /// EM Forward Calorimeter
  Gaudi::Property<double> m_ECalFwdThreshold{this, "ecalFwdThreshold", 0.0075/4., "Cell threshold of EM Forward Calorimeter in GeV"};
  /// Hadron Barrel Calorimeter
  Gaudi::Property<double> m_HCalBThreshold{this, "hcalBarrelThreshold", 0.0115/4., "Cell threshold of Hadron Barrel Calorimeter in GeV"};
  /// Hadron Endcap Calorimeter
  Gaudi::Property<double> m_HCalECThreshold{this, "hcalEndcapThreshold", 0.0075/4., "Cell threshold of Hadron Endcap Calorimeter in GeV"};
  /// Hadron Forward Calorimeter
  Gaudi::Property<double> m_HCalFwdThreshold{this, "hcalFwdThreshold", 0.0075/4., "Cell threshold of Hadron Forward Calorimeter in GeV"};

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  // Decoder
  dd4hep::DDSegmentation::BitFieldCoder* m_decoder = new dd4hep::DDSegmentation::BitFieldCoder("system:4");

};

#endif /* RECCALORIMETER_CONSTNOISETOOL_H */
