#include "CartesianGridXYZTool.h"

// DD4hep
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DDSegmentation/Segmentation.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

DECLARE_TOOL_FACTORY(CartesianGridXYZTool)

CartesianGridXYZTool::CartesianGridXYZTool(const std::string& aType, const std::string& aName, const IInterface* aParent) :
GaudiTool(aType, aName, aParent),
  m_segmentation(nullptr) {
  declareInterface<ISegmentationTool>(this);
  declareProperty("bitfield", m_decoderString);
  declareProperty("cellSizeX", m_cellSizeX = 1.*Gaudi::Units::mm);
  declareProperty("cellSizeY", m_cellSizeY = 1.*Gaudi::Units::mm);
  declareProperty("cellSizeZ", m_cellSizeZ = 1.*Gaudi::Units::mm);
}

CartesianGridXYZTool::~CartesianGridXYZTool() {}

StatusCode CartesianGridXYZTool::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  // create segmentation
  m_segmentation = std::unique_ptr<DD4hep::DDSegmentation::Segmentation>(
    new DD4hep::DDSegmentation::CartesianGridXYZ(m_decoderString));
  std::vector<DD4hep::DDSegmentation::SegmentationParameter*> params = m_segmentation->parameters();
  auto itParam = std::find_if(params.begin(), params.end(),
    [](DD4hep::DDSegmentation::SegmentationParameter* par){return bool(par->name()=="grid_size_x");});
  (*itParam)->setValue(std::to_string(m_cellSizeX/Gaudi::Units::cm));
  itParam = std::find_if(params.begin(), params.end(),
    [](DD4hep::DDSegmentation::SegmentationParameter* par){return bool(par->name()=="grid_size_y");});
  (*itParam)->setValue(std::to_string(m_cellSizeY/Gaudi::Units::cm));
  itParam = std::find_if(params.begin(), params.end(),
    [](DD4hep::DDSegmentation::SegmentationParameter* par){return bool(par->name()=="grid_size_z");});
  (*itParam)->setValue(std::to_string(m_cellSizeZ/Gaudi::Units::cm));
  m_segmentation->setParameters(params);
  // print control info
  info()<<"Segmentation "<<m_segmentation->type()<<endmsg;
  info()<<"cell size (X,Y,Z):  ("<<m_cellSizeX/Gaudi::Units::cm<<",\t"
        <<m_cellSizeY/Gaudi::Units::cm<<",\t"
        <<m_cellSizeZ/Gaudi::Units::cm<<") cm"<<endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CartesianGridXYZTool::finalize() {
  return GaudiTool::finalize();
}

std::unique_ptr<DD4hep::DDSegmentation::Segmentation> CartesianGridXYZTool::segmentation() {
  return std::move(m_segmentation);
}
