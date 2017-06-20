#ifndef SIM_PAPASSIMULATORTOOL_H
#define SIM_PAPASSIMULATORTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
// FCCSW
#include "FWCore/DataHandle.h"
#include "SimPapas/IPapasTool.h"
// papas
#include "papas/datatypes/DefinitionsCollections.h"
#include "papas/datatypes/Event.h"

namespace fcc {
class MCParticleCollection;
}

/** @class PapasSimulatorTool Sim/SimPapas/src/PapasSimulatorTool.h PapasSimulatorTool.h
 *
 *  Runs the papas simulation algorithm. This creates a list of particles from Pythia generated particles.
 *  It then creates particle flow simulated and true and smeared clusters and tracks. All of these
 *  collections are appended to the papasevent.
 *  @author A.J. Robson
 */

class PapasSimulatorTool : public GaudiTool, virtual public IPapasTool {
public:
  /// Constructor.
  PapasSimulatorTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PapasSimulatorTool();
  /// Initialize.
  virtual StatusCode initialize();
  /// Structures
  StatusCode createOutputStructures() { return SUCCESS; };
  /// empty class structures
  virtual StatusCode clear();
  /// Run
  virtual StatusCode run(papas::Event& papas) final;
  /// Finalize.
  virtual StatusCode finalize();

private:
  papas::Clusters m_ecalClusters;
  papas::Clusters m_hcalClusters;
  papas::Tracks m_trackClusters;
  papas::Clusters m_smearedEcalClusters;
  papas::Clusters m_smearedHcalClusters;
  papas::Tracks m_smearedTracks;
  Gaudi::Property<std::string> m_particleSubtype{this, "particleSubtype", "", "particle subtype"};
};  ///< Which particles to use*/

#endif  // SIM_PAPASSIMULATORTOOL_H
