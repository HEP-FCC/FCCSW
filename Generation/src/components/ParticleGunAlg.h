#ifndef GENERATION_PARTICLEGUNALG_H
#define GENERATION_PARTICLEGUNALG_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"
#include "HepMC/GenEvent.h"
#include "GaudiKernel/ToolHandle.h"

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
  ParticleGunAlg( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~ParticleGunAlg( ); ///< Destructor
  
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
  virtual StatusCode execute   ();
  
  /** Algorithm finalization.
   *  Print generation counters.
   */
  virtual StatusCode finalize  ();

private:
  int m_eventType ; ///< Event type (set by options)
  
  /// Location where to store generator events (set by options)
  std::string  m_hepMCEventLocation ; 
  
  ToolHandle<IParticleGunTool> m_particleGunTool; ///< Particle gun tool
  
  ToolHandle<IVertexSmearingTool> m_vertexSmearingTool; ///< Vertex smear tool
  
  /// Name to put in the event
  std::string m_particleGunName ;
  /// The output handle for what is being produced
  DataHandle<HepMC::GenEvent> m_hepmchandle;
};

#endif
