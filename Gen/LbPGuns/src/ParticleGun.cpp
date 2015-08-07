// $Id: ParticleGun.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
// Include files

// local
#include "ParticleGun.h"

// from SEAL
#include "boost/tokenizer.hpp"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/RndmGenerators.h"

// from Event
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"

// from LHCb
#include "Event/ParticleID.h"

// from Generators
#include "Generators/IPileUpTool.h"
#include "LbPGuns/IParticleGunTool.h"
#include "MCInterfaces/IDecayTool.h"
#include "Generators/IVertexSmearingTool.h"
#include "MCInterfaces/IFullGenEventCutTool.h"
#include "MCInterfaces/IGenCutTool.h"
#include "Generators/GenCounters.h"
#include "Event/HepMCUtils.h"

// Gaudi Common Flat Random Number generator
#include "Generators/RandomForGenerator.h"


//-----------------------------------------------------------------------------
// Implementation file for class : ParticleGun
//
// 2008-05-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( ParticleGun )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleGun::ParticleGun( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) ,
    m_particleGunTool       ( 0 ) ,
    m_numberOfParticlesTool ( 0 ) ,
    m_decayTool             ( 0 ) ,
    m_sampleGenerationTool  ( 0 ) ,
    m_vertexSmearingTool    ( 0 ) ,
    m_fullGenEventCutTool   ( 0 ) ,
    m_genCutTool            ( 0 ) ,
    m_nEvents               ( 0 ) ,
    m_nAcceptedEvents       ( 0 ) ,
    m_nParticles            ( 0 ) ,
    m_nAcceptedParticles    ( 0 ) ,
    m_nBeforeFullEvent      ( 0 ) ,
    m_nAfterFullEvent       ( 0 ) ,
    m_nBeforeCut            ( 0 ) ,
    m_nAfterCut             ( 0 ) {
  // Generation Method
  declareProperty ( "ParticleGunTool" ,
                    m_particleGunToolName = "GenericGun" ) ;
  declareProperty ( "EventType"          , m_eventType = 50000000 ) ;

  // Location of the output of the generation
  declareProperty ( "HepMCEventLocation" , m_hepMCEventLocation =
                    LHCb::HepMCEventLocation::Default ) ;
  declareProperty ( "GenHeaderLocation"  , m_genHeaderLocation =
                    LHCb::GenHeaderLocation::Default ) ;
  declareProperty ( "GenCollisionLocation" , m_genCollisionLocation =
                    LHCb::GenCollisionLocation::Default ) ;

  // Tool name to generate the event
  declareProperty( "NumberOfParticlesTool" ,
                   m_numberOfParticlesToolName = "FixedNInteractions/FixedNParticles" ) ;
  // Tool name to decay particles in the event
  declareProperty( "DecayTool"  , m_decayToolName = "" ) ;
  // Tool name to smear vertex
  declareProperty( "VertexSmearingTool" ,
                   m_vertexSmearingToolName = "" ) ;
  // Tool name to cut on full event
  declareProperty( "FullGenEventCutTool" ,
                   m_fullGenEventCutToolName = "" ) ;
  // Tool name to cut on generator-level
  declareProperty( "GenCutTool" ,
                   m_genCutToolName = "" ) ;
  // Flag to generate signal
  declareProperty( "SignalPdgCode" , m_sigPdgCode = 0) ;
}

