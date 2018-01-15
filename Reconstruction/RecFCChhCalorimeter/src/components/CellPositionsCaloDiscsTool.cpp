#include "CellPositionsCaloDiscsTool.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

DECLARE_TOOL_FACTORY(CellPositionsCaloDiscsTool)

CellPositionsCaloDiscsTool::CellPositionsCaloDiscsTool(const std::string& type, const std::string& name,
                                                       const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICellPositionsTool>(this);
  declareProperty("mergedLayers", m_mergedLayers, "array of merged layers");
  declareProperty("readoutName", m_readoutName);
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

void CellPositionsCaloDiscsTool::getPositions(const fcc::CaloHitCollection& aCells,
                                              fcc::PositionedCaloHitCollection& outputColl) {
  int layer, subsystem;
  double radius;
  DD4hep::Geometry::VolumeManager volman = m_geoSvc->lcdd()->volumeManager();

  debug() << "Input collection size : " << aCells.size() << endmsg;
  // Loop through cell collection
  for (const auto& cell : aCells) {
    auto cellid = cell.core().cellId;
    m_decoder->setValue(cellid);
    subsystem = (*m_decoder)["subsystem"].value();
    // for in negtive z positioned cells retrieve cellIds on positive side
    // !!! THIS HAS TO BE CHANGED WITH UPDATED CALDISCS GEOMETRY !!!
    if (subsystem == 1) {
      (*m_decoder)["subsystem"] = 0;
      cellid = m_decoder->getValue();
    }
    DD4hep::Geometry::Position outPos;

    // Check if layers have been merged
    if (m_mergedLayers.size() == 0) {
      const auto& transformMatrix = volman.worldTransformation(cellid);
      double outGlobal[3];
      double inLocal[] = {0, 0, 0};
      transformMatrix.LocalToMaster(inLocal, outGlobal);
      debug() << "Position of volume (mm) : \t" << outGlobal[0] / dd4hep::mm << "\t" << outGlobal[1] / dd4hep::mm
              << "\t" << outGlobal[2] / dd4hep::mm << endmsg;
      // radius calculated from segmenation + z postion of volumes
      auto inSeg = m_segmentation->position(cellid);
      double eta = m_segmentation->eta(cellid);
      radius = outGlobal[2] / std::sinh(eta);
      debug() << "Radius : " << radius << endmsg;
      outPos.SetCoordinates(inSeg.x() * radius, inSeg.y() * radius, outGlobal[2]);
      // !!! THIS HAS TO BE CHANGED WITH UPDATED CALDISCS GEOMETRY !!!
      if (subsystem == 1) {
        outPos.SetCoordinates(inSeg.x() * radius, inSeg.y() * radius, -outGlobal[2]);
      }
    } else {
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
      outPos.SetCoordinates(inSeg.x() * radius, inSeg.y() * radius, meanLayerPositionZ);
      // !!! THIS HAS TO BE CHANGED WITH UPDATED CALDISCS GEOMETRY !!!
      if (subsystem == 1) {
        outPos.SetCoordinates(inSeg.x() * radius, inSeg.y() * radius, -meanLayerPositionZ);
      }
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

StatusCode CellPositionsCaloDiscsTool::finalize() { return GaudiTool::finalize(); }
