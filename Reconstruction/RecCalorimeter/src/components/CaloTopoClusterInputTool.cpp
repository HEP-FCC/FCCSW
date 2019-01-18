#include "CaloTopoClusterInputTool.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"

DECLARE_TOOL_FACTORY(CaloTopoClusterInputTool)

CaloTopoClusterInputTool::CaloTopoClusterInputTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareProperty("ecalBarrelCells", m_ecalBarrelCells, "");
  declareProperty("ecalEndcapCells", m_ecalEndcapCells, "");
  declareProperty("ecalFwdCells", m_ecalFwdCells, "");
  declareProperty("hcalBarrelCells", m_hcalBarrelCells, "");
  declareProperty("hcalExtBarrelCells", m_hcalExtBarrelCells, "");
  declareProperty("hcalEndcapCells", m_hcalEndcapCells, "");
  declareProperty("hcalFwdCells", m_hcalFwdCells, "");
  declareInterface<ITopoClusterInputTool>(this);
}

StatusCode CaloTopoClusterInputTool::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode CaloTopoClusterInputTool::finalize() { return GaudiTool::finalize(); }

StatusCode CaloTopoClusterInputTool::cellIdMap(std::map<uint64_t, double>& aCells) {
  aCells.clear();
  uint totalNumberOfCells = 0;

  // 1. ECAL barrel
  // Get the input collection with calorimeter cells
  const fcc::CaloHitCollection* ecalBarrelCells = m_ecalBarrelCells.get();
  debug() << "Input Ecal barrel cell collection size: " << ecalBarrelCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  for (const auto& iCell : *ecalBarrelCells) {
    aCells.emplace(iCell.cellId(), iCell.energy());
  }
  totalNumberOfCells += ecalBarrelCells->size();
  
  // 2. ECAL endcap calorimeter
  const fcc::CaloHitCollection* ecalEndcapCells = m_ecalEndcapCells.get();
  debug() << "Input Ecal endcap cell collection size: " << ecalEndcapCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  for (const auto& iCell : *ecalEndcapCells) {
    aCells.emplace(iCell.cellId(), iCell.energy());
  }
  totalNumberOfCells += ecalEndcapCells->size();
    
  // 3. ECAL forward calorimeter
  const fcc::CaloHitCollection* ecalFwdCells = m_ecalFwdCells.get();
  debug() << "Input Ecal forward cell collection size: " << ecalFwdCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  for (const auto& iCell : *ecalFwdCells) {
    aCells.emplace(iCell.cellId(), iCell.energy());
  }
  totalNumberOfCells += ecalFwdCells->size();
  
  // 4. HCAL barrel
  const fcc::CaloHitCollection* hcalBarrelCells = m_hcalBarrelCells.get();
  debug() << "Input hadronic barrel cell collection size: " << hcalBarrelCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  for (const auto& iCell : *hcalBarrelCells) {
    aCells.emplace(iCell.cellId(), iCell.energy());
  }
  totalNumberOfCells += hcalBarrelCells->size();
  
  // 5. HCAL extended barrel
  const fcc::CaloHitCollection* hcalExtBarrelCells = m_hcalExtBarrelCells.get();
  debug() << "Input hadronic extended barrel cell collection size: " << hcalExtBarrelCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  for (const auto& iCell : *hcalExtBarrelCells) {
    aCells.emplace(iCell.cellId(), iCell.energy());
  }
  totalNumberOfCells += hcalExtBarrelCells->size();
  
  // 6. HCAL endcap calorimeter                                                                                                              
  const fcc::CaloHitCollection* hcalEndcapCells = m_hcalEndcapCells.get();
  debug() << "Input Hcal endcap cell collection size: " << hcalEndcapCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  for (const auto& iCell : *hcalEndcapCells) {
    aCells.emplace(iCell.cellId(), iCell.energy());
  }
  totalNumberOfCells += hcalEndcapCells->size();
  
  // 7. HCAL forward calorimeter
  const fcc::CaloHitCollection* hcalFwdCells = m_hcalFwdCells.get();
  debug() << "Input Hcal forward cell collection size: " << hcalFwdCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  for (const auto& iCell : *hcalFwdCells) {
    aCells.emplace(iCell.cellId(), iCell.energy());
  }
  totalNumberOfCells += hcalFwdCells->size();
  
  if (totalNumberOfCells != aCells.size()){
    error() << "Map size != total number of cells! " << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
