#ifndef SIM_PAPASPFRECONSTRUCTOR_H
#define SIM_PAPASPFRECONSTRUCTOR_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
// FCCSW
#include "FWCore/DataHandle.h"
#include "SimPapas/IPapasTool.h"
// PAPAS
#include "papas/datatypes/DefinitionsCollections.h"
#include "papas/datatypes/Particle.h"

#include <string>
/** @class PapasPFReconstructorTool Sim/SimPapas/src/PapasPFReconstructorTool.h PapasPFReconstructorTool.h
 *
 *  Runs the papas particle reconstruction algorithm which takes existing particle flow blocks
 *  of tracks and clusters
 *  that are linked, and reconstructs particles from these blocks.
 *  The reconstructed particles are returned to the papasevent.
 *  @author A.J. Robson
 */
class PapasPFReconstructorTool : public GaudiTool, virtual public IPapasTool {
public:
  /// Constructor.
  PapasPFReconstructorTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  /// Initialize.
  virtual StatusCode initialize();
  /// empty class structures
  virtual StatusCode clear();
  /// Execute papas tool.
  virtual StatusCode run(papas::Event& pevent, std::shared_ptr<papas::Detector> spDetector) final;
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// collection of papas reconstructed particles that will be created by this tool
  papas::Particles m_recParticles;
  /// Block type to use in reconstruction eg 's' for split blocks
  Gaudi::Property<std::string> m_blockSubtype{this, "blockSubtype", "", "block subtype"};
};

#endif  // SIM_PAPASPFRECONSTRUCTOR_H
