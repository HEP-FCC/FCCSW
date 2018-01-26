#ifndef RECCALORIMETER_CREATEFCCHHCALONEIGHBOURS_H
#define RECCALORIMETER_CREATEFCCHHCALONEIGHBOURS_H

// Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"
#include "RecInterface/ICaloCreateMap.h"
#include "RecInterface/ICaloReadNeighboursMap.h"
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
  /// Pointer to the tool service
  SmartIF<IToolSvc> m_toolSvc;
  /// Handle for the detector construction tool
  ToolHandle<ICaloReadNeighboursMap> m_tool{"tool", this, true};
  /// Names of the detector readout
  Gaudi::Property<std::vector<std::string>> m_readoutNamesSegmented{this, "readoutNamesPhiEta", {"ECalBarrelPhiEta"}};
  Gaudi::Property<std::vector<std::string>> m_readoutNamesNested{this, "readoutNamesVolumes", {"BarHCal_Readout"}};
  /// Name of the fields describing the segmented volume
  Gaudi::Property<std::vector<std::string>> m_fieldNamesSegmented{this, "systemNamesPhiEta", {"system"}};
  Gaudi::Property<std::vector<std::string>> m_fieldNamesNested{this, "systemNamesNested", {"system"}};
  /// Values of the fields describing the segmented volume
  Gaudi::Property<std::vector<int>> m_fieldValuesSegmented{this, "systemValuesPhiEta", {5}};
  Gaudi::Property<std::vector<int>> m_fieldValuesNested{this, "systemValuesNested", {6}};
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
};

#endif /* RECALORIMETER_CREATEFCCHHCALONEIGHBOURS_H */
