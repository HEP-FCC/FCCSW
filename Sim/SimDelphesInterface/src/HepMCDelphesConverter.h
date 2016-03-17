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

#ifndef SIM_HEPMCDELPHESCONVERTER_H
#define SIM_HEPMCDELPHESCONVERTER_H

/** @class HepMCDelphesConverter
 *
 *  Reads either HepMC event directly from memory of from a file
 *
 *  @author (original): P. Demin - UCL, Louvain-la-Neuve
 *  @author (adapted to FCC SW): Z. Drasal (CERN)
 *
 */

#include <map>

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

class HepMCDelphesConverter
{
public:

  HepMCDelphesConverter();
  ~HepMCDelphesConverter();

  // Read event & fill all variables
  StatusCode readEventFromStore(const HepMC::GenEvent *hepMCEvent, DelphesFactory *factory, TObjArray *allParticleOutputArray, TObjArray *stableParticleOutputArray, TObjArray *partonOutputArray);

private:
  typedef std::map<int, std::pair <int, int>> VertexParticleMap;
  // Method setting particle(s) info into Delphes internal blocks
  void analyzeParticle(DelphesFactory *factory,
    TObjArray *allParticleOutputArray,
    TObjArray *stableParticleOutputArray,
    TObjArray *partonOutputArray);

  void finalizeParticles(TObjArray *allParticleOutputArray,
                         const VertexParticleMap& daughterMap,
                         const VertexParticleMap& motherMap);

  const TDatabasePDG *m_pdg;
};

#endif // SIM_HEPMCDELPHESCONVERTER_H


