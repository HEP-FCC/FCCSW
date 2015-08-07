// $Id: ParticleGun.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_PARTICLEGUN_H
#define PARTICLEGUNS_PARTICLEGUN_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"

// Forward declarations
class IParticleGunTool       ;
class IPileUpTool            ;
class IDecayTool             ;
class ISampleGenerationTool  ;
class IVertexSmearingTool    ;
class IFullGenEventCutTool   ;
class IGenCutTool   ;

namespace HepMC {
class GenEvent ;
}

/** @class ParticleGun ParticleGun.h "ParticleGun.h"
 *
 *  Main algorithm to generate particle gun events.
 *
 *  @author Patrick Robbe
 *  @date   2008-05-18
 */
class ParticleGun : public GaudiAlgorithm {
 public:
  typedef std::vector< HepMC::GenParticle * > ParticleVector ;
  /// Standard constructor
  ParticleGun( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleGun( ); ///< Destructor

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

protected:
  /// Decay the event with the IDecayTool.
  HepMC::GenParticle *decayEvent( LHCb::HepMCEvent * theEvent, ParticleVector & particleList, StatusCode & sc) ;

  /// Perpare the particle containers
  void prepareInteraction( LHCb::HepMCEvents * theEvents ,
                           LHCb::GenCollisions * theCollisions ,
                           HepMC::GenEvent * & theGenEvent ,
                           LHCb::GenCollision * & theGenCollision ) const ;

private:
  int          m_eventType ; ///< Event type (set by options)

  /// Location where to store generator events (set by options)
  std::string  m_hepMCEventLocation ;

  /// Location where to store the Header of the events (set by options)
  std::string  m_genHeaderLocation ;

  /// Location where to store HardInfo (set by options)
  std::string  m_genCollisionLocation ;

  IParticleGunTool         * m_particleGunTool        ; ///< Particle gun tool

  IPileUpTool              * m_numberOfParticlesTool  ; ///< Number of particles tool

  IDecayTool               * m_decayTool              ; ///< Decay tool

  ISampleGenerationTool    * m_sampleGenerationTool   ; ///< Sample tool

  IVertexSmearingTool      * m_vertexSmearingTool     ; ///< Smearing tool

  IFullGenEventCutTool     * m_fullGenEventCutTool    ; ///< Cut tool

  IGenCutTool              * m_genCutTool    ; ///< Cut tool

  /// Name of the IParticleGunTool (set by options)
  std::string m_particleGunToolName ;

  /// Name of the tool to set number of particles per event (set by options)
  std::string m_numberOfParticlesToolName ;

  /// Name of the IDecayTool (set by options)
  std::string m_decayToolName            ;

  /// Name of the IVertexSmearingTool (set by options)
  std::string m_vertexSmearingToolName   ;

  /// Name of the IFullGenEventCutTool (set by options)
  std::string m_fullGenEventCutToolName  ;

  /// Name of the IGenCutTool (set by options)
  std::string m_genCutToolName  ;

  /// Name to put in the event
  std::string m_particleGunName ;

  unsigned int m_nEvents ; ///< Number of generated events

  unsigned int m_nAcceptedEvents ; ///< Number of accepted events

  unsigned int m_nParticles ; ///< Number of generated particles

  /// Number of particles in accepted events
  unsigned int m_nAcceptedParticles ;

  /// Counter of events before the full event generator level cut
  unsigned int m_nBeforeFullEvent ;

  /// Counter of events after the full event generator level cut
  unsigned int m_nAfterFullEvent ;

  /// Counter of events before the generator level cut
  unsigned int m_nBeforeCut ;

  /// Counter of events after the generator level cut
  unsigned int m_nAfterCut ;

  /// PDG id of signal particle.
  int          m_sigPdgCode ;
};
#endif // PARTICLEGUNS_PARTICLEGUN_H
