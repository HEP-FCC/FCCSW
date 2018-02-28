#include "FastHitFilterTool.h"


#include "datamodel/TrackHitCollection.h"




DECLARE_TOOL_FACTORY(FastHitFilterTool)

FastHitFilterTool::FastHitFilterTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IHitFilterTool>(this);

}

StatusCode FastHitFilterTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  return sc;
}

bool   FastHitFilterTool::filter(fcc::TrackHit hit) {

  
  if (m_systemId == 0 || m_systemId == 1) { // barrel 
    if (hit.core().cellId % 16 == m_systemId) {
      if ((hit.core().cellId >> 4) % 32 == m_layerId) {
        return true;
      }
    }
  } else { //endcap
    if (hit.core().cellId % 16 == m_systemId) {
      if ((hit.core().cellId >> 4) % 64 == m_layerId) {
        return true;
      }
    }


  }


      
 return false;

}

void FastHitFilterTool::setIds(unsigned int systemId, unsigned int layerId) {
  m_systemId = systemId;
  m_layerId = layerId;

}




StatusCode FastHitFilterTool::finalize() { return GaudiTool::finalize(); }
