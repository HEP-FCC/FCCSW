#include "CellPositionsEndcapTool.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

DECLARE_TOOL_FACTORY(CellPositionsEndcapTool)

CellPositionsEndcapTool::CellPositionsEndcapTool(const std::string& type, const std::string& name,
                                                 const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICellPositionsTool>(this);
  declareProperty("mergedLayers", m_mergedLayers, "array of merged layers");
  declareProperty("readoutName", m_readoutName);
}

StatusCode CellPositionsEndcapTool::initialize() {
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
  info() << "Layers' merged : " << m_mergedLayers << endmsg;
  return sc;
}

void CellPositionsEndcapTool::getPositions(const fcc::CaloHitCollection& aCells,
                                           fcc::PositionedCaloHitCollection& outputColl) {
  int layer, subsystem;
  double radius;
  DD4hep::Geometry::VolumeManager volman = m_geoSvc->lcdd()->volumeManager();

  debug() << "Input collection size : " << aCells.size() << endmsg;
  // Loop through cell collection
  for (const auto& cell : aCells) {
    auto cellid = cell.core().cellId;
    // layers have been merged
    // --> for new z position, use mean positons of the originally merged layers
    m_decoder->setValue(cellid);
    layer = (*m_decoder)["layer"].value();
    debug() << "new layer : " << layer << endmsg;
    int oldLayersMin = 0;
    for (int iLayer = 0; iLayer < layer; iLayer++) {
      oldLayersMin += m_mergedLayers[iLayer];
    }
    int oldLayersMax = oldLayersMin + m_mergedLayers[layer] - 1;
    (*m_decoder)["layer"] = oldLayersMin;
    auto minCellId = m_decoder->getValue();
    (*m_decoder)["layer"] = oldLayersMax;
    auto maxCellId = m_decoder->getValue();
    debug() << "old min layer : " << oldLayersMin << endmsg;
    debug() << "old max layer : " << oldLayersMax << endmsg;

    const auto& transformMatrixMin = volman.worldTransformation(minCellId);
    const auto& transformMatrixMax = volman.worldTransformation(maxCellId);
    double inLocal[3] = {0, 0, 0};
    double outGlobalMin[3];
    double outGlobalMax[3];
    transformMatrixMin.LocalToMaster(inLocal, outGlobalMin);
    transformMatrixMax.LocalToMaster(inLocal, outGlobalMax);
    double meanLayerPositionZ = (outGlobalMin[2] + outGlobalMax[2]) / 2.;

    auto inSeg = m_segmentation->position(cellid);
    double eta = m_segmentation->eta(cellid);
    radius = meanLayerPositionZ / std::sinh(eta);
    // global cartesian coordinates calculated from r,phi,eta, for r=1
    DD4hep::Geometry::Position outPos(inSeg.x() * radius, inSeg.y() * radius, meanLayerPositionZ);

    // for in negtive z positioned cells retrieve layer positions from the positive side
    // !!! THIS HAS TO BE CHANGED WITH UPDATED CALDISCS GEOMETRY !!!
    m_decoder->setValue(cellid);
    subsystem = (*m_decoder)["subsystem"].value();
    if (subsystem == 1) {
      (*m_decoder)["subsystem"] = 0;
      auto newCellid = m_decoder->getValue();
      auto negSeg = m_segmentation->position(newCellid);
      eta = m_segmentation->eta(newCellid);
      outPos.SetCoordinates(negSeg.x() * radius, negSeg.y() * radius, -meanLayerPositionZ);
    }

    auto edmPos = fcc::Point();
    edmPos.x = outPos.x() / dd4hep::mm;
    edmPos.y = outPos.y() / dd4hep::mm;
    edmPos.z = outPos.z() / dd4hep::mm;

    auto positionedHit = outputColl.create(edmPos, cell.core());

    // Debug information about cell position
    debug() << "Cell energy (GeV) : " << cell.core().energy << "\tcellID " << cellid << endmsg;
    debug() << "Position of cell (mm) : \t" << outPos.x() / dd4hep::mm << "\t" << outPos.y() / dd4hep::mm << "\t"
            << outPos.z() / dd4hep::mm << endmsg;
  }
  debug() << "Output positions collection size: " << outputColl.size() << endmsg;
}

StatusCode CellPositionsEndcapTool::finalize() { return GaudiTool::finalize(); }
