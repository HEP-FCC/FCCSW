#ifndef RECCALORIMETER_CALOTOPOCLUSTERINPUTTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITopoClusterInputTool.h"

class IGeoSvc;

// datamodel
namespace fcc {
  class CaloHit;
  class CaloHitCollection;
}

namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

/** @class CaloTopoClusterInputTool Reconstruction/RecCalorimeter/src/components/CaloTopoClusterInputTool.h
 * CaloTopoClusterInputTool.h
 *
 *  Tool filling a map of all Calo cells as input for TopoCluster algorithm.
 *  This tool runs over all calorimeter systems (ECAL barrel, HCAL barrel + extended barrel, calorimeter endcaps,
 * forward calorimeters). If not all systems are available or not wanted to be used, create an empty collection using
 * CreateDummyCellsCollection algorithm.
 *
 *  @author Coralie Neubueser
 */

class CaloTopoClusterInputTool : public GaudiTool, virtual public ITopoClusterInputTool {
public:
  CaloTopoClusterInputTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~CaloTopoClusterInputTool() = default;

  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;

  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

  /** cellIdMap
   * Fills the given map with all cellIDs pointing to the cells energy.
   *  @return status code
   */
  virtual StatusCode cellIdMap(std::map<uint64_t, double>& aCells) final;

private:
  /// Handle for electromagnetic barrel cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_ecalBarrelCells{"ecalBarrelCells", Gaudi::DataHandle::Reader, this};
  /// Handle for ecal endcap calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_ecalEndcapCells{"ecalEndcapCells", Gaudi::DataHandle::Reader, this};
  /// Handle for ecal forward calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_ecalFwdCells{"ecalFwdCells", Gaudi::DataHandle::Reader, this};
  /// Handle for hadronic barrel cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_hcalBarrelCells{"hcalBarrelCells", Gaudi::DataHandle::Reader, this};
  /// Handle for hadronic extended barrel cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_hcalExtBarrelCells{"hcalExtBarrelCells", Gaudi::DataHandle::Reader, this};
  /// Handle for hcal endcap calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_hcalEndcapCells{"hcalEndcapCells", Gaudi::DataHandle::Reader, this};
  /// Handle for hcal forward calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_hcalFwdCells{"hcalFwdCells", Gaudi::DataHandle::Reader, this};
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic barrel readout
  Gaudi::Property<std::string> m_ecalBarrelReadoutName{this, "ecalBarrelReadoutName", "",
                                                       "name of the ecal barrel readout"};
  /// Name of the ecal endcap calorimeter readout
  Gaudi::Property<std::string> m_ecalEndcapReadoutName{this, "ecalEndcapReadoutName", "",
                                                      "name of the ecal endcap readout"};
  /// Name of the ecal forward calorimeter readout
  Gaudi::Property<std::string> m_ecalFwdReadoutName{this, "ecalFwdReadoutName", "", "name of the ecal fwd readout"};
  /// Name of the hadronic barrel readout
  Gaudi::Property<std::string> m_hcalBarrelReadoutName{this, "hcalBarrelReadoutName", "",
                                                       "name of the hcal barrel readout"};
  /// Name of the hadronic extended barrel readout
  Gaudi::Property<std::string> m_hcalExtBarrelReadoutName{this, "hcalExtBarrelReadoutName", "",
                                                          "name of the hcal extended barrel readout"};
  /// Name of the hcal endcap calorimeter readout
  Gaudi::Property<std::string> m_hcalEndcapReadoutName{this, "hcalEndcapReadoutName", "",
                                                       "name of the hcal endcap readout"};
  /// Name of the hcal forward calorimeter readout
  Gaudi::Property<std::string> m_hcalFwdReadoutName{this, "hcalFwdReadoutName", "", 
                                                    "name of the hcal fwd readout"};
  /// Map to be filled
  std::map<uint64_t, double> m_inputMap; 
};

#endif /* RECCALORIMETER_CALOTOPOCLUSTERINPUTTOOL_H */
