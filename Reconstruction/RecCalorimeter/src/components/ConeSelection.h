#ifndef RECCALORIMETER_CONESELECTION_H
#define RECCALORIMETER_CONESELECTION_H

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ICellPositionsTool.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"
#include "datamodel/MCParticleCollection.h"

class IGeoSvc;

/** @class ConeSelection
 *
 *  Algorithm select cells within a cone around the generated particles.
 *
 *  @author Coralie Neubueser
 *  @date   2018-11
 *
 */

class ConeSelection : public GaudiAlgorithm {

public:
  ConeSelection(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for tool to get cell positions
  ToolHandle<ICellPositionsTool> m_cellPositionsTool{"CellPositionsTool", this};

  /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloHitCollection> m_cells{"cells", Gaudi::DataHandle::Reader, this};
  /// Handle for calo hits (input collection)
  DataHandle<fcc::MCParticleCollection> m_particles{"particles", Gaudi::DataHandle::Reader, this};
  /// Handle for calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_selCells{"selCells", Gaudi::DataHandle::Writer, this};
  /// Map of cell IDs (corresponding to DD4hep IDs) and energy
  std::unordered_map<uint64_t, double> m_cellsMap;

  Gaudi::Property<double> m_r{this, "radius", 0.4, "radius of selection cone"};
};

#endif /* RECCALORIMETER_CONESELECTION_H */
