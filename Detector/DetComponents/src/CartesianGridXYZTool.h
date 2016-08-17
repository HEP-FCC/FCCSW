#ifndef DETCOMPONENTS_CARTESIANGRIDXYZTOOL_H
#define DETCOMPONENTS_CARTESIANGRIDXYZTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "DetInterface/ISegmentationTool.h"

/** @class CartesianGridXYZTool DetComponents/src/CartesianGridXYZTool.h CartesianGridXYZTool.h
 *
 *  Construction of the segmentation x-y-z.
 *  It is used to study a different segmentation than the one specified in the geometry description.
 *
 *  For an example see Detector/DetComponents/tests/options/redoSegmentationXYZ.py.
 *
 *  @author Anna Zaborowska
 */

class CartesianGridXYZTool: public GaudiTool, virtual public ISegmentationTool {
  public:
  explicit CartesianGridXYZTool(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~CartesianGridXYZTool();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Get the segmentation.
   *   @return pointer to the segmentation.
   */
  virtual std::unique_ptr<DD4hep::DDSegmentation::Segmentation> segmentation() final;
private:
  /// New segmentation
  std::unique_ptr<DD4hep::DDSegmentation::Segmentation> m_segmentation;
  /// Bitfield description, including the segmentation fields
  std::string m_decoderString;
  /// Size of the cell in x
  double m_cellSizeX;
  /// Size of the cell in y
  double m_cellSizeY;
  /// Size of the cell in z
  double m_cellSizeZ;
};

#endif /* DETCOMPONENTS_CARTESIANGRIDXYZTOOL_H */
