/*
 *  Delphes: a framework for fast simulation of a generic collider experiment
 *  Copyright (C) 2012-2014  Universite catholique de Louvain (UCL), Belgium
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \class HepMCDelphesConverter
 *
 *  Reads HepMC file or HepMC event from transient data store
 *
 *  \ Original: author P. Demin - UCL, Louvain-la-Neuve
 *  \ Adapted to FCC SW: author Z. Drasal (CERN)
 *
 *    - reads either HepMC event directly from memory of from a file
 */

#include "HepMCDelphesConverter.h"

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <map>
#include <vector>

#include <stdio.h>

#include "TObjArray.h"
#include "TStopwatch.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TLorentzVector.h"

#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesStream.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/SimpleVector.h"
#include "HepMC/Units.h"

#include "ExRootAnalysis/ExRootTreeBranch.h"

using namespace std;

static const int kBufferSize  = 1024;

//---------------------------------------------------------------------------

HepMCDelphesConverter::HepMCDelphesConverter() :
  fPDG(0),
  fVertexCounter(-1), fInCounter(-1), fOutCounter(-1),
  fParticleCounter(0)
{
  fPDG = TDatabasePDG::Instance();
}

//---------------------------------------------------------------------------

HepMCDelphesConverter::~HepMCDelphesConverter()
{
}

//---------------------------------------------------------------------------
void HepMCDelphesConverter::Clear()
{
  fStateSize = 0;
  fState.clear();
  fWeightSize = 0;
  fWeight.clear();
  fMomentumCoefficient = 1.0;
  fPositionCoefficient = 1.0;
  fVertexCounter = -1;
  fInCounter = -1;
  fOutCounter = -1;
  fMotherMap.clear();
  fDaughterMap.clear();
  fParticleCounter = 0;
}

