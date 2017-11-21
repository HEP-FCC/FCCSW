
#include "DECalLongitudinalTest.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/CaloHitCollection.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"
#include "TVector2.h"
#include "TMath.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_ALGORITHM_FACTORY(DECalLongitudinalTest)

DECalLongitudinalTest::DECalLongitudinalTest(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc),
      m_histSvc("THistSvc", "DECalLongitudinalTest"),
      m_geoSvc("GeoSvc", "DECalLongitudinalTest")
{
  declareProperty("deposits", m_deposits, "Energy deposits in sampling calorimeter (input)");
}
DECalLongitudinalTest::~DECalLongitudinalTest() {}

StatusCode DECalLongitudinalTest::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  // create histograms
  m_totalHits = new TH1F("decal_totalHits", "Total hits in an event", 15000, 0, 150000);
  if (m_histSvc->regHist("/rec/decal_hits", m_totalHits).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totalHitsVsLayer = new TH1F("decal_totalHitsVsLayer", "Total hits per layer", m_numLayers+1, -0.5, m_numLayers+0.5);
  m_totalHitsVsLayer->Sumw2(kFALSE);
  if(m_histSvc->regHist("/rec/decal_hitsVsLayer", m_totalHitsVsLayer).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  for (uint i = 0; i < m_numLayers; i++) {
    m_totalHitsLayers.push_back(new TH1F(("decal_Hits_layer" + std::to_string(i)).c_str(),
                                       ("Total pixel hits in layer " + std::to_string(i)).c_str(), 1000, 0,
                                       10000));
    if (m_histSvc->regHist("/rec/decal_total_hits_layer" + std::to_string(i), m_totalHitsLayers.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  m_XYEvent0 = new TH2F("decal_XYEvent0", "XY Positions for Event 0", 5000, -2500, 2500, 5000, -2500, 2500);
  if (m_histSvc->regHist("/rec/decal_XYEvent0", m_XYEvent0).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }

  //m_EtaPhiEvent0 = new TH2F("decal_EtaPhiEvent0", "Eta Phi Positions for Event 0", 222, -0.004, 0.002, 222, -0.8305,-0.8245);
  m_EtaPhiEvent0 = new TH2F("decal_EtaPhiEvent0", "Eta Phi Positions for Event 0", 2000, -0.04, 0.02, 2000, -4,4);
  if (m_histSvc->regHist("/rec/decal_EtaPhiEvent0", m_EtaPhiEvent0).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }

  m_EtaPhiSeparation = new TH1F("decal_EtaSeparation", "Eta Phi Positions", 1200, 0, 1);
  if (m_histSvc->regHist("/rec/decal_EtaPhiSeparation", m_EtaPhiSeparation).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }

  
  
  // if it gets to here then it has registered all histograms
  return StatusCode::SUCCESS;
}

StatusCode DECalLongitudinalTest::execute() {
  // get the decoder for the readout 
  // this will allow to extract layers etc.
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();

  // set some variables to hold information for an event
  double sumHits = 0.;
  std::vector<double> sumHitslayers;
  sumHitslayers.assign(m_numLayers, 0);

  bool fillXY = true;
  if (m_XYEvent0->GetEntries()>0) {
    fillXY = false;
  }

  const auto deposits = m_deposits.get();
  for (const auto& hit : *deposits) {
    decoder->setValue(hit.core().cellId);
    //debug() << "(*decoder)[m_layerFieldName] = " << (*decoder)[m_layerFieldName] << endmsg;
    sumHitslayers[(*decoder)[m_layerFieldName]] += 1; //hit.core().energy;
    // check if energy was deposited in the calorimeter (active/passive material)
    // layers are numbered starting from 1, layer == 0 is cryostat/bath
    std::cout << (*decoder)[m_layerFieldName] << std::endl;
    if ((*decoder)[m_layerFieldName] > 0) {
      sumHits++;
      decoder->setValue(hit.core().cellId);     
      // calculate eta and phi
      double r = TMath::Sqrt(hit.position().x*hit.position().x + hit.position().y*hit.position().y + hit.position().z*hit.position().z);
      double theta = TMath::ACos(hit.position().z/r);
      double eta = -TMath::Log2(TMath::Tan(theta/2.0));
      double phi = TMath::ATan(hit.position().y / hit.position().x); 
      if (fillXY) {
        m_XYEvent0->Fill((*decoder)["x"],(*decoder)["y"]);
        m_XYEvent0->Fill(hit.position().x, hit.position().y);
	    if((*decoder)[m_layerFieldName] == 18) {
          m_EtaPhiEvent0->Fill(eta,phi);
          }
    	}
    }
  }

  // Fill histograms
  m_totalHits->Fill(sumHits);
  //debug() << "total hits in event = " << sumHits << endmsg;
  for (uint i = 1; i < m_numLayers; i++) {
    m_totalHitsLayers[i]->Fill(sumHitslayers[i]);
    //debug() << "total hits in layer " << i << " = " << sumHitslayers[i] << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode DECalLongitudinalTest::finalize() { 
  debug() << "StatusCode DECalLongitudinalTest::finalize()" << endmsg;

  // count from 1 to avoid the hits in the Trkr volume
  for (uint i = 1; i < m_numLayers; i++) {
      m_totalHitsVsLayer->SetBinContent(i+1,m_totalHitsLayers[i]->GetMean());
    //debug() << "total hits in layer " << i << " = " << sumHitslayers[i] << endmsg;
  }

return GaudiAlgorithm::finalize(); }
