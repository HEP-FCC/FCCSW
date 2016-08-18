#ifndef DETCOMPONENTS_GRIDPHIETATOOL_H
#define DETCOMPONENTS_GRIDPHIETATOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "DetInterface/ISegmentationTool.h"

/** @class GridPhiEtaTool DetComponents/src/GridPhiEtaTool.h GridPhiEtaTool.h
 *
 *  Construction of the segmentation phi-eta.
 *  It is used to study a different segmentation than the one specified in the geometry description.
 *
 *  For an example see Detector/DetComponents/tests/options/redoSegmentationRPhi.py.
 *
 *  @author Anna Zaborowska
 */

class GridPhiEtaTool: public GaudiTool, virtual public ISegmentationTool {
public:
  explicit GridPhiEtaTool(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~GridPhiEtaTool();
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
  /// Size of the cell in eta
  double m_cellSizeEta;
  /// Number of cells in phi
  double m_cellNumPhi;
};

#endif /* DETCOMPONENTS_GRIDPHIETATOOL_H */
