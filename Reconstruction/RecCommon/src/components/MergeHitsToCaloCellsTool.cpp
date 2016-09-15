#include "MergeHitsToCaloCellsTool.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

#include <vector>
#include <unordered_map>

// DD4hep
#include "DD4hep/LCDD.h"
#include "DDSegmentation/Segmentation.h"

DECLARE_TOOL_FACTORY(MergeHitsToCaloCellsTool)

MergeHitsToCaloCellsTool::MergeHitsToCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareInterface<IMergeHitsToCaloCellsTool>(this);

}

MergeHitsToCaloCellsTool::~MergeHitsToCaloCellsTool()
{
}

StatusCode MergeHitsToCaloCellsTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  info() << "MergeHitsToCaloCellsTool initialized" << endmsg;

  m_readoutName = "ECalHitsPhiEta";
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
  // Take readout, bitfield from GeoSvc
  m_decoder = std::unique_ptr<DD4hep::DDSegmentation::BitField64>(new DD4hep::DDSegmentation::BitField64(m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder()->fieldDescription()));
  if (m_decoder==nullptr) {
    info() << "Decoder is a nullptr!!!!" << endmsg; 
  }

  return sc;

}


void MergeHitsToCaloCellsTool::DoMerge(const fcc::CaloHitCollection& aHits, std::vector<fcc::CaloHit*>& aCells) {

  //map: cellID, index in the vector of cells
  std::unordered_map<uint64_t, unsigned> cellIdMap;

  unsigned index = 0;
  //Prepare a map
  for (const auto& acells : aCells) {
    int cellID = acells->Core().Cellid;
    cellIdMap[cellID] = index;
    //info() << "index " << index << " " << acells-aCells.at(0) << endmsg;
    index += 1;
  }

  for (const auto& ehit : aHits) {
    int cellID = ehit.Core().Cellid;
    auto it = cellIdMap.find(cellID);
    if (it != end(cellIdMap)) {
      aCells.at(it->second)->Core().Energy += ehit.Core().Energy;
      aCells.at(it->second)->Core().Time = ehit.Core().Time;
      aCells.at(it->second)->Core().Bits = ehit.Core().Bits;
    }
    else {
      m_decoder->setValue(cellID);
      info() << "CellID not foundl!!!! " << cellID << " " << m_decoder->valueString() << endmsg;    
    }
  }

}

StatusCode MergeHitsToCaloCellsTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  info() << "MergeHitsToCaloCellsTool finalized" << endmsg;
  return sc;
}