//---------------------------------------------------------------------------
bool HepMCDelphesConverter::ReadEventFromStore(const HepMC::GenEvent *hepMCEvent, DelphesFactory *factory,
                                               TObjArray *allParticleOutputArray,
                                               TObjArray *stableParticleOutputArray,
                                               TObjArray *partonOutputArray)
{
  if (hepMCEvent->is_valid()) {

    // Clear first
    Clear();

    // Units info
    HepMC::Units::LengthUnit   lengthUnit   = hepMCEvent->length_unit();
    HepMC::Units::MomentumUnit momentumUnit = hepMCEvent->momentum_unit();

    if     (momentumUnit == HepMC::Units::GEV) fMomentumCoefficient = 1.0;
    else if(momentumUnit == HepMC::Units::MEV) fMomentumCoefficient = 0.001;

    if     (lengthUnit == HepMC::Units::MM) fPositionCoefficient = 1.0;
    else if(lengthUnit == HepMC::Units::CM) fPositionCoefficient = 10.0;

    // Event info
    fEventNumber   = hepMCEvent->event_number();

    fMPI           = hepMCEvent->mpi();
    fScale         = hepMCEvent->event_scale();
    fAlphaQCD      = hepMCEvent->alphaQCD();
    fAlphaQED      = hepMCEvent->alphaQED();
    fProcessID     = hepMCEvent->signal_process_id();
    if (hepMCEvent->signal_process_vertex()) {
      fSignalCode  = hepMCEvent->signal_process_vertex()->barcode();
    }
    fVertexCounter = hepMCEvent->vertices_size();
    fBeamCode[0]   = hepMCEvent->beam_particles().first->barcode();
    fBeamCode[1]   = hepMCEvent->beam_particles().second->barcode();

    fStateSize   = hepMCEvent->random_states().size();
    for (auto i=0; i<fStateSize;  i++) fState.push_back(hepMCEvent->random_states()[i]);
    fWeightSize  = hepMCEvent->weights().size();
    for (auto i=0; i<fWeightSize; i++) fWeight.push_back(hepMCEvent->weights()[i]);

    // PDF info
    fID1         = hepMCEvent->pdf_info()->id1();
    fID2         = hepMCEvent->pdf_info()->id2();
    fX1          = hepMCEvent->pdf_info()->x1();
    fX2          = hepMCEvent->pdf_info()->x2();
    fScalePDF    = hepMCEvent->pdf_info()->scalePDF();
    fPDF1        = hepMCEvent->pdf_info()->pdf1();
    fPDF2        = hepMCEvent->pdf_info()->pdf2();

    // Go through vertices, fill vertex & corresponding particles
    map< int, pair< int, int > >::iterator itMotherMap;   // Vertex ID -> first mother particle, last mother particle
    map< int, pair< int, int > >::iterator itDaughterMap; // Vertex ID -> first daughter particle, last daughter particle

    for (auto ipart=hepMCEvent->particles_begin(); ipart!=hepMCEvent->particles_end(); ++ipart) {

      // Particle info
      const HepMC::GenParticle *hepMCPart = *ipart;

      fParticleCode = hepMCPart->barcode();
      fPID          = hepMCPart->pdg_id();
      fPx           = hepMCPart->momentum().px();
      fPy           = hepMCPart->momentum().py();
      fPz           = hepMCPart->momentum().pz();
      fE            = hepMCPart->momentum().e();
      fMass         = hepMCPart->generatedMass();
      fStatus       = hepMCPart->status();
      fTheta        = hepMCPart->polarization().theta();
      fPhi          = hepMCPart->polarization().phi();

      // Stable particle?
      if (hepMCPart->end_vertex()) fInVertexCode = hepMCPart->end_vertex()->barcode();
      else                         fInVertexCode = 0;

      // Production vertex info
      if (hepMCPart->production_vertex()) {

        const HepMC::GenVertex *hepMCVertex = hepMCPart->production_vertex();

        fOutVertexCode = hepMCVertex->barcode();
        fVertexID      = hepMCVertex->id();
        fX             = hepMCVertex->position().x();
        fY             = hepMCVertex->position().y();
        fZ             = hepMCVertex->position().z();
        fT             = hepMCVertex->position().t();
        fInCounter     = 0;
        fOutCounter    = hepMCVertex->particles_out_size();
      }
      // Particle is orphan
      else {

        fOutVertexCode = 0;
        fVertexID      = -1;
        fX             = 0.;
        fY             = 0.;
        fZ             = 0.;
        fT             = 0.;
        fInCounter     = hepMCPart->end_vertex()->particles_in_size();
        fOutCounter    = 0;
      }

      // Create map
      if (fInVertexCode < 0) {

        int motherID      = (*(hepMCPart->end_vertex()->particles_in_const_begin()))->barcode()-1;
        int motherIDRange = hepMCPart->end_vertex()->particles_in_size()-1;
        itMotherMap = fMotherMap.find(fInVertexCode);
        if (itMotherMap == fMotherMap.end()) fMotherMap[fInVertexCode] = make_pair(motherID,motherID+motherIDRange);
      }

      if (fOutVertexCode < 0) {

        int daughterID      = (*(hepMCPart->production_vertex()->particles_out_const_begin()))->barcode()-1;
        int daughterIDRange = hepMCPart->production_vertex()->particles_out_size()-1;
        itDaughterMap = fDaughterMap.find(fOutVertexCode);
        if(itDaughterMap == fDaughterMap.end()) fDaughterMap[fOutVertexCode] = make_pair(daughterID,daughterID+daughterIDRange);
      }

      AnalyzeParticle(factory, allParticleOutputArray, stableParticleOutputArray, partonOutputArray);

      // For compatibility
      fVertexCounter = 0;
      fInCounter     = 0;
      fOutCounter    = 0;
      ++fParticleCounter;
    }

    // Finalize
    FinalizeParticles(allParticleOutputArray);

    return true;
  }
  else {

    return false;
  }
}

//---------------------------------------------------------------------------

