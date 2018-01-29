#ifndef RECCALORIMETER_CREATEFCCHHCALONEIGHBOURS_H
#define RECCALORIMETER_CREATEFCCHHCALONEIGHBOURS_H

// Gaudi
#include "GaudiKernel/Service.h"
#include "RecInterface/ICaloCreateMap.h"
class IGeoSvc;

/** @class CreateFCChhCaloNeighbours
 *
 *  Service building a map of neighbours for all existing cells in the geometry.
 *
 *  @author Anna Zaborowska
 */

class CreateFCChhCaloNeighbours : public extends1<Service, ICaloCreateMap> {
public:
  /// Standard constructor
  explicit CreateFCChhCaloNeighbours(const std::string& aName, ISvcLocator* aSL);
  /// Standard destructor
  virtual ~CreateFCChhCaloNeighbours();
  /**  Initialize the map creator service.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize the map creator service.
   *   @return status code
   */
  virtual StatusCode finalize() final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Names of the detector readout
  Gaudi::Property<std::vector<std::string>> m_readoutNamesSegmented{this, "readoutNamesPhiEta", {"ECalBarrelPhiEta"}};
  Gaudi::Property<std::vector<std::string>> m_readoutNamesNested{this, "readoutNamesVolumes", {"BarHCal_Readout"}};
  /// Name of the fields describing the segmented volume
  Gaudi::Property<std::vector<std::string>> m_fieldNamesSegmented{this, "systemNamesPhiEta", {"system"}};
  Gaudi::Property<std::string> m_fieldNameNested{this, "systemNameNested", "system"};
  /// Values of the fields describing the segmented volume
  Gaudi::Property<std::vector<int>> m_fieldValuesSegmented{this, "systemValuesPhiEta", {5}};
  Gaudi::Property<int> m_fieldValueNested{this, "systemValueNested", 6};
  /// Names of the active volume in geometry (e.g. layer)
  Gaudi::Property<std::vector<std::string>> m_activeFieldNamesSegmented{this, "activeFieldNamesPhiEta", {"layer"}};
  Gaudi::Property<std::vector<std::string>> m_activeFieldNamesNested{
      this, "activeFieldNamesNested", {"layer", "module", "row"}};
  /// Number of layers in geometry
  Gaudi::Property<std::vector<unsigned int>> m_activeVolumesNumbersSegmented{this, "activeVolumesNumbers", {8}};
  Gaudi::Property<std::vector<std::string>> m_activeVolumeNamesNested{
      this,
      "activeVolumeNamesNested",
      {"layerVolume", "moduleVolume", "wedgeVolume"}};  // to find out number of volumes
  Gaudi::Property<bool> m_connectBarrels{this, "connectBarrels", false};
  Gaudi::Property<double> m_hCalZSize{this, "hCalZsize", 19.2}; // 18
  Gaudi::Property<double> m_hCalZOffset{this, "hCalZoffset", -4896.0}; // -4590
  Gaudi::Property<double> m_hCalRinner{this, "hCalRinner", 1920}; // 2850
  Gaudi::Property<double> m_hCalPhiOffset{this, "hCalPhiOffset", - M_PI / 2.};
};

#endif /* RECALORIMETER_CREATEFCCHHCALONEIGHBOURS_H */
