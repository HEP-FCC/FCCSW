#include "DD4hep/Factories.h"
#include "DD4hep/detail/SegmentationsInterna.h"

namespace {
template <typename T>
dd4hep::SegmentationObject* create_segmentation(const dd4hep::BitFieldCoder* decoder) {
  return new dd4hep::SegmentationWrapper<T>(decoder);
}
}

#include "DetSegmentation/GridEta.h"
DECLARE_SEGMENTATION(GridEta, create_segmentation<dd4hep::DDSegmentation::GridEta>)

#include "DetSegmentation/FCCSWGridPhiEta.h"
DECLARE_SEGMENTATION(FCCSWGridPhiEta, create_segmentation<dd4hep::DDSegmentation::FCCSWGridPhiEta>)

#include "DetSegmentation/GridTheta.h"
DECLARE_SEGMENTATION(GridTheta, create_segmentation<dd4hep::DDSegmentation::GridTheta>)

#include "DetSegmentation/FCCSWGridPhiTheta.h"
DECLARE_SEGMENTATION(FCCSWGridPhiTheta, create_segmentation<dd4hep::DDSegmentation::FCCSWGridPhiTheta>)

#include "DetSegmentation/GridRPhiEta.h"
DECLARE_SEGMENTATION(GridRPhiEta, create_segmentation<dd4hep::DDSegmentation::GridRPhiEta>)

#include "DetSegmentation/GridDriftChamber.h"
DECLARE_SEGMENTATION(GridDriftChamber, create_segmentation<dd4hep::DDSegmentation::GridDriftChamber>)

