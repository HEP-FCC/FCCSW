#ifndef DETCOMMON_PHIETAGRIDTOOL_H
#define DETCOMMON_PHIETAGRIDTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "DetInterface/ISegmentationTool.h"

/** @class PhiEtaGridTool DetCommon/src/PhiEtaGridTool.h PhiEtaGridTool.h
 *
 *  Construction of the segmentation phi-eta.
 *  It is used to study a different segmentation than the one specified in the geometry description.
 *
 *  @author Anna Zaborowska
 */

class PhiEtaGridTool: public GaudiTool, virtual public ISegmentationTool {
public:
  explicit PhiEtaGridTool(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~PhiEtaGridTool();
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

#endif /* DETCOMMON_PHIETAGRIDTOOL_H */
