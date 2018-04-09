#ifndef SIMG4COMPONENTS_G4SAVECALHITS_H
#define SIMG4COMPONENTS_G4SAVECALHITS_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"
class IGeoSvc;

// datamodel
namespace fcc {
class PositionedCaloHitCollection;
class CaloHitCollection;
}

/** @class SimG4SaveCalHits SimG4Components/src/SimG4SaveCalHits.h SimG4SaveCalHits.h
 *
 *  Save calorimeter hits tool.
 *  All collections passed in the job options will be saved (\b'readoutNames').
 *  Readout name is defined in DD4hep XML file as the attribute 'readout' of 'detector' tag.
 *  If (\b'readoutNames') contain no elements or names that do not correspond to any hit collection,
 *  tool will fail at initialization.
 *  [For more information please see](@ref md_sim_doc_geant4fullsim).
 *
 *  @author Anna Zaborowska
 */

class SimG4SaveCalHits : public GaudiTool, virtual public ISimG4SaveOutputTool {
public:
  explicit SimG4SaveCalHits(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4SaveCalHits();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save the data output.
   *   Saves the calorimeter hits from the collections as specified in the job options in \b'readoutNames'.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Handle for calo hits with additional position information
  DataHandle<fcc::PositionedCaloHitCollection> m_positionedCaloHits{"hits/positionedCaloHits",
                                                                    Gaudi::DataHandle::Writer, this};
  /// Handle for calo hits
  DataHandle<fcc::CaloHitCollection> m_caloHits{"hits/caloHits", Gaudi::DataHandle::Writer, this};
  /// Name of the readouts (hits collections) to save
  Gaudi::Property<std::vector<std::string>> m_readoutNames{
      this, "readoutNames", {}, "Name of the readouts (hits collections) to save"};
};

#endif /* SIMG4COMPONENTS_G4SAVECALHITS_H */
