#ifndef SIM_PAPASALG_H
#define SIM_PAPASALG_H

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// FCCSW
#include "FWCore/DataHandle.h"
// papas
#include "papas/datatypes/Event.h"
#include "papas/detectors/CMS.h"

#include <string>
class IPapasTool;

namespace fcc {
class MCParticleCollection;
}

/** @class PapasAlg Sim/SimPapasInterface/src/PapasAlg.h PapasAlg.h
 *
 *  Runs papas simulation and reconstruction tools
 *  @author A.J. Robson
 */
class PapasAlg : public GaudiAlgorithm {
  friend class AlgFactory<PapasAlg>;

public:
  /// Constructor.
  PapasAlg(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute: runs each of the tools on a PapasEvent.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Stores the linkages between papas objects (eg clusters, tracks, particles)
  papas::Nodes m_history;
  std::vector<IPapasTool*> m_tools;      ///<vector of tools to be run
  std::vector<std::string> m_toolNames;  ///< names of tools to be run
  long m_eventno;                        ///< the papas Event number, incremented for each event processed
  ///<seed for random generator, default to 0 (no seed)
  Gaudi::Property<long> m_seed{this, "seed", 0, "random seed"};
  ///<seed for papas physics debug ouput default to "" no output
  Gaudi::Property<std::string> m_physicsDebugFile{this, "physicsDebugFile", "",
                                                  "name of optional file to output physics info"};
};

#endif  // PAPASALG_H
