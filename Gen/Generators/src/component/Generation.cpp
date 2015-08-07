// $Id: Generation.cpp,v 1.33 2009-06-15 17:02:35 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Event
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"

// from LHCb
#include "Event/ParticleID.h"
#include "MCInterfaces/IFullGenEventCutTool.h"
#include "MCInterfaces/IDecayTool.h" 

// from Generators
#include "Generators/ISampleGenerationTool.h"
#include "Generators/IPileUpTool.h"
#include "Generators/IVertexSmearingTool.h"
#include "Generators/ICounterLogFile.h"
#include "Generators/GenCounters.h"
#include "Event/HepMCUtils.h"

// Gaudi Common Flat Random Number generator
#include "Generators/RandomForGenerator.h"

// local
#include "Generation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Generation
//
// 2005-08-11 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( Generation )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Generation::Generation( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) ,
    m_pileUpTool           ( 0 ) ,
    m_decayTool            ( 0 ) ,
    m_xmlLogTool           ( 0 ) ,
    m_sampleGenerationTool ( 0 ) ,
    m_vertexSmearingTool   ( 0 ) ,
    m_fullGenEventCutTool  ( 0 ) ,
    m_nEvents              ( 0 ) , 
    m_nAcceptedEvents      ( 0 ) ,
    m_nInteractions        ( 0 ) , 
    m_nAcceptedInteractions( 0 ) ,
    m_nBeforeFullEvent     ( 0 ) ,
    m_nAfterFullEvent      ( 0 ) {
    // Generation Method
    declareProperty ( "SampleGenerationTool" , 
                      m_sampleGenerationToolName = "MinimumBias" ) ;
    declareProperty ( "EventType"          , m_eventType = 30000000 ) ;

    // Location of the output of the generation
    declareProperty ( "HepMCEventLocation" , m_hepMCEventLocation = 
                      LHCb::HepMCEventLocation::Default ) ;
    declareProperty ( "GenHeaderLocation"  , m_genHeaderLocation = 
                      LHCb::GenHeaderLocation::Default ) ;
    declareProperty ( "GenCollisionLocation" , m_genCollisionLocation = 
                      LHCb::GenCollisionLocation::Default ) ;
    
    // Tool name to generate the event
    declareProperty( "PileUpTool" , m_pileUpToolName = "FixedLuminosity" ) ;
    // Tool name to decay particles in the event
    declareProperty( "DecayTool"  , m_decayToolName = "EvtGenDecay" ) ;
    // Tool name to smear vertex
    declareProperty( "VertexSmearingTool" , 
                     m_vertexSmearingToolName = "BeamSpotSmearVertex" ) ;
    // Tool name to cut on full event
    declareProperty( "FullGenEventCutTool" , 
                     m_fullGenEventCutToolName = "" ) ;
    // Flag to attach all pile up events to the same PV
    declareProperty( "CommonVertex" , m_commonVertex = false ) ;

    // Reset counters
    m_intC.assign( 0 ) ;
    m_intCAccepted.assign( 0 ) ;
    // setup counter names
    m_intCName[ Oneb ] = "generated interactions with >= 1b" ;    
    m_intCName[ Threeb ] = "generated interactions with >= 3b" ;
    m_intCName[ PromptB ] = "generated interactions with 1 prompt B" ;
    m_intCName[ Onec ] = "generated interactions with >= 1c" ;
    m_intCName[ Threec ] = "generated interactions with >= 3c" ;
    m_intCName[ PromptC ] = "generated interactions with >= prompt C" ;
    m_intCName[ bAndc ] = "generated interactions with b and c" ;

    m_intCAcceptedName[ Oneb ] = "accepted interactions with >= 1b" ;    
    m_intCAcceptedName[ Threeb ] = "accepted interactions with >= 3b" ;
    m_intCAcceptedName[ PromptB ] = "accepted interactions with 1 prompt B" ;
    m_intCAcceptedName[ Onec ] = "accepted interactions with >= 1c" ;
    m_intCAcceptedName[ Threec ] = "accepted interactions with >= 3c" ;
    m_intCAcceptedName[ PromptC ] = "accepted interactions with >= prompt C" ;
    m_intCAcceptedName[ bAndc ] = "accepted interactions with b and c" ;

}

