#include "UpstreamMaterial.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/MCParticleCollection.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TVector2.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/ITHistSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_ALGORITHM_FACTORY(UpstreamMaterial)

UpstreamMaterial::UpstreamMaterial(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
   declareProperty("deposits", m_deposits, "Energy deposits (input)");
   declareProperty("particle", m_particle, "Generated single-particle event (input)");
}
UpstreamMaterial::~UpstreamMaterial() {}

StatusCode UpstreamMaterial::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  m_histSvc = service("THistSvc");
  if (! m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }
  int num_layers = m_numLayers;
  for(int i = 0; i < num_layers; i++) {
    m_cellEnergyPhi.push_back( new TH1F(("presamplerEnergy_phi"+std::to_string(i)).c_str(), ("Energy deposited in layer "+std::to_string(i)).c_str(), 100, -10 * 0.00360895, 10 * 0.00360895) );
    if (m_histSvc->regHist("/rec/presamplerEnergy_phi"+std::to_string(i), m_cellEnergyPhi.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
    m_upstreamEnergyCellEnergy2D.push_back( new TH2F(("upstreamEnergy_presamplerEnergy2D"+std::to_string(i)).c_str(), ("2D Upstream energy vs energy deposited in layer "+std::to_string(i)).c_str(), 4000, 0, m_energy, 1000, 0, m_energy / 5. ));
    if (m_histSvc->regHist("/rec/upstreamEnergy_presamplerEnergy2D"+std::to_string(i), m_upstreamEnergyCellEnergy2D.back()).isFailure()) {
      error() << "Couldn't register hist" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode UpstreamMaterial::execute() {
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  double sumEupstream = 0.;
  double integralEcells = 0;
  std::vector<double> sumEcells;
  sumEcells.assign(m_numLayers, 0);

  const auto particle = m_particle.get();
  double phi = 0;
  for(const auto& part : *particle) {
    phi = atan2(part.core().p4.py, part.core().p4.px);
  }

  const auto deposits = m_deposits.get();
   for(const auto& hit : *deposits) {
     decoder->setValue(hit.core().cellId);
     if((*decoder)["cryo"] == 0) {
       sumEcells[(*decoder)[m_layerFieldName] - m_firstLayerId] += hit.core().energy;
     } else {
       sumEupstream += hit.core().energy;
     }
   }
  for(uint i = 0; i < m_numLayers; i++) {
    sumEcells[i] /= m_samplingFraction[i];
    integralEcells += sumEcells[i];
    m_cellEnergyPhi[i]->Fill(phi, integralEcells);
    m_upstreamEnergyCellEnergy2D[i]->Fill(integralEcells, sumEupstream);
    verbose() << "total E in cell "<< i << " = " << sumEcells[i] <<"\t up to current cell = " << integralEcells << "\t upstream = " << sumEupstream << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode UpstreamMaterial::finalize() {
  return GaudiAlgorithm::finalize();
}
