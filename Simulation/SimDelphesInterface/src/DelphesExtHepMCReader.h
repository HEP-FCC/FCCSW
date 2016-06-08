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

#ifndef DelphesExtHepMCReader_h
#define DelphesExtHepMCReader_h

/** @class DelphesExtHepMCReader
 *
 *  Reads either HepMC event directly from memory of from a file
 *
 *  @author (original): P. Demin - UCL, Louvain-la-Neuve
 *  @author (adapted to FCC SW): Z. Drasal (CERN)
 *
 */

#include <map>
#include <vector>

#include <stdio.h>
#include <Rtypes.h>

class TObjArray;
class TStopwatch;
class TDatabasePDG;
class ExRootTreeBranch;
class DelphesFactory;

namespace HepMC
{
  class GenEvent;
}

class DelphesExtHepMCReader
{
public:

  DelphesExtHepMCReader();
  ~DelphesExtHepMCReader();

  // Zero all internal variables
  void Clear();

  // Set input HepMC file --> ReadEventFromFile() then
  void SetInputFile(FILE *inputFile);

  // Read event & fill all variables
  bool ReadEventFromFile(DelphesFactory *factory, TObjArray *allParticleOutputArray, TObjArray *stableParticleOutputArray, TObjArray *partonOutputArray);
  bool ReadEventFromStore(const HepMC::GenEvent *hepMCEvent, DelphesFactory *factory, TObjArray *allParticleOutputArray, TObjArray *stableParticleOutputArray, TObjArray *partonOutputArray);

  // Create event branch for Delphes treeWriter
  void MakeEventBranch(ExRootTreeBranch *branch, TStopwatch *readStopWatch, TStopwatch *procStopWatch);

private:

  // Is event correctly read-in
  bool EventReady();

  // Method reading blocks from input-file untill all variables correctly set
  bool ReadBlock(DelphesFactory *factory,
    TObjArray *allParticleOutputArray,
    TObjArray *stableParticleOutputArray,
    TObjArray *partonOutputArray);

  // Method setting particle(s) info into Delphes internal blocks
  void AnalyzeParticle(DelphesFactory *factory,
    TObjArray *allParticleOutputArray,
    TObjArray *stableParticleOutputArray,
    TObjArray *partonOutputArray);

  void FinalizeParticles(TObjArray *allParticleOutputArray);

  // Variables
  FILE     *fInputFile;
  Long64_t fInputFileLength;

  char *fBuffer;

  TDatabasePDG *fPDG;

  int fEventNumber, fMPI, fProcessID, fSignalCode, fVertexCounter, fBeamCode[2];
  double fScale, fAlphaQCD, fAlphaQED;

  double fMomentumCoefficient, fPositionCoefficient;

  int fStateSize;
  std::vector< int > fState;

  int fWeightSize;
  std::vector< double > fWeight;

  int fID1, fID2;
  double fX1, fX2, fScalePDF, fPDF1, fPDF2;

  int fOutVertexCode, fVertexID, fInCounter, fOutCounter;
  double fX, fY, fZ, fT;

  int fParticleCode, fPID, fStatus, fInVertexCode;
  double fPx, fPy, fPz, fE, fMass, fTheta, fPhi;

  int fParticleCounter;

  std::map< int, std::pair < int, int > > fMotherMap;
  std::map< int, std::pair < int, int > > fDaughterMap;
};

#endif // DelphesExtHepMCReader_h


