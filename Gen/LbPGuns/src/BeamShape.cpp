// Include files

// This class
#include "BeamShape.h"

// From STL
#include <cmath>

// From Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/IParticlePropertySvc.h"
#include "Event/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IRndmGenSvc.h"

//===========================================================================
// Implementation file for class: BeamShape
//
// 2009-10-21: Magnus Lieng
//===========================================================================

DECLARE_TOOL_FACTORY( BeamShape )

//===========================================================================
// Constructor
//===========================================================================
BeamShape::BeamShape( const std::string& type ,
                  const std::string& name , 
                  const IInterface* parent )
  : GaudiTool( type , name, parent ) , 
    m_xSigma( 0. ) , 
    m_ySigma( 0. ) ,
    m_pxCenter( 0. ) ,
    m_pyCenter( 0. ) ,
    m_pxSigma( 0. ) , 
    m_pySigma( 0. ) ,
    m_mass( 0. ) 
{
    declareInterface< IParticleGunTool >( this ) ;

    // Beam Centroid
    // zCenter gives point of generation 
    declareProperty( "xCenter", m_xCenter = 0.0 * Gaudi::Units::mm ) ;
    declareProperty( "yCenter", m_yCenter = 0.0 * Gaudi::Units::mm ) ;
    declareProperty( "zCenter", m_zCenter = -1000.0 * Gaudi::Units::mm ) ;
    declareProperty( "BeamDirection", m_zDir = 1 );
    declareProperty( "ThetaCenter", m_theta = 0.0 * Gaudi::Units::rad ) ;
    declareProperty( "PhiCenter", m_phi =   0.0 * Gaudi::Units::rad ) ;

    // Beam Parameters
    declareProperty( "Momentum", m_momentum = 450.0 * Gaudi::Units::GeV ) ;
    declareProperty( "xEmmitance", m_xEmm = 7.82e-9 * Gaudi::Units::m) ;
    declareProperty( "yEmmitance", m_yEmm = 7.82e-9 * Gaudi::Units::m) ;
    declareProperty( "xBeta", m_xBeta = 10.0 * Gaudi::Units::m ) ;
    declareProperty( "yBeta", m_yBeta = 10.0 * Gaudi::Units::m ) ;
    declareProperty( "xAlpha", m_xAlpha = 4.18e-7 ) ;
    declareProperty( "yAlpha", m_yAlpha = 1.18e-7 ) ;

    // Particles
    declareProperty("PdgCode",m_pdgCode = 2212 ) ; // Default P
}

//===========================================================================
// Destructor
//===========================================================================
BeamShape::~BeamShape() { }

//===========================================================================
// Initialize Particle Gun parameters
//===========================================================================
StatusCode BeamShape::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return sc ;

  // Get beam direction (force 1 or -1)
  m_zDir = (m_zDir<0?-1:1);
  
  // Get position spread
  m_xSigma = sqrt(m_xEmm*m_xBeta);
  m_ySigma = sqrt(m_yEmm*m_yBeta);

  // Get centroid momentums
  const double pt = m_momentum*sin(m_theta);
  m_pxCenter = m_zDir*pt*cos(m_phi);
  m_pyCenter = m_zDir*pt*sin(m_phi);

  // Get momentum spread
  m_pxSigma = sqrt(m_xEmm/m_xBeta);
  m_pySigma = sqrt(m_yEmm/m_yBeta);


  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;

  sc = m_flatGenerator.initialize( randSvc , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() )
    return Error( "Cannot initialize flat generator" ) ;

  sc = m_gaussGenerator.initialize( randSvc , Rndm::Gauss( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() )
    return Error( "Cannot initialize gaussian generator" ) ;

  // Get the mass of the particle to be generated
  LHCb::IParticlePropertySvc* ppSvc =
    svc< LHCb::IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;

  // setup particle information
  m_mass = ppSvc->find(LHCb::ParticleID( m_pdgCode ) )->mass();

  info() << "Particle type: " << ppSvc->find(LHCb::ParticleID( m_pdgCode ) )->particle()
         << endmsg ;

  // printout vertex information
  info() << "Beam Centroid at : ( "
         << "x = " << m_xCenter / Gaudi::Units::mm << " mm "
         << ", y = " << m_yCenter / Gaudi::Units::mm << " mm "
         << ", z = " << m_zCenter / Gaudi::Units::mm << " mm "
         << ")" << endmsg ;

  info() << "Momentum: ( "
         << "px = " << m_pxCenter / Gaudi::Units::GeV << " GeV "
         << ", py = " << m_pyCenter / Gaudi::Units::GeV << " GeV "
         << ", p = " << m_momentum / Gaudi::Units::GeV << " GeV "
         << ")" <<  endmsg;

  info() << "Direction: " << (m_zDir<0?"Beam 2":"Beam 1") << endmsg;

  release( ppSvc ) ;

  return sc ;
}

//===========================================================================
// Generate the particles
//===========================================================================
void BeamShape::generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                Gaudi::LorentzVector & origin , 
                                int & pdgId ) {
  double xn(0.),yn(0.),zn(0.) ;
  double x(0.),y(0.),z(0.) ;
  double px(0.), py(0.), pz(0.) ;

  // Get particle position in distance from center
  xn = m_gaussGenerator()*m_xSigma;
  yn = m_gaussGenerator()*m_ySigma;
  zn = 0.0; // If one ever would want to implement z-smearing.

  // In real lhcb coord sys
  x = xn + m_xCenter;
  y = yn + m_yCenter;
  z = zn + m_zCenter;

  // Get momenta
  double dx = -(m_xAlpha/m_xBeta)*xn + m_pxSigma*m_gaussGenerator();
  double dy = -(m_yAlpha/m_yBeta)*yn + m_pySigma*m_gaussGenerator();
  px = m_momentum*dx + m_pxCenter;
  py = m_momentum*dy + m_pyCenter;
  pz = m_zDir*sqrt(m_momentum*m_momentum - px*px - py*py);

  // Set up fourmomentum
  fourMomentum.SetPx( px );
  fourMomentum.SetPy( py );
  fourMomentum.SetPz( pz );
  fourMomentum.SetE( std::sqrt( m_mass * m_mass + fourMomentum.P2() ) );

  // Set up vertex
  origin.SetCoordinates( x , y , z , 0. ) ; // No timing

  // Pass the particle code
  pdgId = m_pdgCode ;
}
