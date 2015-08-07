// $Id: GaussianTheta.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
// Include files

// This class
#include "GaussianTheta.h"

// From STL
#include <cmath>

// From Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/IParticlePropertySvc.h"
#include "Event/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

//==========================================================================
// Implementation file for class: GaussianTheta
//
// 2008-05-18: Patrick Robbe, rewrite in tool format particle gun algorithm
//==========================================================================

DECLARE_TOOL_FACTORY( GaussianTheta )

//==========================================================================
// Constructor
//==========================================================================
GaussianTheta::GaussianTheta(const std::string & type, 
                             const std::string & name , 
                             const IInterface * parent ) : 
  GaudiTool(type , name, parent ) , m_mass( 0. ) {
    declareInterface< IParticleGunTool >( this ) ;
    declareProperty( "MomentumMin" , m_minMom = 100.0 * Gaudi::Units::GeV ) ;
    declareProperty( "MomentumMax" , m_maxMom = 100.0 * Gaudi::Units::GeV ) ;
    
    declareProperty( "SigmaTheta"  , m_sigmaTheta = 1. * Gaudi::Units::rad ) ;
    declareProperty( "MeanTheta"   , m_meanTheta  = 0. * Gaudi::Units::rad ) ; 

    declareProperty( "XAxis" , m_x_axis = 0. ) ;
    declareProperty( "YAxis" , m_y_axis = 0. ) ;
        
    declareProperty( "PdgCode" , m_pdgCode = 211 ) ;
  }

//==========================================================================
// Destructor
//==========================================================================
GaussianTheta::~GaussianTheta() { }

//==========================================================================
// Initialize Generator
//==========================================================================
StatusCode GaussianTheta::initialize( ) {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return sc;
  
  // Create the flat and gaussian generators
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_flatGenerator.initialize( randSvc , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Cannot initialize flat generator" ) ;
  
  sc = m_gaussGenerator.initialize( randSvc , Rndm::Gauss( m_meanTheta ,
       m_sigmaTheta ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Cannot initialize Gaussian generator" ) ;
  
  // Get the mass of the particle to be generated
  LHCb::IParticlePropertySvc * ppSvc = 
    svc< LHCb::IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;
 
  const LHCb::ParticleProperty* particle = 
    ppSvc->find( LHCb::ParticleID( m_pdgCode ) ) ; 
  m_mass = particle->mass();
      
  release( ppSvc ) ;
     
  if ( m_maxMom < m_minMom ) 
    return Error( "Invalid options for momentum range" ) ;
  
  return sc ;
}

//===========================================================================
// Generate Particle
//===========================================================================
void GaussianTheta::generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                      Gaudi::LorentzVector & origin , 
                                      int & pdgId ) {
  
  const double theta = m_gaussGenerator();
  const double phi = m_flatGenerator() * Gaudi::Units::twopi ;

  double px , py , pz ;
  
  const double momentum = m_minMom   + m_flatGenerator() * (m_maxMom-m_minMom);

  ///       Transform to x,y,z coordinates
  const double pt = momentum*sin(theta);
  px              = pt*cos(phi);
  py              = pt*sin(phi);
  pz              = momentum*cos(theta);

  //defining x & y rotation matrices
  Gaudi::XYZVector momVect( px , py , pz ) ;  
  Gaudi::RotationX rotationx( m_x_axis ) ;
  
  momVect = rotationx * momVect;

  Gaudi::RotationY rotationy( m_y_axis ) ;

  momVect = rotationy * momVect;
  
  origin.SetCoordinates( 0. , 0. , 0. , 0. ) ;
  fourMomentum.SetPx( momVect.X() ) ;
  fourMomentum.SetPy( momVect.Y() ) ;
  fourMomentum.SetPz( momVect.Z() ) ;
  fourMomentum.SetE( std::sqrt( m_mass * m_mass + fourMomentum.P2() ) ) ;
  pdgId = m_pdgCode ;
}