//=============================================================================
// Destructor
//=============================================================================
Generation::~Generation() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Generation::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize( ) ; // Initialize base class
  if ( sc.isFailure() ) return sc ;
  debug() << "==> Initialise" << endmsg ;

  // Initialization of the Common Flat Random generator if not already done
  // This generator must be used by all external MC Generator
  if ( ! ( RandomForGenerator::getNumbers() ) ) {
    sc = RandomForGenerator::getNumbers().initialize( randSvc( ) , 
                                                      Rndm::Flat( 0 , 1 ) ) ;
    if ( ! sc.isSuccess( ) )
      return Error( "Could not initialize Rndm::Flat" , sc ) ;
  }

  // Retrieve pile up tool
  if ( "" == m_pileUpToolName ) {
    info() << "No Pile Up Tool is defined. Will generate no pile-up " 
           << endmsg ;
    info() << "and set luminosity in header to 2e32" << endmsg ;
  } else m_pileUpTool = tool< IPileUpTool >( m_pileUpToolName , this ) ;

  // Retrieve decay tool
  if ( "" != m_decayToolName ) m_decayTool = 
    tool< IDecayTool >( m_decayToolName ) ;

  // Retrieve xml log file tool for generator statistics
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  // Retrieve generation method tool
  if ( "" == m_sampleGenerationToolName ) 
    return Error( "No Sample Generation Tool is defined. This is mandatory" ) ;
  m_sampleGenerationTool = 
    tool< ISampleGenerationTool >( m_sampleGenerationToolName , this ) ;
  
  // Retrieve vertex smearing tool
  if ( "" != m_vertexSmearingToolName ) 
    m_vertexSmearingTool = 
      tool< IVertexSmearingTool >( m_vertexSmearingToolName , this ) ;
  
  // Retrieve full gen event cut tool
  if ( "" != m_fullGenEventCutToolName ) m_fullGenEventCutTool =
    tool< IFullGenEventCutTool >( m_fullGenEventCutToolName , this ) ;

  // Message relative to event type
  always()
    << "=================================================================="
    << endmsg;
  always()
    << "Requested to generate EventType " << m_eventType << endmsg;
  always()
    << "=================================================================="
    << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Generation::execute() {

  debug() << "Processing event type " << m_eventType << endmsg ;
  StatusCode sc = StatusCode::SUCCESS ;
  setFilterPassed( true ) ;

  // Get the header and update the information
  LHCb::GenHeader* theGenHeader = get<LHCb::GenHeader> ( m_genHeaderLocation );
  if( !theGenHeader->evType() ){
    theGenHeader -> setEvType( m_eventType );  
  }

  unsigned int  nPileUp( 0 ) ;
  
  LHCb::HepMCEvents::iterator itEvents ;

  // Create temporary containers for this event
  LHCb::HepMCEvents* theEvents = new LHCb::HepMCEvents( );
  LHCb::GenCollisions* theCollisions = new LHCb::GenCollisions( );

  interactionCounter theIntCounter ;

  // Generate a set of interaction until a good one is found
  bool goodEvent = false ;
  while ( ! goodEvent ) {
    theEvents->clear() ;
    theCollisions->clear() ;
    
    // Compute the number of pile-up interactions to generate 
    if ( 0 != m_pileUpTool ) 
      nPileUp = m_pileUpTool -> numberOfPileUp( ) ;
    else 
      // default set to 1 pile and 2.10^32 luminosity
      nPileUp = 1 ;

    // generate a set of Pile up interactions according to the requested type
    // of event
    if ( 0 < nPileUp ) 
      goodEvent = m_sampleGenerationTool -> generate( nPileUp, theEvents, 
                                                      theCollisions );
    else { 
      goodEvent = true ;
      setFilterPassed( false ) ;
    }
    
    // increase event and interactions counters
    ++m_nEvents ;    m_nInteractions += nPileUp ;

    // Update interaction counters
    if ( 0 < nPileUp ) { 
      theIntCounter.assign( 0 ) ;
      for ( itEvents = theEvents->begin() ; itEvents != theEvents->end() ; 
            ++itEvents ) updateInteractionCounters( theIntCounter , *itEvents ) ;
    
      GenCounters::AddTo( m_intC , theIntCounter ) ;

      // Decay the event if it is a good event
      if ( goodEvent ) {
        unsigned short iPile( 0 ) ;
        for ( itEvents = theEvents->begin() ; itEvents != theEvents->end() ;
              ++itEvents ) {
          if ( 0 != m_decayTool ) {
            sc = decayEvent( *itEvents ) ;
            if ( ! sc.isSuccess() ) goodEvent = false ;
          }
          (*itEvents) -> pGenEvt() -> set_event_number( ++iPile ) ;
          if ( ( ! ( m_commonVertex ) ) || ( 1 == iPile ) )
            sc = m_vertexSmearingTool -> smearVertex( *itEvents ) ;
          if ( ! sc.isSuccess() ) return sc ;
        }
      }

      if ( ( m_commonVertex ) && ( 1 < nPileUp ) ) {
        HepMC::FourVector commonV = 
          ((*(theEvents->begin())) ->pGenEvt() -> beam_particles().first) -> end_vertex() -> position() ;
        for ( itEvents = (theEvents->begin()+1) ; itEvents != theEvents -> end() ;
              ++itEvents ) {
          HepMC::GenEvent::vertex_iterator vit ;
          HepMC::GenEvent * pEvt = (*itEvents) -> pGenEvt() ;
          for ( vit = pEvt -> vertices_begin() ; vit != pEvt -> vertices_end() ; 
                ++vit ) {
            HepMC::FourVector pos = (*vit) -> position() ;
            (*vit) -> set_position( HepMC::FourVector( pos.x() + commonV.x() , 
                                                       pos.y() + commonV.y() , 
                                                       pos.z() + commonV.z() , 
                                                       pos.t() + commonV.t() ) ) ;
          }
        } 
      }
      

      // Apply generator level cut on full event
      if ( m_fullGenEventCutTool ) {
        if ( goodEvent ) {
          ++m_nBeforeFullEvent ;
          goodEvent = m_fullGenEventCutTool -> studyFullEvent( theEvents , 
                                                             theCollisions );
          if ( goodEvent ) ++m_nAfterFullEvent ;
        }
      }
    }
  }  

  ++m_nAcceptedEvents ;
  m_nAcceptedInteractions += nPileUp ;
  
  LHCb::HepMCEvents* eventsInTES( 0 )  ;
  LHCb::GenCollisions* collisionsInTES( 0 ) ;
  if ( 0 < nPileUp ) {
    GenCounters::AddTo( m_intCAccepted , theIntCounter ) ;

    // Now either create the info in the TES or add it to the existing one
    eventsInTES = 
      getOrCreate<LHCb::HepMCEvents,LHCb::HepMCEvents>( m_hepMCEventLocation );

    collisionsInTES = 
      getOrCreate<LHCb::GenCollisions,LHCb::GenCollisions>( m_genCollisionLocation );
  }
    
  // Copy the HepMCevents and Collisions from the temporary containers to 
  // those in TES and update the header information

  // Check that number of temporary HepMCEvents is the same as GenCollisions
  if( theEvents->size() != theCollisions->size() ) {
    return Error("Number of HepMCEvents and GenCollisions do not match" );
  }

  itEvents = theEvents->begin();
  if ( 0 < nPileUp ) {
    for( LHCb::GenCollisions::const_iterator it = theCollisions->begin();
         theCollisions->end() != it; ++it ) {
      
      // HepMCEvent
      LHCb::HepMCEvent* theHepMCEvent = new LHCb::HepMCEvent();
      theHepMCEvent->setGeneratorName( (*itEvents)->generatorName() );
      (*theHepMCEvent->pGenEvt()) = (*(*itEvents)->pGenEvt());
      eventsInTES->insert( theHepMCEvent );
      ++itEvents;
      
      // GenCollision
      LHCb::GenCollision* theGenCollision = new LHCb::GenCollision();
      theGenCollision->setIsSignal( (*it)->isSignal() );
      theGenCollision->setProcessType( (*it)->processType() );
      theGenCollision->setSHat( (*it)->sHat() );
      theGenCollision->setTHat( (*it)->tHat() );
      theGenCollision->setUHat( (*it)->uHat() );
      theGenCollision->setPtHat( (*it)->ptHat() );
      theGenCollision->setX1Bjorken( (*it)->x1Bjorken() );
      theGenCollision->setX2Bjorken( (*it)->x2Bjorken() );
      theGenCollision->setEvent( theHepMCEvent );
      collisionsInTES->insert( theGenCollision );
      
      // GenHeader
      theGenHeader->addToCollisions( theGenCollision );
      
    }
  }

  // Clear and delete the temporary containers
  theEvents->clear();
  theCollisions->clear();
  delete(theEvents);
  delete(theCollisions);

  return sc ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Generation::finalize() {
  using namespace GenCounters ;
  debug( ) << "==> Finalize" << endmsg ;
  // Add the event type number to the log file
  m_xmlLogTool -> addEventType( m_eventType ) ;
  
  // Print the various counters
  if ( 0 != m_pileUpTool ) { m_pileUpTool -> printPileUpCounters( ) ; }

  printCounter( m_xmlLogTool , "generated events" , m_nEvents ) ;
  printCounter( m_xmlLogTool , "generated interactions" , m_nInteractions ) ;
  
  for ( unsigned int i = 0 ; i < m_intC.size() ; ++i )
    printCounter( m_xmlLogTool , m_intCName[ i ] , m_intC[ i ] ) ;
  
  printCounter( m_xmlLogTool , "accepted events" , m_nAcceptedEvents ) ;
  printCounter( m_xmlLogTool , "interactions in accepted events" , 
                m_nAcceptedInteractions ) ;
  
  for ( unsigned int j = 0 ; j < m_intCAccepted.size() ; ++j ) 
    printCounter( m_xmlLogTool , m_intCAcceptedName[ j ] , 
                  m_intCAccepted[ j ] ) ;

  printEfficiency( m_xmlLogTool , "full event cut" , m_nAfterFullEvent , 
                   m_nBeforeFullEvent ) ;

  m_sampleGenerationTool -> printCounters() ;

  if ( 0 != m_pileUpTool ) release( m_pileUpTool ) ;
  if ( 0 != m_decayTool ) release( m_decayTool ) ;
  if ( 0 != m_sampleGenerationTool ) release( m_sampleGenerationTool ) ;
  if ( 0 != m_vertexSmearingTool ) release( m_vertexSmearingTool ) ;
  if ( 0 != m_fullGenEventCutTool ) release( m_fullGenEventCutTool ) ;
  
  return GaudiAlgorithm::finalize( ) ; // Finalize base class
}

//=============================================================================
// Decay in the event all particles which have been left stable by the
// production generator
//=============================================================================
StatusCode Generation::decayEvent( LHCb::HepMCEvent * theEvent ) {
  using namespace LHCb;
  m_decayTool -> disableFlip() ;
  StatusCode sc ;
  
  HepMC::GenEvent * pEvt = theEvent -> pGenEvt() ;

  // We must use particles_begin to obtain an ordered iterator of GenParticles
  // according to the barcode: this allows to reproduce events !
  HepMCUtils::ParticleSet pSet( pEvt -> particles_begin() , 
                                pEvt -> particles_end() ) ;

  HepMCUtils::ParticleSet::iterator itp ;

  for ( itp = pSet.begin() ; itp != pSet.end() ; ++itp ) {
    
    HepMC::GenParticle * thePart = (*itp) ;
    unsigned int status = thePart -> status() ;
    
    if ( ( HepMCEvent::StableInProdGen  == status ) || 
         ( ( HepMCEvent::DecayedByDecayGenAndProducedByProdGen == status )
           && ( 0 == thePart -> end_vertex() ) ) ) {
      
      if ( m_decayTool -> isKnownToDecayTool( thePart -> pdg_id() ) ) {
        
        if ( HepMCEvent::StableInProdGen == status ) 
          thePart -> 
            set_status( HepMCEvent::DecayedByDecayGenAndProducedByProdGen ) ;
        else thePart -> set_status( HepMCEvent::DecayedByDecayGen ) ;
        
        sc = m_decayTool -> generateDecay( thePart ) ;
        if ( ! sc.isSuccess() ) return sc ;
      }
    } 
  }  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Interaction counters
//=============================================================================
void Generation::updateInteractionCounters( interactionCounter & theCounter ,
                                            const LHCb::HepMCEvent * evt ) 
{
  const HepMC::GenEvent * theEvent = evt -> pGenEvt() ;
  unsigned int bQuark( 0 ) , bHadron( 0 ) , cQuark( 0 ) , cHadron( 0 ) ;
  int pdgId ;

  HepMC::GenEvent::particle_const_iterator iter ;
  for ( iter = theEvent -> particles_begin() ; 
        theEvent -> particles_end() != iter ; ++iter ) {
    if ( ( (*iter) -> status() == LHCb::HepMCEvent::DocumentationParticle ) ||
         ( (*iter) -> status() == LHCb::HepMCEvent::DecayedByDecayGen ) ||
         ( (*iter) -> status() == LHCb::HepMCEvent::StableInDecayGen ) ) 
      continue ;
    pdgId = abs( (*iter) -> pdg_id() ) ;
    LHCb::ParticleID thePid( pdgId ) ;

    if ( 5 == pdgId ) { 
      if ( 0 != (*iter) -> production_vertex() ) {
        if ( 1 != (*iter) -> production_vertex() -> particles_in_size() ) {
           bool containB = false;
           for (HepMC::GenVertex::particles_in_const_iterator par = (*iter) -> production_vertex() -> particles_in_const_begin();
                par != (*iter) -> production_vertex() -> particles_in_const_end() ; par++) {
             if (5==abs((*par)->pdg_id()))
               containB = true;
           }
           if (!containB) ++bQuark ;
        } else {
          const HepMC::GenParticle * par = 
            *( (*iter) -> production_vertex() -> particles_in_const_begin() ) ;
          if ( ( par -> status() == 
                 LHCb::HepMCEvent::DocumentationParticle ) ||
               ( par -> pdg_id() != (*iter) -> pdg_id() ) ) { 
            ++bQuark ;
          }
        }
      }
    }
    else if( 4 == pdgId ) {
      if ( 0 != (*iter) -> production_vertex() ) {
        if ( 1 != (*iter) -> production_vertex() -> particles_in_size() ) {
          bool containC = false;
          for (HepMC::GenVertex::particles_in_const_iterator par = (*iter) -> production_vertex() -> particles_in_const_begin();
               par != (*iter) -> production_vertex() -> particles_in_const_end() ; par++) {
            if (4==abs((*par)->pdg_id()))
              containC = true;
          }
          if (!containC) ++cQuark ;
        } else {
          const HepMC::GenParticle * par =
            *( (*iter) -> production_vertex() -> particles_in_const_begin() ) ;
          if ( ( par -> status() ==
                 LHCb::HepMCEvent::DocumentationParticle ) ||
               ( par -> pdg_id() != (*iter) -> pdg_id() ) ) {
            ++cQuark ;
          }
        }
      }
    }
    else {
      if ( thePid.hasBottom() ) {
        // Count B from initial proton as a quark
        if ( 0 != (*iter) -> production_vertex() ) {
          if ( 0 != (*iter) -> production_vertex() -> particles_in_size() ) {
            const HepMC::GenParticle * par = 
              *( (*iter)-> production_vertex()-> particles_in_const_begin() ) ;
            if ( 0 != par -> production_vertex() ) {
              if ( 0 == par -> production_vertex() -> particles_in_size() ) {
                ++bQuark ;
              }
            } 
            else {
              ++bQuark ;
            }
          }
        }
        ++bHadron ;
      } else if ( thePid.hasCharm() ) {
        // Count D from initial proton as a quark
        if ( 0 != (*iter) -> production_vertex() ) {
          if ( 0 != (*iter) -> production_vertex() -> particles_in_size() ) {
            const HepMC::GenParticle * par = 
              *( (*iter)-> production_vertex()-> particles_in_const_begin() ) ;
            if ( 0 != par -> production_vertex() ) {
              if ( 0 == par -> production_vertex() -> particles_in_size() ) 
                ++cQuark ;
            } else ++cQuark ;
          }
        }
        ++cHadron ;
      }
    }
  }

  if ( bQuark >= 1 ) { 
    ++theCounter[ Oneb ] ;
    if ( bQuark >= 3 ) ++theCounter[ Threeb ] ;
  }
  if ( cQuark >= 1 ) {
    ++theCounter[ Onec ] ;
    if ( cQuark >= 3 ) ++theCounter[ Threec ] ;
    if ( bQuark >= 1 ) ++theCounter[ bAndc ] ;
  }
  if ( ( 0 == bQuark ) && ( bHadron > 0 ) ) ++theCounter[ PromptB ] ;
  if ( ( 0 == cQuark ) && ( 0 == bHadron ) && ( cHadron > 0 ) ) 
    ++theCounter[ PromptC ];
}

