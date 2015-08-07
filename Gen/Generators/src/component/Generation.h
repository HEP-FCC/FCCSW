// $Id: Generation.h,v 1.9 2007-07-03 16:31:36 gcorti Exp $
#ifndef GENERATORS_GENERATION_H 
#define GENERATORS_GENERATION_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

#include <boost/array.hpp>

// Forward declarations
class ISampleGenerationTool ;
class IPileUpTool ;
class IDecayTool ;
class IVertexSmearingTool ;
class IFullGenEventCutTool ;
class ICounterLogFile ;

namespace HepMC {
  class GenParticle ;
}

namespace LHCb {
  class HepMCEvent;
}

/** @class Generation Generation.h "Generation.h"
 *  
 *  Main algorithm to generate Monte Carlo events.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-11
 */
class Generation : public GaudiAlgorithm {
 public:
  /// Standard constructor
  Generation( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Generation( ); ///< Destructor

  /** Algorithm initialization.
   *  -# Initializes the common Gaudi random number generator used in all 
   *     generators,
   *  -# Retrieve Pile-up tool, sample generation tool, decay tool,
   *     vertex smearing tool and full event cut tool used in the generation 
   *     of events.
   */
  virtual StatusCode initialize();

  /** Algorithm execution.
   *  Repeat the following sequence until a good set of interactions is 
   *  generated.
   *  -# Compute the number of interactions to generate in the event, using
   *     the IPileUpTool.
   *  -# Generate this number of interactions using the IProductionTool. 
   *     The IProductionTool indicates if a good event has been generated
   *     (containing for example a B hadron).
   *  -# Decay with the IDecayTool all particles in the event, if this is
   *     a good event.
   *  -# Apply the full event generator level cut (using IFullEventGenCutTool)
   *     and accept or reject the event.
   *  -# Store in event store the accepted event.
   */
  virtual StatusCode execute   ();

  /** Algorithm finalization.
   *  Print generation counters.
   */
  virtual StatusCode finalize  ();

protected:
  /// Decay the event with the IDecayTool.
  StatusCode decayEvent( LHCb::HepMCEvent * theEvent ) ;

private:
  int          m_eventType ; ///< Event type (set by options)

  /// Location where to store generator events (set by options)
  std::string  m_hepMCEventLocation ; 

  /// Location where to store the Header of the events (set by options)
  std::string  m_genHeaderLocation ;

  /// Location where to store HardInfo (set by options)
  std::string  m_genCollisionLocation ;

  IPileUpTool              * m_pileUpTool             ; ///< Pile-up tool

  IDecayTool               * m_decayTool              ; ///< Decay tool

  ICounterLogFile          * m_xmlLogTool             ; ///< Xml Log file tool

  ISampleGenerationTool    * m_sampleGenerationTool   ; ///< Sample tool

  IVertexSmearingTool      * m_vertexSmearingTool     ; ///< Smearing tool

  IFullGenEventCutTool     * m_fullGenEventCutTool    ; ///< Cut tool

  /// Name of the IPileUpTool (set by options).
  std::string m_pileUpToolName           ;

  /// Name of the ISampleGenerationTool - MinimumBias, ... (set by options)
  std::string m_sampleGenerationToolName ;

  /// Name of the IDecayTool (set by options)
  std::string m_decayToolName            ;

  /// Name of the IVertexSmearingTool (set by options)
  std::string m_vertexSmearingToolName   ;

  /// Name of the IFullGenEventCutTool (set by options)
  std::string m_fullGenEventCutToolName  ;

  /// Flag to generate all pile up events at the same PV (set by options)
  bool m_commonVertex ;

  unsigned int m_nEvents ; ///< Number of generated events

  unsigned int m_nAcceptedEvents ; ///< Number of accepted events

  unsigned int m_nInteractions ; ///< Number of generated interactions

  /// Number of interactions in accepted events
  unsigned int m_nAcceptedInteractions ;

  /// Description of the counter index
  enum interationCounterType{ Oneb = 0 , ///< interaction with >= 1 b quark
                              Threeb , ///< interaction with >= 3 b quarks
                              PromptB , ///< interaction with prompt B
                              Onec , ///< interaction with >= 1 c quark
                              Threec , ///< interaction with >= 3 c quarks
                              PromptC , ///< interaction with prompt C
                              bAndc ///< interaction with b and c
  } ;  

  /// Type for interaction counter
  typedef boost::array< unsigned int , 7 > interactionCounter ;
  typedef boost::array< std::string  , 7 > interactionCNames  ;

  /// Counter of content of generated interactions
  interactionCounter m_intC ; 

  /// Array of counter names
  interactionCNames  m_intCName ;

  /// Counter of content of accepted interactions
  interactionCounter m_intCAccepted ;

  /// Array of accepted counter names
  interactionCNames  m_intCAcceptedName ;

  /// Counter of events before the full event generator level cut  
  unsigned int m_nBeforeFullEvent ;

  /// Counter of events after the full event generator level cut
  unsigned int m_nAfterFullEvent ;

  /** Update the counters counting on interactions.
   *  @param[in,out] theCounter Counter of events
   *  @param[in]     theEvent  The interaction to study
   */
  void updateInteractionCounters( interactionCounter & theCounter ,
                                  const LHCb::HepMCEvent * theEvent ) ;

  
};
#endif // GENERATORS_GENERATION_H
