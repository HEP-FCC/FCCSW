#ifndef RECCALORIMETER_CREATEPOSITIONEDHIT_H
#define RECCALORIMETER_CREATEPOSITIONEDHIT_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "DetSegmentation/GridPhiEta.h"
class IGeoSvc;

namespace fcc {
class CaloHitCollection;
class PositionedCaloHitCollection;
}

// DD4hep
#include "DD4hep/Readout.h"
namespace DD4hep {
namespace DDSegmentation {
  class Segmentation;
}
}

/** @class CreatePositionedHit
 *
 *  Loops through caloHits (energy, cellID) and saves information in caloClusters (energy, global position)
 *  Only for Tube shapes with PhiEta segmentation!
 *
 *  @author Jana Faltova
 *  @date   2016-09
 *
 */


class CreatePositionedHit : public GaudiAlgorithm
{
public:
  CreatePositionedHit(const std::string& name, ISvcLocator* svcLoc);

  ~CreatePositionedHit() = default;

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for calo cells (input collection with cellID)
  DataHandle<fcc::CaloHitCollection> m_caloCells;
  /// Handle for positioned hits (output collection)
  DataHandle<fcc::PositionedCaloHitCollection> m_caloPositionedHits;
  /// Name of the detector readout
  std::string m_readoutName;
  /// Name of active layers for sampling calorimeter
  std::string m_activeFieldName;
  ///Name of active volumes
  std::string m_activeVolumeName;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// PhiEta segmentation
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  /// Number of active layers
  unsigned int m_numLayers;
};

#endif /* RECCALORIMETER_CREATEPOSITIONEDHIT_H */
