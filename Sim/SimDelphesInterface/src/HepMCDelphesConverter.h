#ifndef SIM_HEPMCDELPHESCONVERTER_H
#define SIM_HEPMCDELPHESCONVERTER_H

/** @class HepMCDelphesConverter
 *
 *  Fills TObjArrays with particles and vertices from HepMC::GenEvent
 *
 *  @author (original): P. Demin - UCL, Louvain-la-Neuve
 *  @author (adapted to FCC SW): Z. Drasal (CERN)
 *
 */

#include <map>

#include "GaudiKernel/StatusCode.h"

class TObjArray;
class TDatabasePDG;
class DelphesFactory;

namespace HepMC {
class GenEvent;
}

class HepMCDelphesConverter {
public:
  /// Default constructor
  HepMCDelphesConverter();
  /// Default destructor
  ~HepMCDelphesConverter(){};

  /// Fill vertices and particles into arrays and add them through the factory
  StatusCode hepMCEventToArrays(const HepMC::GenEvent* hepMCEvent,
                                DelphesFactory& factory,
                                TObjArray& allParticleOutputArray,
                                TObjArray& stableParticleOutputArray,
                                TObjArray& partonOutputArray) const;

private:
  /// Map to store vertex to particle relations
  typedef std::map<int, std::pair<int, int>> VertexParticleMap;
  /// Sets relations of particles
  void setRelationIndices(TObjArray& allParticleOutputArray,
                          const VertexParticleMap& daughterMap,
                          const VertexParticleMap& motherMap) const;

  /// Database to get charge
  const TDatabasePDG* m_pdg;
};

#endif  // SIM_HEPMCDELPHESCONVERTER_H
