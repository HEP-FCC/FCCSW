#ifndef SIM_PAPASSIMULATORTOOL_H
#define SIM_PAPASSIMULATORTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
// FCCSW
#include "FWCore/DataHandle.h"
#include "SimPapas/IPapasTool.h"
// PAPAS
#include "papas/datatypes/DefinitionsCollections.h"
#include "papas/datatypes/Event.h"

namespace fcc {
class MCParticleCollection;
}

/** @class PapasSimulatorTool Sim/SimPapas/src/PapasSimulatorTool.h PapasSimulatorTool.h
 *
 *  Runs the papas simulation algorithm. This takes a collection of papas simulated particles
 *  and simulates the interaction of these particles in the detector to produce true and smeared
 *  clusters and tracks, that will be used in the particle flow reconstruction.
 *  The collections of true and smeared clusters and tracks will be appended to the papasevent.
 *  @author A.J. Robson
 */

class PapasSimulatorTool : public GaudiTool, virtual public IPapasTool {
public:
  /// Constructor.
  PapasSimulatorTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PapasSimulatorTool();
  /// Initialize.
  virtual StatusCode initialize();
  /// empty class structures
  virtual StatusCode clear();
  /// Execute the papas tool.
  virtual StatusCode run(papas::Event& papas, std::shared_ptr<papas::Detector> spDetector) final;
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// collection into which simulated Ecal clusters will be added
  papas::Clusters m_ecalClusters;
  /// collection into which simulated Hcal clusters will be added
  papas::Clusters m_hcalClusters;
  /// collection into which simulated tracks will be added
  papas::Tracks m_tracks;
  /// collection into which simulated smeared Ecal clusters will be added
  papas::Clusters m_smearedEcalClusters;
  /// collection into which simulated smeared Hcal clusters will be added
  papas::Clusters m_smearedHcalClusters;
  /// collection into which simulated smeared tracks will be added
  papas::Tracks m_smearedTracks;
  /// subtype of particles that should be used in the simulation
  Gaudi::Property<std::string> m_particleSubtype{this, "particleSubtype", "", "particle subtype"};
};

#endif  // SIM_PAPASSIMULATORTOOL_H
