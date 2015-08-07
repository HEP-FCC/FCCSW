// $Id: FixedMomentum.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $

// This class
#include "FixedMomentum.h"

// From STL
#include <cmath>

// FromGaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/IParticlePropertySvc.h"
#include "Event/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IRndmGenSvc.h" 

//===========================================================================
// Implementation file for class: FixedMomentum
//
// 2008-05-18: Patrick Robbe adaptation to tool structure
//===========================================================================

DECLARE_TOOL_FACTORY( FixedMomentum )

//===========================================================================
// Constructor
//===========================================================================
FixedMomentum::FixedMomentum( const std::string & type ,
                              const std::string & name , 
                              const IInterface * parent )
  : GaudiTool( type , name, parent ) {
    declareInterface< IParticleGunTool >( this ) ;
      
    m_pdgCodes.push_back( -211 ); // default pi-
    declareProperty("PdgCodes",m_pdgCodes);

    declareProperty("px", m_px = 1.0 * Gaudi::Units::GeV ) ;
    declareProperty("py", m_py = 1.0 * Gaudi::Units::GeV ) ;
    declareProperty("pz", m_pz = 1.0 * Gaudi::Units::GeV ) ;
}

//===========================================================================
// Destructor
//===========================================================================
FixedMomentum::~FixedMomentum() { }

//===========================================================================
// Initialize Particle Gun parameters
//===========================================================================
StatusCode FixedMomentum::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return sc ;

  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_flatGenerator.initialize( randSvc , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Cannot initialize flat generator" ) ;
  
  // Get the mass of the particle to be generated
  //
  LHCb::IParticlePropertySvc* ppSvc = 
    svc< LHCb::IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;

  // setup particle information
  m_masses.clear();

  info() << "Particle type chosen randomly from :";
  PIDs::iterator icode ;
  for ( icode = m_pdgCodes.begin(); icode != m_pdgCodes.end(); ++icode ) {
    const LHCb::ParticleProperty * particle = 
      ppSvc->find( LHCb::ParticleID( *icode ) ) ;
    m_masses.push_back( ( particle->mass() ) ) ;
    m_names.push_back( particle->particle() ) ;
    info() << " " << particle->particle() ;
  }
  
  info() << endmsg ;

  info() << "Momentum: (" << m_px / Gaudi::Units::GeV << " GeV, " 
          << m_py / Gaudi::Units::GeV << " GeV, " << m_pz / Gaudi::Units::GeV << " GeV)" 
          << endmsg ;
 
  release( ppSvc ) ;

  return sc ;
}

//===========================================================================
// Generate the particles
//===========================================================================
void FixedMomentum::generateParticle( Gaudi::LorentzVector & momentum , 
                                      Gaudi::LorentzVector & origin , 
                                      int & pdgId ) {  
  unsigned int currentType = 
    (unsigned int)( m_pdgCodes.size() * m_flatGenerator() );
  // protect against funnies
  if ( currentType >= m_pdgCodes.size() ) currentType = 0; 

  origin.SetCoordinates( 0. , 0. , 0. , 0. ) ;  
  
  momentum.SetPx( m_px ) ; momentum.SetPy( m_py ) ; momentum.SetPz( m_pz ) ;
  momentum.SetE( std::sqrt( m_masses[currentType] * m_masses[currentType] + 
                            momentum.P2() ) ) ;  
                        
  pdgId = m_pdgCodes[ currentType ] ;                      
    
  debug() << " -> " << m_names[ currentType ] << endmsg 
          << "   P   = " << momentum << endmsg ;
}

