#ifndef TESTGEOMETRY_G4SAVETESTCALHITS_H
#define TESTGEOMETRY_G4SAVETESTCALHITS_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"

// datamodel
namespace fcc {
class CaloClusterCollection;
class CaloHitCollection;
class CaloClusterHitsAssociationCollection;
}

/** @class SimG4SaveTestCalHits TestGeometry/src/components/SimG4SaveTestCalHits.h SimG4SaveTestCalHits.h
 *
 *  Save calorimeter hits tool.
 *  It checks for the calorimeter hits collections that contain hits of type test::TestCalorimeterHit.
 *
 *  @author Anna Zaborowska
 */

class SimG4SaveTestCalHits: public GaudiTool, virtual public ISimG4SaveOutputTool {
public:
  explicit SimG4SaveTestCalHits(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~SimG4SaveTestCalHits();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save the data output.
   *   Saves the calorimeter hits and clusters from every hits collection associated with the event
   *   that has m_calType in its name.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) final;
private:
  /// Handle for calo clusters
  DataHandle<fcc::CaloClusterCollection> m_caloClusters;
  /// Handle for calo hits
  DataHandle<fcc::CaloHitCollection> m_caloHits;
  /// Name of the calorimeter type (ECal/HCal)
  std::string m_calType;

};

#endif /* TESTGEOMETRY_G4SAVETESTCALHITS_H */
