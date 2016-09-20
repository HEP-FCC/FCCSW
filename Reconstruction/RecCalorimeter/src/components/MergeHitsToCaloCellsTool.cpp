#include "MergeHitsToCaloCellsTool.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_TOOL_FACTORY(MergeHitsToCaloCellsTool)

MergeHitsToCaloCellsTool::MergeHitsToCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareProperty("readoutName", m_readoutName, "ECalHitsPhiEta");
  declareInterface<IMergeHitsToCaloCellsTool>(this);

}

MergeHitsToCaloCellsTool::~MergeHitsToCaloCellsTool()
{
}

StatusCode MergeHitsToCaloCellsTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if(m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_readoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }
  info()<<"Readout <<"<<m_readoutName<<">>"<<endmsg;

  return sc;

}


void MergeHitsToCaloCellsTool::DoMerge(const fcc::CaloHitCollection& aHits, std::vector<fcc::CaloHit*>& aCells) {

  //Prepare a map: cellID, index in the vector of cells
  std::unordered_map<uint64_t, unsigned> cellIdMap;
  unsigned index = 0;
  for (const auto& acells : aCells) {
    int cellID = acells->Core().Cellid;
    cellIdMap[cellID] = index;
    index += 1;
  }

  //Loop through all hits (aHits) and merge the energy in the prepared cells (aCells)
  //Time and Bits: not used, a copy of the last entry at the moment
  //If cellID is not found in the vector, write a message with all bitfields
  for (const auto& ehit : aHits) {
    uint64_t cellID = ehit.Core().Cellid;
    auto it = cellIdMap.find(cellID);
    if (it != end(cellIdMap)) {
      aCells.at(it->second)->Core().Energy += ehit.Core().Energy;
      aCells.at(it->second)->Core().Time = ehit.Core().Time;
      aCells.at(it->second)->Core().Bits = ehit.Core().Bits;
    }
    else {
      auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
      decoder->setValue(cellID);
      info() << "CellID not found!!!! " << cellID << " " << decoder->valueString() << endmsg;
    }
  }

}

StatusCode MergeHitsToCaloCellsTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}
