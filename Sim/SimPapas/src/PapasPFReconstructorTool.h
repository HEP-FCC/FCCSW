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
  /// Structures
  StatusCode createOutputStructures() { return SUCCESS; };
  /// empty class structures
  virtual StatusCode clear();
  /// Execute:
  virtual StatusCode run(papas::Event& pevent) final;
  /// Finalize.
  virtual StatusCode finalize();

private:
  papas::Particles m_rec_particles;
  Gaudi::Property<std::string> m_blockSubtype{this, "blockSubtype", "", "block subtype"};
  ///<Block type to use in reconstruction eg 's' for split blocks
};

#endif  // SIM_PAPASPFRECONSTRUCTO_H
