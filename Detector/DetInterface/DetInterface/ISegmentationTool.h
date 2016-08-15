#ifndef DETINTERFACE_ISEGMENTATIONTOOL_H
#define DETINTERFACE_ISEGMENTATIONTOOL_H

// Gaudi
#include  "GaudiKernel/IAlgTool.h"

// DD4hep
namespace DD4hep {
namespace DDSegmentation {
  class Segmentation;
}
}

/** @class ISegmentationTool DetInterface/DetInterface/ISegmentationTool.h ISegmentationTool.h
 *
 *  Interface to the DD4hep segmentation tool.
 *  Provides the segmentation.
 *  It is used to study a different segmentation than the one specified in the geometry description.
 *
 *  @author Anna Zaborowska
 */

class ISegmentationTool: virtual public IAlgTool {
public:
  DeclareInterfaceID(ISegmentationTool,1,0);

  /**  Get the segmentation.
   *   @return pointer to the segmentation.
   */
  virtual std::unique_ptr<DD4hep::DDSegmentation::Segmentation> segmentation() = 0;
};
#endif /* DETINTERFACE_ISEGMENTATIONTOOL_H */
