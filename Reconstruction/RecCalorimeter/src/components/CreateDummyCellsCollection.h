#ifndef RECCALORIMETER_CREATEDUMMYCELLSCOLLECTION_H
#define RECCALORIMETER_CREATEDUMMYCELLSCOLLECTION_H

//FCCSW
#include "FWCore/DataHandle.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

//EDM
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

class IGeoSvc;

/** @class CreateDummyCellsCollection
 *
 *  Algorithm for creating empty calorimeter cells collection
 *  Use-case:
 *  Input for clustering algorithm are collections from ALL calorimeter systems (ECAL barrel, HCAL barrel+extended barrel, ECAL + HCAL endcaps, ECAL + HCAL forward calorimeter). If not all collections are available (e.g. running simulations with just a subset of calorimeters), the user is supposed to create dummy cell collections for the other calorimeters.
 *
 *  @author Jana Faltova
 *  @date   2017-08
 *
 */

class CreateDummyCellsCollection : public GaudiAlgorithm {

public:
  CreateDummyCellsCollection(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for the dummy calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_caloCells{"cells", Gaudi::DataHandle::Writer, this};
};

#endif /* RECCALORIMETER_CREATEDUMMYCELLSCOLLECTION_H */
