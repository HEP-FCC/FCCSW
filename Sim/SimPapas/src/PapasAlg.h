#ifndef SIM_PAPASALG_H
#define SIM_PAPASALG_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
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
  PapasAlg(const std::string& name, ISvcLocator* svcLoc, seed = 0);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute: runs the algorithm on a PapasEvent
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Stores the linkages between papas objects (eg clusters, tracks, particles)
  papas::Nodes m_history;
  std::vector<IPapasTool*> m_tools;      ///<vector of tools to be run
  std::vector<std::string> m_toolNames;  ///< names of tools to be run
  long m_eventno; ///< the papas Event number, incremented for each event processed
  int m_seed; ///<seed for random generator, default to 0 (no seed)
};

#endif  // PAPASALG_H
