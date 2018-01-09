#ifndef SIM_PAPASALG_H
#define SIM_PAPASALG_H

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// FCCSW
#include "FWCore/DataHandle.h"
// papas
#include "papas/datatypes/Definitions.h"
#include "papas/datatypes/Event.h"

#include <string>
class IPapasTool;
class IPapasImportTool;
class IPapasExportTool;
class IPapasDetSvc;

namespace fcc {
class MCParticleCollection;
}
namespace papas {
class Detector;
}

/** @class PapasAlg Sim/SimPapasInterface/src/PapasAlg.h PapasAlg.h
 *
 *  Main algorithm that coordinate and runs papas simulation and reconstruction tools
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
  std::shared_ptr<papas::Detector> m_spDetector;  ///< papas detector
  /// Stores the linkages between papas objects (eg clusters, tracks, particles)
  papas::Nodes m_history;
  std::vector<IPapasTool*> m_tools;      ///< vector of tools to be run
  IPapasImportTool* m_importTool;        ///< import tool to be run
  IPapasExportTool* m_exportTool;        ///< export tool to be run
  std::vector<std::string> m_toolNames;  ///< names of tools to be run
  std::string m_importToolName;          ///< name of import tool to be run
  std::string m_exportToolName;          ///< name of export tool to be run
  std::unordered_map<papas::Identifier, int>
      m_particleLinks;  ///< map to contain links between GenParticles and SimParticles
  long m_eventno;       ///< the papas Event number, incremented for each event processed
  ///<seed for random generator, default to 0 (no seed)
  Gaudi::Property<long> m_seed{this, "seed", 0, "random seed"};
  ///<seed for papas physics debug ouput default to "" no output
  Gaudi::Property<std::string> m_physicsDebugFile{this, "physicsDebugFile", "",
                                                  "name of optional file to output physics info"};
  ///< Name of the papas detector service that is to be used
  Gaudi::Property<std::string> m_detServiceName{this, "detService", "", "name of detector service"};
};

#endif  // PAPASALG_H
