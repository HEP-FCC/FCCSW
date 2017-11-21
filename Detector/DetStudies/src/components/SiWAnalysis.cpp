
#include "SiWAnalysis.h"

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

DECLARE_ALGORITHM_FACTORY(SiWAnalysis)

SiWAnalysis::SiWAnalysis(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc),
      m_histSvc("THistSvc", "SiWAnalysis"),
      m_geoSvc("GeoSvc", "SiWAnalysis")
{
  declareProperty("deposits", m_deposits, "Energy deposits in sampling calorimeter (input)");
  declareProperty("truth", m_truth, "Generated particle truth");

  m_calP0 = 239.74;
  m_calP1 = 99.1602;
  m_calP2 = -0.0143;

}
SiWAnalysis::~SiWAnalysis() {}

StatusCode SiWAnalysis::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_padReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_padReadoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  

  m_treeInfo = new TTree("info", "Analysis Tree");
  m_treeInfo->Branch("event_number", &m_treeEvtNumber);
  m_treeInfo->Branch("truth_energy", &m_truthEnergy);
  m_treeInfo->Branch("edep_tot", &m_sumEnergyDep);


  if(m_histSvc->regTree("/rec/Info", m_treeInfo).isFailure()) {
	error() << "Couldn't register tree" << endmsg;
	return StatusCode::FAILURE;
  }

  // create histograms

  m_totalEdep = new TH1F("SiW_totalEdep", "Total edep in an event", 15000, 0, 15);
  if (m_histSvc->regHist("/rec/SiW_Edep", m_totalEdep).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }

  m_calFit = new TF1("calFit", "pol2", 0,3000);
  m_calFit->SetParameter("p0", m_calP0);
  m_calFit->SetParameter("p1", m_calP1);
  m_calFit->SetParameter("p2", m_calP2);
  
  // if it gets to here then it has registered all histograms
  return StatusCode::SUCCESS;
}

StatusCode SiWAnalysis::execute() {
  // get the decoder for the readout 
  // this will allow to extract layers etc.
  auto decoder = m_geoSvc->lcdd()->readout(m_padReadoutName).idSpec().decoder();

  // calculate the MC truth energy of the incident particle  
  const auto truth = m_truth.get();
  for (const auto& t : *truth) {
    double px = t.core().p4.px;
    double py = t.core().p4.py;
    double pz = t.core().p4.pz;
    double m = t.core().p4.mass;

    m_truthEnergy = TMath::Sqrt(m*m + px*px + py*py + pz*pz);   
  }

  // added a little print out to ensure it is still working
  if(m_treeEvtNumber < 10 or m_treeEvtNumber % 100 == 0) {
    info() << "Event Number " << m_treeEvtNumber << endmsg;
     info() << "Truth energy of single particle = " << m_truthEnergy << endmsg;
  }


  // set some variables to hold information for an event
  m_sumEdepLayers.assign(m_numLayers+1, 0);
  m_sumEnergyDep = 0;

  int isdigital = 0, notdigital = 0;

  const auto deposits = m_deposits.get();
  for (const auto& hit : *deposits) {
    decoder->setValue(hit.core().cellId);
    int digital = (*decoder)["digital"];
    if(digital==0) {
      notdigital++;

    }
    if(digital==1) {
      isdigital++;
      continue;
    }

    int layer = (*decoder)[m_layerFieldName];
    
    // check if energy was deposited in the calorimeter (active/passive material)
    // layers are numbered starting from 1, layer == 0 is cryostat/bath
    if (layer > 0) {
      decoder->setValue(hit.core().cellId);  
      int layer = (*decoder)[m_layerFieldName];
      // get the energy deposited in the active area
      double edep = hit.core().energy;
      m_sumEnergyDep += edep;
      
    }
  }

  std::cout << "isdigital = " << isdigital << std::endl;
  std::cout << "notdigital = " << notdigital << std::endl;
/*
  m_calPixels = m_calFit->GetX(m_sumPixels);
  //std::cout << "#### " << m_sumPixels << " " << m_calPixels << std::endl;
*/

// Fill histograms
  m_totalEdep->Fill(m_sumEnergyDep);

  if(m_sumEnergyDep>0) {
    m_treeInfo->Fill();
  }

  m_treeEvtNumber++;

  return StatusCode::SUCCESS;
}

StatusCode SiWAnalysis::finalize() { 
  debug() << "StatusCode SiWAnalysis::finalize()" << endmsg;


return GaudiAlgorithm::finalize(); }

Double_t SiWAnalysis::FitLongProfile( Double_t *x, Double_t *par)
{
	double E_0 = par[0];
	double a = par[1];
	double b = par[2];
	double led =  E_0*b * (pow( (b*x[0]), a-1 )* exp(-(b*x[0])) ) / TMath::Gamma(a);
	
	return led;
}
