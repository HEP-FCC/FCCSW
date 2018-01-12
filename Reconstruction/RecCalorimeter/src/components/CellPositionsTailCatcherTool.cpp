#include "CellPositionsTailCatcherTool.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

DECLARE_TOOL_FACTORY(CellPositionsTailCatcherTool)

CellPositionsTailCatcherTool::CellPositionsTailCatcherTool(const std::string& type,
                                                           const std::string& name,
                                                           const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICellPositionsTool>(this);
  declareProperty("readoutName", m_readoutName);
  declareProperty("centralRadius", m_centralRadius);
}

StatusCode CellPositionsTailCatcherTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry service." << endmsg;
    return StatusCode::FAILURE;
  }
  // get PhiEta segmentation
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no phi-eta segmentation!!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Take readout bitfield decoder from GeoSvc
  m_decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
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

void CellPositionsTailCatcherTool::getPositions(const fcc::CaloHitCollection& aCells,
                                                fcc::PositionedCaloHitCollection& outputColl) {
  DD4hep::Geometry::VolumeManager volman = m_geoSvc->lcdd()->volumeManager();
  double radius;

  debug() << "Input collection size : " << aCells.size() << endmsg;
  // Loop through cell collection
  for (const auto& cell : aCells) {
    auto cellid = cell.core().cellId;
    const auto& transformMatrix = volman.worldTransformation(cellid);
    double outGlobal[3];
    double inLocal[] = {0, 0, 0};
    transformMatrix.LocalToMaster(inLocal, outGlobal);

    debug() << "Position of volume (mm) : \t" << outGlobal[0] / dd4hep::mm << "\t" << outGlobal[1] / dd4hep::mm << "\t"
            << outGlobal[2] / dd4hep::mm << endmsg;

    // radius calculated from segmenation + z postion of volumes
    auto inSeg = m_segmentation->position(cellid);
    double eta = m_segmentation->eta(cellid);
    radius = outGlobal[2] / std::sinh(eta);
    debug() << "Radius : " << radius << endmsg;
    DD4hep::Geometry::Position outSeg(inSeg.x() * radius, inSeg.y() * radius, outGlobal[2]);

    if (outGlobal[2] == 0) {     // central tail catcher
      radius = m_centralRadius;  // 901.5cm
      outSeg.SetCoordinates(inSeg.x() * radius, inSeg.y() * radius, inSeg.z() * radius);
    }

    auto edmPos = fcc::Point();
    edmPos.x = outSeg.x() / dd4hep::mm;
    edmPos.y = outSeg.y() / dd4hep::mm;
    edmPos.z = outSeg.z() / dd4hep::mm;

    auto positionedHit = outputColl.create(edmPos, cell.core());
    ;

    // Debug information about cell position
    debug() << "Cell energy (GeV) : " << cell.core().energy << "\tcellID " << cellid << endmsg;
    debug() << "Position of cell (mm) : \t" << outSeg.x() / dd4hep::mm << "\t" << outSeg.y() / dd4hep::mm << "\t"
            << outSeg.z() / dd4hep::mm << endmsg;
  }
  debug() << "Output positions collection size: " << outputColl.size() << endmsg;
}

StatusCode CellPositionsTailCatcherTool::finalize() { return GaudiTool::finalize(); }