//=============================================================================
// Destructor
//=============================================================================
ParticleGun::~ParticleGun() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ParticleGun::initialize() {
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
  if ( "" != m_numberOfParticlesToolName )
    m_numberOfParticlesTool = tool< IPileUpTool >( m_numberOfParticlesToolName , this ) ;

  // Retrieve decay tool
  if ( "" != m_decayToolName ) {
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;
    if ( m_decayTool && m_sigPdgCode!=0 ) m_decayTool -> setSignal( m_sigPdgCode ) ;
  }

  // Retrieve generation method tool
  if ( "" == m_particleGunToolName )
    return Error( "No ParticleGun Generation Tool is defined. This is mandatory" ) ;
  m_particleGunTool =
    tool< IParticleGunTool >( m_particleGunToolName , this ) ;

  boost::char_separator<char> sep(".");
  boost::tokenizer< boost::char_separator<char> >
    strList( m_particleGunTool -> name() , sep ) ;

  std::string result = "" ;
  for ( boost::tokenizer< boost::char_separator<char> >::iterator
          tok_iter = strList.begin();
        tok_iter != strList.end(); ++tok_iter)
    result = (*tok_iter) ;
  m_particleGunName = result ;

  // Retrieve generation method tool
  if ( "" != m_vertexSmearingToolName )
    m_vertexSmearingTool =
      tool< IVertexSmearingTool >( m_vertexSmearingToolName , this ) ;

  // Retrieve full gen event cut tool
  if ( "" != m_fullGenEventCutToolName ) m_fullGenEventCutTool =
                                           tool< IFullGenEventCutTool >( m_fullGenEventCutToolName , this ) ;

  // Retrieve gen cut tool
  if ( "" != m_genCutToolName ) m_genCutTool =
                                           tool< IGenCutTool >( m_genCutToolName , this ) ;

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
StatusCode ParticleGun::execute() {

  debug() << "Processing event type " << m_eventType << endmsg ;
  StatusCode sc = StatusCode::SUCCESS ;

  // Get the header and update the information
  LHCb::GenHeader* theGenHeader = get<LHCb::GenHeader> ( m_genHeaderLocation );
  if( !theGenHeader->evType() ){
    theGenHeader -> setEvType( m_eventType );
  }

  unsigned int  nParticles( 0 ) ;

  LHCb::HepMCEvents::iterator itEvents ;

  // Create temporary containers for this event
  LHCb::HepMCEvents* theEvents = new LHCb::HepMCEvents( );
  LHCb::GenCollisions* theCollisions = new LHCb::GenCollisions( );

  // Working set of pointers
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;

  Gaudi::LorentzVector theFourMomentum ;
  Gaudi::LorentzVector origin ;
  int thePdgId ;

  // Generate sets of particles until a good one is found
  bool goodEvent = false ;

  while ( ! goodEvent ) {
    theEvents->clear() ;
    theCollisions->clear() ;

    // Compute the number of pile-up interactions to generate
    if ( 0 != m_numberOfParticlesTool )
      nParticles = m_numberOfParticlesTool -> numberOfPileUp( ) ;
    // default set to 1 pile and 0 luminosity
    else nParticles = 1 ;

    // generate a set of particles according to the requested type
    // of particle gun
    for ( unsigned int i = 0 ; i < nParticles ; ++i ) {
      // Prepare event container
      prepareInteraction( theEvents , theCollisions , theGenEvent , theGenCollision ) ;

      // generate one particle
      m_particleGunTool -> generateParticle( theFourMomentum , origin , thePdgId );

      // create HepMC Vertex
      HepMC::GenVertex * v =
        new HepMC::GenVertex( HepMC::FourVector( origin.X() ,
                                                 origin.Y() ,
                                                 origin.Z() ,
                                                 origin.T() ) ) ;
      // create HepMC particle
      HepMC::GenParticle * p =
        new HepMC::GenParticle( HepMC::FourVector( theFourMomentum.Px() ,
                                                   theFourMomentum.Py() ,
                                                   theFourMomentum.Pz() ,
                                                   theFourMomentum.E()  ) ,
                                thePdgId ,
                                LHCb::HepMCEvent::StableInProdGen ) ;

      v -> add_particle_out( p ) ;
      theGenEvent -> add_vertex( v ) ;
      theGenEvent -> set_signal_process_id( nParticles ) ;
      theGenEvent -> set_signal_process_vertex( v ) ;
    }

    goodEvent = true ;
    // increase event and interactions counters
    ++m_nEvents ;    m_nParticles += nParticles ;

    // Decay the event if it is a good event
    if ( 0 != m_decayTool ) {
      unsigned short iPart( 0 ) ;
      for ( itEvents = theEvents->begin() ; itEvents != theEvents->end() ;
            ++itEvents ) {
        ParticleVector theParticleList ;
        theParticleList.clear();

        HepMC::GenParticle *theSignal = decayEvent( *itEvents, theParticleList, sc) ;
        if ( ! sc.isSuccess() ) return sc;

        (*itEvents) -> pGenEvt() -> set_event_number( ++iPart ) ;

        // Add Cut tool
        bool passCut(true);
        if ( m_genCutTool && theSignal ) {
          ++m_nBeforeCut;
          passCut = m_genCutTool -> applyCut( theParticleList , theGenEvent ,
                                              theGenCollision ) ;
          // event does not pass cuts
          if ( !passCut || theParticleList.empty() ){
            
            HepMCUtils::RemoveDaughters( theSignal ) ;
            goodEvent = false;
          }
          else ++m_nAfterCut;
        }
      }
    }

    // Apply smearing of primary vertex
    if ( 0 != m_vertexSmearingTool ) {
      for ( itEvents = theEvents->begin() ; itEvents != theEvents->end() ;
            ++itEvents ) {
        sc = m_vertexSmearingTool -> smearVertex( *itEvents ) ;
        if ( ! sc.isSuccess() ) return sc ;
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

  ++m_nAcceptedEvents ;
  m_nAcceptedParticles += nParticles ;

  // Now either create the info in the TES or add it to the existing one
  LHCb::HepMCEvents* eventsInTES =
    getOrCreate<LHCb::HepMCEvents,LHCb::HepMCEvents>( m_hepMCEventLocation );

  LHCb::GenCollisions* collisionsInTES =
    getOrCreate<LHCb::GenCollisions,LHCb::GenCollisions>( m_genCollisionLocation );

  // Check that number of temporary HepMCEvents is the same as GenCollisions
  if( theEvents->size() != theCollisions->size() ) {
    return Error("Number of HepMCEvents and GenCollisions do not match" );
  }

  itEvents = theEvents->begin();
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
StatusCode ParticleGun::finalize() {
  using namespace GenCounters ;
  debug( ) << "==> Finalize" << endmsg ;
  // Print the various counters
  info() << "**************************************************" << endmsg ;
  if ( 0 != m_numberOfParticlesTool )
    m_numberOfParticlesTool -> printPileUpCounters( ) ;
  info() << "***********   Generation counters   **************" << std::endl ;
  printCounter( info() , "generated events" , m_nEvents ) ;
  printCounter( info() , "generated particles" , m_nParticles ) ;

  printCounter( info() , "accepted events" , m_nAcceptedEvents ) ;
  printCounter( info() , "particles in accepted events" ,
                m_nAcceptedParticles ) ;

  printEfficiency( info() , "full event cut" , m_nAfterFullEvent ,
                   m_nBeforeFullEvent ) ;
  printEfficiency( info() , "signal cut" , m_nAfterCut ,
                   m_nBeforeCut ) ;
  info() << endmsg ;

  m_particleGunTool -> printCounters() ;

  if ( 0 != m_numberOfParticlesTool ) release( m_numberOfParticlesTool ) ;
  if ( 0 != m_decayTool ) release( m_decayTool ) ;
  if ( 0 != m_particleGunTool ) release( m_particleGunTool ) ;
  if ( 0 != m_vertexSmearingTool ) release( m_vertexSmearingTool ) ;
  if ( 0 != m_fullGenEventCutTool ) release( m_fullGenEventCutTool ) ;
  if ( 0 != m_genCutTool ) release( m_genCutTool ) ;

  return GaudiAlgorithm::finalize( ) ; // Finalize base class
}

//=============================================================================
// Decay in the event all particles which have been left stable by the
// production generator
//=============================================================================
HepMC::GenParticle *ParticleGun::decayEvent( LHCb::HepMCEvent * theEvent,
                                             ParticleVector & theParticleList,
                                             StatusCode & sc) {
  using namespace LHCb;
  m_decayTool -> disableFlip() ;
  sc = StatusCode::SUCCESS ;
  HepMC::GenParticle *theSignal(0);

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

        if ( abs(m_sigPdgCode) == abs(thePart->pdg_id()) ) {
          bool hasFlipped(false);
          sc = m_decayTool -> generateSignalDecay( thePart, hasFlipped ) ;
          theSignal = thePart;
        } else
          sc = m_decayTool -> generateDecay( thePart ) ;

        theParticleList.push_back( thePart );

        if ( ! sc.isSuccess() ) return 0 ;
      }
    }
  }
  return theSignal ;
}

//=============================================================================
// Set up event
//=============================================================================
void ParticleGun::prepareInteraction( LHCb::HepMCEvents * theEvents ,
                                      LHCb::GenCollisions * theCollisions ,
                                      HepMC::GenEvent * & theGenEvent ,
                                      LHCb::GenCollision * & theGenCollision ) const
{
  LHCb::HepMCEvent * theHepMCEvent = new LHCb::HepMCEvent( ) ;
  theHepMCEvent -> setGeneratorName( m_particleGunName ) ;
  theGenEvent = theHepMCEvent -> pGenEvt() ;

  theGenCollision = new LHCb::GenCollision() ;
  theGenCollision -> setEvent( theHepMCEvent ) ;
  theGenCollision -> setIsSignal( (m_sigPdgCode!=0) ) ;

  theEvents -> insert( theHepMCEvent ) ;
  theCollisions -> insert( theGenCollision ) ;
}
