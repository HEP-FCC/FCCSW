#ifndef RECCALORIMETER_CREATEFCCHHCALONEIGHBOURS_H
#define RECCALORIMETER_CREATEFCCHHCALONEIGHBOURS_H

// Gaudi
#include "GaudiKernel/Service.h"
#include "RecInterface/ICaloCreateMap.h"
class IGeoSvc;

/** @class CreateFCChhCaloNeighbours
 *
 *  Service building a map of neighbours for all existing cells in the geometry.
 *  The volumes for which the neighbour map is created can be either segmented in eta-phi (e.g. ECal inclined),
 *  or can contain nested volumes (e.g. HCal barrel).
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

  /// Names of the detector readout for volumes with eta-phi segmentation
  Gaudi::Property<std::vector<std::string>> m_readoutNamesSegmented{this, "readoutNamesPhiEta", {"ECalBarrelPhiEta"}};
  /// Name of the fields describing the segmented volume
  Gaudi::Property<std::vector<std::string>> m_fieldNamesSegmented{this, "systemNamesPhiEta", {"system"}};
  /// Values of the fields describing the segmented volume
  Gaudi::Property<std::vector<int>> m_fieldValuesSegmented{this, "systemValuesPhiEta", {5}};
  /// Names of the active volume in geometry along radial axis (e.g. layer), the others are "phi", "eta"
  Gaudi::Property<std::vector<std::string>> m_activeFieldNamesSegmented{this, "activeFieldNamesPhiEta", {"layer"}};
  /// Number of layers in the segmented volume
  Gaudi::Property<std::vector<unsigned int>> m_activeVolumesNumbersSegmented{this, "activeVolumesNumbers", {8}};
  // Radii of layers in the segmented volume
  Gaudi::Property<std::vector<double>> m_activeVolumesEta{this, "activeVolumesEta"};

  /// Names of the detector readout for volumes with nested volume structure and no segmentation
  Gaudi::Property<std::vector<std::string>> m_readoutNamesNested{this, "readoutNamesVolumes"};
  /// Name of the field describing the nested volume
  Gaudi::Property<std::string> m_fieldNameNested{this, "systemNameNested"};
  /// Values of the fields describing the nested volume
  Gaudi::Property<std::vector<int>> m_fieldValuesNested{this, "systemValuesNested"};
  /// Names of the active volume in geometry: along radial axis, azimuthal angle, and along z axis
  Gaudi::Property<std::vector<std::string>> m_activeFieldNamesNested{
      this, "activeFieldNamesNested"};
  /// Names of the nested volumes - to retrieve the number of active volumes, need to correspond to m_activeFieldNamesNested
  Gaudi::Property<std::vector<std::string>> m_activeVolumeNamesNested{
      this,
      "activeVolumeNamesNested",
      {"layerVolume", "moduleVolume", "wedgeVolume"}};  // to find out number of volumes
  /// Name of output file
  std::string m_outputFileName;

  // For combination of barrels: flag if ECal and HCal barrels should be merged
  Gaudi::Property<bool> m_connectBarrels{this, "connectBarrels", true};
  // For combination of barrels: size of HCal cell along z axis
  Gaudi::Property<double> m_hCalZSize{this, "hCalZsize", 18};
  // For combination of barrels: offset of HCal detector in z (lower edge)
  Gaudi::Property<double> m_hCalZOffset{this, "hCalZoffset", -4590};
  // For combination of barrels: HCal inner radius for calculation of eta from z
  Gaudi::Property<double> m_hCalRinner{this, "hCalRinner", 2850};
  // For combination of barrels: offset of HCal modules in phi (lower edge)
  Gaudi::Property<double> m_hCalPhiOffset{this, "hCalPhiOffset"};
};

#endif /* RECALORIMETER_CREATEFCCHHCALONEIGHBOURS_H */
