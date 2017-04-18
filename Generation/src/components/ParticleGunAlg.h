#ifndef GENERATION_PARTICLEGUNALG_H
#define GENERATION_PARTICLEGUNALG_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "HepMC/GenEvent.h"

/// Forward declarations
#include "Generation/IParticleGunTool.h"
#include "Generation/IVertexSmearingTool.h"

/** @class ParticleGunAlg ParticleGunAlg.h "ParticleGunAlg.h"
 *
 *  Main algorithm to generate particle gun events.
 *
 *  @author Patrick Robbe
 *  @author Benedikt Hegner (simplification and generalization for Gaudi)
 *  @date   2008-05-18
 */

class ParticleGunAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  ParticleGunAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~ParticleGunAlg();  ///< Destructor

  /** Algorithm initialization.
   *  -# Initializes the common Gaudi random number generator used in all
   *     generators,
   *  -# Retrieve particle gun tool, decay tool, vertex smearing tool and
   *     full event cut tool used in the generation of events.
   */
  virtual StatusCode initialize();

  /** Algorithm execution.
   *  Repeat the following sequence until a good set of interactions is
   *  generated.
   *  -#
   */
  virtual StatusCode execute();

  /** Algorithm finalization.
   *  Print generation counters.
   */
  virtual StatusCode finalize();

private:
  /// Tool that interfaces to the actual particle gun
  ToolHandle<IParticleGunTool> m_particleGunTool{"FlatSmearVertex/VertexSmearingToolPGun", this};
  /// Tool that allows to smear vertex
  ToolHandle<IVertexSmearingTool> m_vertexSmearingTool{"FlatSmearVertex/VertexSmearingToolPGun", this};
  /// The output handle for what is being produced
  DataHandle<HepMC::GenEvent> m_hepmchandle{"hepmc", Gaudi::DataHandle::Writer, this};
};

#endif
