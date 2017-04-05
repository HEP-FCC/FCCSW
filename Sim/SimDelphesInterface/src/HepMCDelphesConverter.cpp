#include "HepMCDelphesConverter.h"

#include <map>

#include "TDatabasePDG.h"
#include "TLorentzVector.h"
#include "TObjArray.h"
#include "TParticlePDG.h"

#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/SimpleVector.h"
#include "HepMC/Units.h"

#include "ExRootAnalysis/ExRootTreeBranch.h"

HepMCDelphesConverter::HepMCDelphesConverter() : m_pdg(TDatabasePDG::Instance()) {}

StatusCode HepMCDelphesConverter::hepMCEventToArrays(const HepMC::GenEvent* hepMCEvent, DelphesFactory& factory,
                                                     TObjArray& allParticleOutputArray,
                                                     TObjArray& stableParticleOutputArray,
                                                     TObjArray& partonOutputArray) const {
  if (hepMCEvent->is_valid()) {
    // Vertex ID -> first mother particle, last mother particle
    VertexParticleMap motherMap;
    // Vertex ID -> first daughter particle, last daughter particle
    VertexParticleMap daughterMap;

    // Units info
    HepMC::Units::LengthUnit lengthUnit = hepMCEvent->length_unit();
    HepMC::Units::MomentumUnit momentumUnit = hepMCEvent->momentum_unit();

    double momentumCoefficient = 1.0;
    if (momentumUnit == HepMC::Units::MEV) momentumCoefficient = 0.001;

    double positionCoefficient = 1.0;
    if (lengthUnit == HepMC::Units::CM) positionCoefficient = 10.0;

    std::map<int, std::pair<int, int>>::iterator itMotherMap;
    std::map<int, std::pair<int, int>>::iterator itDaughterMap;

    for (auto ipart = hepMCEvent->particles_begin(); ipart != hepMCEvent->particles_end(); ++ipart) {
      auto candidate = factory.NewCandidate();
      // Particle info
      const HepMC::GenParticle& hepMCPart = **ipart;

      candidate->PID = hepMCPart.pdg_id();
      candidate->Status = hepMCPart.status();
      candidate->Mass = hepMCPart.generatedMass();
      candidate->Momentum.SetPxPyPzE(
          hepMCPart.momentum().px(), hepMCPart.momentum().py(), hepMCPart.momentum().pz(), hepMCPart.momentum().e());
      if (momentumCoefficient != 1.0) {
        candidate->Momentum *= momentumCoefficient;
      }
      candidate->M2 = 1;
      candidate->D2 = 1;

      int outVertexCode = 0;

      // Production vertex info
      if (hepMCPart.production_vertex()) {
        const HepMC::GenVertex& hepMCVertex = *hepMCPart.production_vertex();
        outVertexCode = hepMCVertex.barcode();

        candidate->M1 = outVertexCode;
        candidate->Position.SetXYZT(hepMCVertex.position().x(),
                                    hepMCVertex.position().y(),
                                    hepMCVertex.position().z(),
                                    hepMCVertex.position().t());
        if (positionCoefficient != 1.0) {
          candidate->Position *= positionCoefficient;
        }
      } else {  // Particle is orphan
        outVertexCode = 0;
        candidate->Position.SetXYZT(0, 0, 0, 0);
        candidate->M1 = 1;
      }

      // Stable particle? => Create std::map
      int inVertexCode = 0;
      if (hepMCPart.end_vertex()) {
        inVertexCode = hepMCPart.end_vertex()->barcode();
      }
      if (inVertexCode < 0) {
        candidate->D1 = inVertexCode;
        int motherID = (*(hepMCPart.end_vertex()->particles_in_const_begin()))->barcode() - 1;
        int motherIDRange = hepMCPart.end_vertex()->particles_in_size() - 1;
        itMotherMap = motherMap.find(inVertexCode);
        if (itMotherMap == motherMap.end())
          motherMap[inVertexCode] = std::make_pair(motherID, motherID + motherIDRange);
      } else {
        candidate->D1 = 1;
      }

      if (outVertexCode < 0) {
        int daughterID = (*(hepMCPart.production_vertex()->particles_out_const_begin()))->barcode() - 1;
        int daughterIDRange = hepMCPart.production_vertex()->particles_out_size() - 1;
        itDaughterMap = daughterMap.find(outVertexCode);
        if (itDaughterMap == daughterMap.end())
          daughterMap[outVertexCode] = std::make_pair(daughterID, daughterID + daughterIDRange);
      }

      auto pdgParticle = m_pdg->GetParticle(candidate->PID);
      candidate->Charge = pdgParticle ? int(pdgParticle->Charge() / 3.0) : -999;

      allParticleOutputArray.Add(candidate);
      if (pdgParticle) {
        int pdgCode = TMath::Abs(candidate->PID);
        if (candidate->Status == 1) {
          stableParticleOutputArray.Add(candidate);
        } else if (pdgCode <= 5 || pdgCode == 21 || pdgCode == 15) {
          partonOutputArray.Add(candidate);
        }
      }
    }

    // Finalize
    setRelationIndices(allParticleOutputArray, daughterMap, motherMap);

    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

void HepMCDelphesConverter::setRelationIndices(TObjArray& allParticleOutputArray,
                                               const VertexParticleMap& daughterMap,
                                               const VertexParticleMap& motherMap) const {
  for (int i = 0; i < allParticleOutputArray.GetEntriesFast(); ++i) {
    auto candidate = static_cast<Candidate*>(allParticleOutputArray.At(i));

    if (candidate->M1 > 0) {
      candidate->M1 = -1;
      candidate->M2 = -1;
    } else {
      auto itMotherMap = motherMap.find(candidate->M1);
      if (itMotherMap == motherMap.end()) {
        candidate->M1 = -1;
        candidate->M2 = -1;
      } else {
        candidate->M1 = itMotherMap->second.first;
        candidate->M2 = itMotherMap->second.second;
      }
    }

    if (candidate->D1 > 0) {
      candidate->D1 = -1;
      candidate->D2 = -1;
    } else {
      auto itDaughterMap = daughterMap.find(candidate->D1);
      if (itDaughterMap == daughterMap.end()) {
        candidate->D1 = -1;
        candidate->D2 = -1;
      } else {
        candidate->D1 = itDaughterMap->second.first;
        candidate->D2 = itDaughterMap->second.second;
      }
    }
  }
}
