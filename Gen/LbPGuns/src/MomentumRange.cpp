// $Id: MomentumRange.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $

// This class
#include "MomentumRange.h"

// From STL
#include <cmath>

// FromGaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/IParticlePropertySvc.h"
#include "Event/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IRndmGenSvc.h" 

//===========================================================================
// Implementation file for class: MomentumRange
//
// 2008-05-18: Patrick Robbe adaptation to tool structure
//===========================================================================

DECLARE_TOOL_FACTORY( MomentumRange )

//===========================================================================
// Constructor
//===========================================================================
MomentumRange::MomentumRange( const std::string & type ,
                              const std::string & name , 
                              const IInterface * parent )
  : GaudiTool( type , name, parent ) {
    declareInterface< IParticleGunTool >( this ) ;
    declareProperty( "MomentumMin"  , m_minMom = 100.0 * Gaudi::Units::GeV ) ;
    declareProperty( "ThetaMin"     , m_minTheta = 0.1 * Gaudi::Units::rad ) ;
    declareProperty( "PhiMin"       , m_minPhi = 0. * Gaudi::Units::rad ) ;
    
    declareProperty( "MomentumMax" , m_maxMom   = 100.0 * Gaudi::Units::GeV ) ;
    declareProperty( "ThetaMax"     , m_maxTheta = 0.4 * Gaudi::Units::rad ) ;
    declareProperty( "PhiMax"      , m_maxPhi   = Gaudi::Units::twopi * Gaudi::Units::rad ) ;

    m_pdgCodes.push_back( -211 ); // default pi-
    declareProperty("PdgCodes",m_pdgCodes);
}

//===========================================================================
// Destructor
//===========================================================================
MomentumRange::~MomentumRange() { }

//===========================================================================
// Initialize Particle Gun parameters
//===========================================================================
StatusCode MomentumRange::initialize() {
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

  // check momentum and angles
  if ( ( m_minMom   > m_maxMom ) || ( m_minTheta > m_maxTheta ) || 
       ( m_minPhi   > m_maxPhi ) )
    return Error( "Incorrect values for momentum, theta or phi!" ) ;
  
  // setup particle information
  m_masses.clear();

  info() << "Particle type chosen randomly from :";
  PIDs::iterator icode ;
  for ( icode = m_pdgCodes.begin(); icode != m_pdgCodes.end(); ++icode ) {
    const LHCb::ParticleProperty * particle = ppSvc->find( LHCb::ParticleID( *icode ) );
    m_masses.push_back( ( particle->mass() ) ) ;
    m_names.push_back( particle->particle() ) ;
    info() << " " << particle->particle() ;
  }
  
  info() << endmsg ;

  info() << "Momentum range: " << m_minMom / Gaudi::Units::GeV << " GeV <-> " 
          << m_maxMom / Gaudi::Units::GeV << " GeV" << endmsg ;
  info() << "Theta range: " << m_minTheta / Gaudi::Units::rad << " rad <-> " 
          << m_maxTheta / Gaudi::Units::rad << " rad" << endmsg ;
  info() << "Phi range: " << m_minPhi / Gaudi::Units::rad << " rad <-> " 
          << m_maxPhi / Gaudi::Units::rad << " rad" << endmsg ;
  
  release( ppSvc ) ;

  return sc ;
}

//===========================================================================
// Generate the particles
//===========================================================================
void MomentumRange::generateParticle( Gaudi::LorentzVector & momentum , 
                                      Gaudi::LorentzVector & origin , 
                                      int & pdgId ) {  
  
  origin.SetCoordinates( 0. , 0. , 0. , 0.  );                                      
  double px(0.), py(0.), pz(0.) ;
      
  // Generate values for energy, theta and phi
  double p = m_minMom + m_flatGenerator() * (m_maxMom-m_minMom) ;
  double theta = m_minTheta + m_flatGenerator() * (m_maxTheta-m_minTheta) ;
  double phi = m_minPhi + m_flatGenerator() * (m_maxPhi-m_minPhi);
  
  // Transform to x,y,z coordinates
  double pt = p*sin(theta);
  px = pt*cos(phi);
  py = pt*sin(phi);
  pz = p*cos(theta);
    
  // randomly choose a particle type
  unsigned int currentType = 
    (unsigned int)( m_pdgCodes.size() * m_flatGenerator() );
  // protect against funnies
  if ( currentType >= m_pdgCodes.size() ) currentType = 0; 

  momentum.SetPx( px ) ; momentum.SetPy( py ) ; momentum.SetPz( pz ) ;
  momentum.SetE( std::sqrt( m_masses[currentType] * m_masses[currentType] + 
                            momentum.P2() ) ) ;  
                        
  pdgId = m_pdgCodes[ currentType ] ;
    
  debug() << " -> " << m_names[ currentType ] << endmsg 
          << "   P   = " << momentum << endmsg ;
}

