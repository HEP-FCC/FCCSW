#ifndef DETSTUDIES_FilterSiliconEcalHits_H
#define DETSTUDIES_FilterSiliconEcalHits_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "TH2F.h"
#include "TTree.h"
#include "TF1.h"
#include "TProfile.h"

class IGeoSvc;

// datamodel
namespace fcc {
class PositionedCaloHitCollection;
}

class ITHistSvc;
/** @class DECalAnalysis DECalAnalysis.h
 *
 *  Histograms of energy deposited in active material and total energy deposited in the calorimeter.
 *  Passive material needs to be marked as sensitive. It needs to be divided into layers (cells) as active material.
 *  Layers (cells) are numbered starting at 1 so that energy depoisted in cryostat and bath could be easily recognised.
 *  Sampling fraction is calculated for each layer as the ratio of energy deposited in active material to energy
 *  deposited in the layer (also in passive material).
 *
 *  @author Anna Zaborowska
 */

class FilterSiliconEcalHits : public GaudiAlgorithm {
public:
  explicit FilterSiliconEcalHits(const std::string&, ISvcLocator*);
  virtual ~FilterSiliconEcalHits();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Fills the histograms.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;


private:
  /// Pointer to the interface of histogram service
  ServiceHandle<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;

  /// The Pixel analysis
  /// Handle for the energy deposits
  DataHandle<fcc::PositionedCaloHitCollection> m_deposits{"rec/deposits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::PositionedCaloHitCollection> m_filtered{"rec/filtered", Gaudi::DataHandle::Writer, this};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_padReadoutName{this, "readoutName", "", "Name of the detector readout"};

  Gaudi::Property<uint> m_digitalFlag{this, "digitalFlag", 0, "If 1 then filter for digital hits, 0 filter for analgoue hits"};

  int m_EvtNumber;

};
#endif /* DETSTUDIES_FilterSiliconEcalHits_H */
