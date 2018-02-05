#include "CellPositionsCaloDiscsTool.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

DECLARE_TOOL_FACTORY(CellPositionsCaloDiscsTool)

CellPositionsCaloDiscsTool::CellPositionsCaloDiscsTool(const std::string& type, const std::string& name,
                                                       const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICellPositionsTool>(this);
}

StatusCode CellPositionsCaloDiscsTool::initialize() {
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
    return StatusCode::FAILURE;
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

void CellPositionsCaloDiscsTool::getPositions(const fcc::CaloHitCollection& aCells,
                                              fcc::PositionedCaloHitCollection& outputColl) {
  debug() << "Input collection size : " << aCells.size() << endmsg;
  // Loop through cell collection
  for (const auto& cell : aCells) {
    auto outPos = CellPositionsCaloDiscsTool::xyzPosition(cell.core().cellId);

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

dd4hep::Position CellPositionsCaloDiscsTool::xyzPosition(const uint64_t& aCellId) const {
  double radius;
  m_decoder->setValue(aCellId);
  (*m_decoder)["phi"] = 0;
  (*m_decoder)["eta"] = 0;
  auto volumeId = m_decoder->getValue();
  dd4hep::Position outPos;
  auto detelement = m_volman.lookupDetElement(volumeId);
  const auto& transformMatrix = detelement.nominal().worldTransformation();  // m_volman.worldTransformation(volumeId);
  double outGlobal[3];
  double inLocal[] = {0, 0, 0};
  transformMatrix.LocalToMaster(inLocal, outGlobal);
  debug() << "Position of volume (mm) : \t" << outGlobal[0] / dd4hep::mm << "\t" << outGlobal[1] / dd4hep::mm << "\t"
          << outGlobal[2] / dd4hep::mm << endmsg;
  // radius calculated from segmenation + z postion of volumes
  auto inSeg = m_segmentation->position(aCellId);
  double eta = m_segmentation->eta(aCellId);
  radius = outGlobal[2] / std::sinh(eta);
  debug() << "Radius : " << radius << endmsg;
  outPos.SetCoordinates(inSeg.x() * radius, inSeg.y() * radius, outGlobal[2]);

  return outPos;
}

int CellPositionsCaloDiscsTool::layerId(const uint64_t& aCellId) {
  int layer;
  m_decoder->setValue(aCellId);
  layer = (*m_decoder)["layer"].value();
  return layer;
}

StatusCode CellPositionsCaloDiscsTool::finalize() { return GaudiTool::finalize(); }
