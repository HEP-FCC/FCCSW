#ifndef RECCALORIMETER_CREATEFCCHHCALONEIGHBOURS_H
#define RECCALORIMETER_CREATEFCCHHCALONEIGHBOURS_H

// Gaudi
#include "GaudiKernel/Service.h"
#include "RecInterface/ICaloCreateMap.h"
#include "GaudiKernel/ToolHandle.h"
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
  Gaudi::Property<std::vector<std::string>> m_readoutNames{this, "readoutNames", {"ECalBarrelPhiEta"}};
  /// Name of the fields describing the segmented volume
  Gaudi::Property<std::vector<std::string>> m_fieldNames{this, "systemNames", {"system"}};
  /// Values of the fields describing the segmented volume
  Gaudi::Property<std::vector<int>> m_fieldValues{this, "systemValues",{5}};
  /// Names of the active volume in geometry (e.g. layer)
  Gaudi::Property<std::vector<std::string>> m_activeFieldNames{this, "activeFieldNames", {"layer"}};
  /// Number of layers in geometry
  Gaudi::Property<std::vector<unsigned int>> m_activeVolumesNumbers{this, "activeVolumesNumbers", {8}};
};

#endif /* RECALORIMETER_CREATEFCCHHCALONEIGHBOURS_H */
