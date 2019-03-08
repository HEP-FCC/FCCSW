#include "CellPositionsHCalBarrelPhiSegTool.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

DECLARE_TOOL_FACTORY(CellPositionsHCalBarrelPhiSegTool)

CellPositionsHCalBarrelPhiSegTool::CellPositionsHCalBarrelPhiSegTool(const std::string& type, const std::string& name,
                                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICellPositionsTool>(this);
}

StatusCode CellPositionsHCalBarrelPhiSegTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry service." << endmsg;
    return StatusCode::FAILURE;
  }
  // get PhiEta segmentation                                                                                                                                   
  m_segmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
									  m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no phi-eta segmentation!!!!" << endmsg;
  }
  // Take readout bitfield decoder from GeoSvc
  m_decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  m_volman = m_geoSvc->lcdd()->volumeManager();
  // check if decoder contains "layer"
  std::vector<std::string> fields;
  for (uint itField = 0; itField < m_decoder->size(); itField++) {
    fields.push_back((*m_decoder)[itField].name());
  }
  auto iter = std::find(fields.begin(), fields.end(), "layer");
  if (iter == fields.end()) {
    error() << "Readout does not contain field: 'layer'" << endmsg;
  }
  return sc;
}

void CellPositionsHCalBarrelPhiSegTool::getPositions(const fcc::CaloHitCollection& aCells,
                                                    fcc::PositionedCaloHitCollection& outputColl) {
  debug() << "Input collection size : " << aCells.size() << endmsg;
  // Loop through cell collection
  for (const auto& cell : aCells) {
    auto outPos = CellPositionsHCalBarrelPhiSegTool::xyzPosition(cell.core().cellId);

    auto edmPos = fcc::Point();
    edmPos.x = outPos.x() / dd4hep::mm;
    edmPos.y = outPos.y() / dd4hep::mm;
    edmPos.z = outPos.z() / dd4hep::mm;

    auto positionedHit = outputColl.create(edmPos, cell.core());

    // Debug information about cell position
    debug() << "Cell energy (GeV) : " << cell.core().energy << "\tcellID " << cell.core().cellId << endmsg;
    debug() << "Position of cell (mm) : \t" << outPos.x() / dd4hep::mm << "\t" << outPos.y() / dd4hep::mm << "\t"
            << outPos.z() / dd4hep::mm << endmsg;
  }
  debug() << "Output positions collection size: " << outputColl.size() << endmsg;
}

dd4hep::Position CellPositionsHCalBarrelPhiSegTool::xyzPosition(const uint64_t& aCellId) const {
  // global cartesian coordinates calculated from r,phi,eta, for r=1
  dd4hep::DDSegmentation::CellID volumeId = aCellId;
  m_decoder->set(volumeId, "phi", 0);
  m_decoder->set(volumeId, "eta", 0);
  int layer = m_decoder->get(volumeId, "layer");

  auto detelement = m_volman.lookupDetElement(volumeId);
  const auto& transform = detelement.nominal().worldTransformation();
  double global[3];
  double local[3] = {0, 0, 0};
  transform.LocalToMaster(local, global);

  auto inSeg = m_segmentation->position(aCellId);
  // get radius in cm
  double radius = m_radii[layer];  
  dd4hep::Position outSeg(inSeg.x()*radius, inSeg.y()*radius, global[2]);

  return outSeg;
}

int CellPositionsHCalBarrelPhiSegTool::layerId(const uint64_t& aCellId) {
  int layer;
  dd4hep::DDSegmentation::CellID cID = aCellId;
  layer = m_decoder->get(cID, "layer");
  return layer;
}

StatusCode CellPositionsHCalBarrelPhiSegTool::finalize() { return GaudiTool::finalize(); }