void HepMCDelphesConverter::MakeEventBranch(ExRootTreeBranch *branch, TStopwatch *readStopWatch, TStopwatch *procStopWatch)
{
  HepMCEvent *element;

  element = static_cast<HepMCEvent *>(branch->NewEntry());
  element->Number = fEventNumber;

  element->ProcessID = fProcessID;
  element->MPI = fMPI;
  element->Weight = fWeight.size() > 0 ? fWeight[0] : 1.0;
  element->Scale = fScale;
  element->AlphaQED = fAlphaQED;
  element->AlphaQCD = fAlphaQCD;

  element->ID1 = fID1;
  element->ID2 = fID2;
  element->X1 = fX1;
  element->X2 = fX2;
  element->ScalePDF = fScalePDF;
  element->PDF1 = fPDF1;
  element->PDF2 = fPDF2;

  element->ReadTime = readStopWatch->RealTime();
  element->ProcTime = procStopWatch->RealTime();
}

//---------------------------------------------------------------------------

void HepMCDelphesConverter::AnalyzeParticle(DelphesFactory *factory,
  TObjArray *allParticleOutputArray,
  TObjArray *stableParticleOutputArray,
  TObjArray *partonOutputArray)
{
  Candidate *candidate;
  TParticlePDG *pdgParticle;
  int pdgCode;

  candidate = factory->NewCandidate();

  candidate->PID = fPID;
  pdgCode = TMath::Abs(candidate->PID);

  candidate->Status = fStatus;

  pdgParticle = fPDG->GetParticle(fPID);
  candidate->Charge = pdgParticle ? int(pdgParticle->Charge()/3.0) : -999;
  candidate->Mass = fMass;

  candidate->Momentum.SetPxPyPzE(fPx, fPy, fPz, fE);
  if(fMomentumCoefficient != 1.0)
  {
    candidate->Momentum *= fMomentumCoefficient;
  }

  candidate->M2 = 1;
  candidate->D2 = 1;
  if(fInCounter > 0)
  {
    candidate->M1 = 1;
    candidate->Position.SetXYZT(0.0, 0.0, 0.0, 0.0);
  }
  else
  {
    candidate->M1 = fOutVertexCode;
    candidate->Position.SetXYZT(fX, fY, fZ, fT);
    if(fPositionCoefficient != 1.0)
    {
      candidate->Position *= fPositionCoefficient;
    }
  }
  if(fInVertexCode < 0)
  {
    candidate->D1 = fInVertexCode;
  }
  else
  {
    candidate->D1 = 1;
  }

  allParticleOutputArray->Add(candidate);

  if(!pdgParticle) return;

  if(fStatus == 1 && pdgParticle->Stable())
  {
    stableParticleOutputArray->Add(candidate);
  }
  else if(pdgCode <= 5 || pdgCode == 21 || pdgCode == 15)
  {
    partonOutputArray->Add(candidate);
  }
}

//---------------------------------------------------------------------------

void HepMCDelphesConverter::FinalizeParticles(TObjArray *allParticleOutputArray)
{
  Candidate *candidate;
  map< int, pair< int, int > >::iterator itMotherMap;
  map< int, pair< int, int > >::iterator itDaughterMap;
  int i;

  for(i = 0; i < allParticleOutputArray->GetEntriesFast(); ++i)
  {
    candidate = static_cast<Candidate *>(allParticleOutputArray->At(i));

    if(candidate->M1 > 0)
    {
      candidate->M1 = -1;
      candidate->M2 = -1;
    }
    else
    {
      itMotherMap = fMotherMap.find(candidate->M1);
      if(itMotherMap == fMotherMap.end())
      {
        candidate->M1 = -1;
        candidate->M2 = -1;
      }
      else
      {
        candidate->M1 = itMotherMap->second.first;
        candidate->M2 = itMotherMap->second.second;
      }
    }
    if(candidate->D1 > 0)
    {
      candidate->D1 = -1;
      candidate->D2 = -1;
    }
    else
    {
      itDaughterMap = fDaughterMap.find(candidate->D1);
      if(itDaughterMap == fDaughterMap.end())
      {
        candidate->D1 = -1;
        candidate->D2 = -1;
      }
      else
      {
        candidate->D1 = itDaughterMap->second.first;
        candidate->D2 = itDaughterMap->second.second;
      }
    }
  }
}

//---------------------------------------------------------------------------
