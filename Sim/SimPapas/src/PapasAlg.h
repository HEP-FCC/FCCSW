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
  PapasAlg(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute: Applies the filter
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the Papas History Collection
  papas::Nodes m_history;
  // DataHandle<papas::Nodes> m_papashistoryHandle{"History", Gaudi::DataHandle::Writer, this};  ///< History nodes
  std::vector<IPapasTool*> m_tools;      ///<vector of tools to be run
  std::vector<std::string> m_toolNames;  ///< names of tools to be run
  long m_eventno;
};

#endif  // PAPASALG_H
