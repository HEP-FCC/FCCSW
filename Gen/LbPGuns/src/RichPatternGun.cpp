// $Id: RichPatternGun.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $

// local
#include "RichPatternGun.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event model classes
#include "Event/HepMCEvent.h"
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"
#include "HepMC/GenEvent.h"

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( RichPatternGun )

//===========================================================================
// Constructor
//===========================================================================
RichPatternGun::RichPatternGun( const std::string& name, 
                                ISvcLocator* pSvcLocator ) : 
  GaudiAlgorithm(name, pSvcLocator) , 
  m_curx( 0 ) , 
  m_cury( 0 ) ,
  m_minx( -400 ) , 
  m_miny( -300 ) ,
  m_maxx(  400 ) ,
  m_maxy(  300 ) 
{
  declareProperty("EventType"     , m_eventType = 0 ) ;
  declareProperty("HepMCEvents",    
                  m_eventLoc = LHCb::HepMCEventLocation::Default );
  declareProperty("GenHeader",      
                  m_headerLoc = LHCb::GenHeaderLocation::Default );
  declareProperty("GenCollisions",  
                  m_collLoc = LHCb::GenCollisionLocation::Default );
  
  declareProperty("Mean"   , m_mean = 0 ) ;
  declareProperty("XSigma" , m_xsigma = 5 ) ;
  declareProperty("YSigma" , m_ysigma = 5 ) ;
  declareProperty("Peak"   , m_peak = 1 ) ;
  
  declareProperty( "NParticles" , m_nParticles = 1 ) ;
  declareProperty( "xPatternCentre" , m_xvtx = 0. * Gaudi::Units::mm ) ;
  declareProperty( "yPatternCentre" , m_yvtx = 0. * Gaudi::Units::mm ) ;
  declareProperty( "zPatternCentre" , m_zvtx = 5000. * Gaudi::Units::mm ) ;
}

//===========================================================================
// Destructor
//===========================================================================
RichPatternGun::~RichPatternGun() { } 

//===========================================================================
// Initialization
//===========================================================================
StatusCode RichPatternGun::initialize() {

  // Initialize the base class
  StatusCode sc = GaudiAlgorithm::initialize( ) ;  
  if ( sc.isFailure( ) ) return sc ;
  
  info() << "Generate event of type " << m_eventType << endmsg;

  sc = m_particleGenerator.initialize( randSvc() , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Cannot initialize flat generator" ) ;

  return sc ;
}

//===========================================================================
// Execute method
//===========================================================================
StatusCode RichPatternGun::execute() {

  debug() << "Processing event type " << m_eventType << endmsg ;
  
  LHCb::HepMCEvents * anhepMCVector = new LHCb::HepMCEvents();
  put( anhepMCVector, m_eventLoc );
  
  LHCb::GenCollisions* collVector = new LHCb::GenCollisions();
  put( collVector, m_collLoc );
    
  double weight =0;

  // Update the GenHeader 
  LHCb::GenHeader* genHead = get< LHCb::GenHeader >( m_headerLoc ) ;
  genHead->setEvType( m_eventType );
  
  for ( m_curx = m_minx ; m_curx < m_maxx ; m_curx++ ) {
    for ( m_cury = m_miny ; m_cury < m_maxy ; m_cury++ ) {
      if ( m_curx==m_maxx || m_cury==m_maxy ||
           m_curx==m_minx || m_cury==m_miny ||
          (m_curx%24)==0  ||(m_cury%24)==0||
          (((m_curx%4)==0 && (m_curx%24)!=0) && 
           ((m_cury%4)==0 && (m_cury%24)!=0)) ||
          ((((m_curx-2)%4)==0 && ((m_curx-2)%24)!=0 && ((m_curx-6)%24)) &&           
           (((m_cury-2)%4)==0 && ((m_cury-2)%24)!=0 && ((m_cury-6)%24)))
           ) {
        weight = (exp(-((m_curx - m_mean)*(m_curx - m_mean))/(2*(m_xsigma)*(m_xsigma))))*
                 (exp(-((m_cury - m_mean)*(m_cury - m_mean))/(2*(m_ysigma)*(m_ysigma))));
        weight = weight * m_peak;
        
        int particles = (int) (weight * m_particleGenerator());
        
        for(int ip=0;ip<particles;ip++){
          LHCb::HepMCEvent* mcevt = new LHCb::HepMCEvent() ;
          mcevt -> setGeneratorName( name() ) ;
          HepMC::GenEvent* evt = mcevt -> pGenEvt() ;
          // Generate an event
          StatusCode sc = callParticleGun( evt ) ;
          if( ! sc.isSuccess( ) ) return Error( "Failed to generate event" , sc ) ;
          
          anhepMCVector->insert( mcevt );
          
          LHCb::GenCollision* coll = new LHCb::GenCollision();
          coll->setIsSignal( false ) ;
          coll->setProcessType( evt -> signal_process_id() ) ;
          coll->setEvent( mcevt ) ;
          collVector->insert( coll ) ;
          
          genHead->addToCollisions( coll );
        }
      }
    }
  } 
  return StatusCode::SUCCESS;
}
  
//=========================================================================
// Generate one particle
//=========================================================================
StatusCode RichPatternGun::callParticleGun( HepMC::GenEvent * evt ) {  
  unsigned int iPart ;
  for ( iPart = 0; iPart < m_nParticles ; ++iPart ) {
    double px = 0. ; 
    double py = 0. ;
    double pz = 4.*Gaudi::Units::eV ;
    
    double energy = sqrt( px*px + py*py + pz*pz ) ;
    HepMC::FourVector fourMom( px , py , pz , energy ) ;
    
    const HepMC::FourVector vtx( m_curx+m_xvtx ,
                                 m_cury+m_yvtx,
                                 m_zvtx,
                                 0.);
    
    HepMC::GenVertex * v1 = new HepMC::GenVertex( vtx );
    evt -> add_vertex( v1 );
    v1  -> add_particle_out( new HepMC::GenParticle( fourMom , 
                                                     10000022 , 
                                                     LHCb::HepMCEvent::StableInProdGen ) );    
  } 
  
  evt -> set_signal_process_id( m_nParticles ) ;
  
  return StatusCode::SUCCESS ;
}
