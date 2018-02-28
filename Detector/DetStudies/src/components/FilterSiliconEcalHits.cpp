
#include "FilterSiliconEcalHits.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/CaloHitCollection.h"
#include "datamodel/MCParticleCollection.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"
#include "TVector2.h"
#include "TMath.h"
#include "TString.h"
#include "string"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_ALGORITHM_FACTORY(FilterSiliconEcalHits)

FilterSiliconEcalHits::FilterSiliconEcalHits(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc),
      m_histSvc("THistSvc", "FilterSiliconEcalHits"),
      m_geoSvc("GeoSvc", "FilterSiliconEcalHits")
{
  declareProperty("deposits", m_deposits, "Energy deposits in sampling calorimeter (input)");
  declareProperty("filtered", m_filtered, "Filtered energy deposits (output)");

}
FilterSiliconEcalHits::~FilterSiliconEcalHits() {}

StatusCode FilterSiliconEcalHits::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_padReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_padReadoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  
  // if it gets to here then it has registered all histograms
  return StatusCode::SUCCESS;
}

StatusCode FilterSiliconEcalHits::execute() {
  // get the decoder for the readout 
  // this will allow to extract layers etc.
  auto decoder = m_geoSvc->lcdd()->readout(m_padReadoutName).idSpec().decoder();

  // added a little print out to ensure it is still working
  if(m_EvtNumber < 10 or m_EvtNumber % 100 == 0) {
    info() << "Event Number " << m_EvtNumber << endmsg;
  }


  // set some variables to hold information for an event
  int nFilteredHits = 0;

  const auto deposits = m_deposits.get();
  auto filtered = m_filtered.createAndPut();

  // if looking at digital need to threshold at this point so the merging of cells takes correct deposits
  // the analgoue we can keep all hits and sum at a later date
  // eventually this will be passed as a parameter but coding quickly now
  double m_threshold = 0;
  if(m_digitalFlag==1) {
	m_threshold = 480*0.0000000036;
  }

  for (const auto& hit : *deposits) {
    decoder->setValue(hit.core().cellId);
    int digital = (*decoder)["digital"];
    if(digital==m_digitalFlag) {
      nFilteredHits++;

      if(hit.energy()>m_threshold) { 
        fcc::PositionedCaloHit filteredHit = filtered->create();
        filteredHit.energy(hit.energy());
        filteredHit.time(hit.time());
        filteredHit.cellId(hit.cellId());
        filteredHit.bits(hit.bits());
        filteredHit.position(hit.position());
      }
    }
    
  }

  info() << "nFilteredHits = " << nFilteredHits << endmsg;

  m_EvtNumber++;

  return StatusCode::SUCCESS;
}

StatusCode FilterSiliconEcalHits::finalize() { 
  debug() << "StatusCode FilterSiliconEcalHits::finalize()" << endmsg;


return GaudiAlgorithm::finalize(); }


