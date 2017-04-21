#include "DD4hep/Factories.h"
#include "DD4hep/objects/SegmentationsInterna.h"

namespace {
template <typename T>
DD4hep::Geometry::SegmentationObject* create_segmentation(DD4hep::Geometry::BitField64* decoder) {
  return new DD4hep::Geometry::SegmentationWrapper<T>(decoder);
}
}

#include "DetSegmentation/GridEta.h"
DECLARE_SEGMENTATION(GridEta, create_segmentation<DD4hep::DDSegmentation::GridEta>)

#include "DetSegmentation/GridPhiEta.h"
DECLARE_SEGMENTATION(GridPhiEta, create_segmentation<DD4hep::DDSegmentation::GridPhiEta>)

#include "DetSegmentation/GridRPhiEta.h"
DECLARE_SEGMENTATION(GridRPhiEta, create_segmentation<DD4hep::DDSegmentation::GridRPhiEta>)
