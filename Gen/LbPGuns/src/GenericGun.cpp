// $Id: GenericGun.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
// Include files

// This class
#include "GenericGun.h"

// From STL
#include <cmath>

// From Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/IParticlePropertySvc.h"
#include "Event/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IRndmGenSvc.h"

//==========================================================================
// Implementation file for class: GenericGun
//
// 2008-05-18: Patrick Robbe, rewrite in tool format particle gun algorithm
//==========================================================================

DECLARE_TOOL_FACTORY( GenericGun )

//==========================================================================
// Constructor
//==========================================================================
GenericGun::GenericGun(const std::string & type,
                       const std::string & name ,
                       const IInterface * parent ) :
  GaudiTool(type , name, parent ) , m_mass( 0. ) {
  declareInterface< IParticleGunTool >( this ) ;

  declareProperty( "Pt"  , m_requestedPt  = 5.0 * Gaudi::Units::GeV ) ;
  declareProperty( "Eta" , m_requestedEta = 0.0 ) ;
  declareProperty( "Phi" , m_requestedPhi = 0.0 * Gaudi::Units::rad ) ;

  declareProperty( "MinPt"  , m_minPt = 1. * Gaudi::Units::GeV ) ;
  declareProperty( "MinEta" , m_minEta = -4.0 ) ;
  declareProperty( "MinPhi" , m_minPhi = 0. * Gaudi::Units::rad ) ;

  declareProperty( "MaxPt"  , m_maxPt = 100. * Gaudi::Units::GeV ) ;
  declareProperty( "MaxEta" , m_maxEta= 4.0 ) ;
  declareProperty( "MaxPhi" , m_maxPhi = Gaudi::Units::twopi * Gaudi::Units::rad ) ;

  declareProperty( "SigmaPt"  , m_sigmaPt = 0.1 * Gaudi::Units::GeV ) ;
  declareProperty( "SigmaEta" , m_sigmaEta= 0.1 ) ;
  declareProperty( "SigmaPhi" , m_sigmaPhi = 0.1 * Gaudi::Units::rad ) ;

  declareProperty( "ModePt"  , m_PtGenMode  = SPGGenMode::FixedMode ) ;
  declareProperty( "ModeEta" , m_EtaGenMode = SPGGenMode::GaussMode ) ;
  declareProperty( "ModePhi" , m_PhiGenMode = SPGGenMode::FlatMode  ) ;

  declareProperty( "PdgCode" , m_pdgCode = 211 ) ;
}

//==========================================================================
// Destructor
//==========================================================================
GenericGun::~GenericGun() { }

//==========================================================================
// Initialize Generator
//==========================================================================
StatusCode GenericGun::initialize( ) {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return sc;

  // Create the flat and gaussian generators
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_flatGenerator.initialize( randSvc , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() )
    return Error( "Cannot initialize flat generator" ) ;

  sc = m_gaussGenerator.initialize( randSvc , Rndm::Gauss( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() )
    return Error( "Cannot initialize Gaussian generator" ) ;

  // Get the mass of the particle to be generated
  LHCb::IParticlePropertySvc * ppSvc =
    svc< LHCb::IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;

  const LHCb::ParticleProperty* particle = 
    ppSvc->find( LHCb::ParticleID( m_pdgCode ) ) ;
  m_mass = particle->mass();

  //
  // Make sure the parameters are in a sensible range...
  //
  if ( SPGGenMode::FixedMode != m_PtGenMode ) {
    if ( ( m_minPt > m_requestedPt ) || ( m_maxPt < m_requestedPt ) ||
         ( m_maxPt < m_minPt ) ) {
      error() << " Pt min and max out of range." << endmsg
              << " Will set Pt mode to Fixed!!!" << endmsg ;
      m_PtGenMode = SPGGenMode::FixedMode ;
    }
  }

  if( SPGGenMode::FixedMode != m_EtaGenMode ) {
    if ( ( m_minEta > m_requestedEta ) || ( m_maxEta < m_requestedEta ) ||
         ( m_maxEta < m_minEta ) ) {
      error() << " Eta min and max out of range." << endmsg
              << " Will set Eta mode to Fixed!!!" << endmsg ;
      m_EtaGenMode = SPGGenMode::FixedMode;
    }
  }

  if( SPGGenMode::FixedMode != m_PhiGenMode ) {
    if ( ( m_minPhi > m_requestedPhi ) || ( m_maxPhi < m_requestedPhi ) ||
         ( m_maxPhi < m_minPhi ) ) {
      error() << " Phi min and max out of range.  \n"
              << " Will set Phi mode to Fixed!!!" << endmsg ;
      m_PhiGenMode = SPGGenMode::FixedMode;
    }
  }

  release( ppSvc ) ;

  return sc ;
}

//===========================================================================
// Generate Particle
//===========================================================================
void GenericGun::generateParticle( Gaudi::LorentzVector & fourMomentum ,
                                   Gaudi::LorentzVector & origin ,
                                   int & pdgId ) {
  // Generate values for pt, eta and phi
  //
  double pt  = generateValue( m_PtGenMode,m_requestedPt, m_sigmaPt,
                              m_minPt, m_maxPt);
  double eta = generateValue(m_EtaGenMode,m_requestedEta, m_sigmaEta,
                             m_minEta, m_maxEta);
  double phi = generateValue(m_PhiGenMode,m_requestedPhi, m_sigmaPhi,
                             m_minPhi, m_maxPhi);

  // Transform to x,y,z coordinates
  //
  double theta = 2.*atan(exp(-eta));
  double px    = pt*cos(phi);
  double py    = pt*sin(phi);
  double pz    = pt/tan(theta);

  origin.SetCoordinates( 0. , 0. , 0. , 0. ) ;
  fourMomentum.SetPx( px ) ;
  fourMomentum.SetPy( py ) ;
  fourMomentum.SetPz( pz ) ;
  fourMomentum.SetE( std::sqrt( m_mass * m_mass + fourMomentum.P2() ) ) ;
  pdgId = m_pdgCode ;
}

//============================================================================
// Generate value
//============================================================================
double GenericGun::generateValue( const int mode, const double val,
                                  const double sigma, const double min,
                                  const double max) {
  double tmp ;
  int i = 0 ;
  const int maxtries = 100 ;

  switch (mode) {
  case SPGGenMode::FixedMode :
    return val ;
  case SPGGenMode::GaussMode :
    tmp = max + 1.0 ;
    i = 0 ;
    do {
      tmp = m_gaussGenerator() * sigma + val ;
      i++;
    } while ( ( (tmp<min) || (tmp > max) ) && (i < maxtries));
    if(i>maxtries) {
      error() << "Cant generate value in range (min, max) "
              << val << "\t" << min << "\t" << max << endreq;
    }
    return tmp;
  case SPGGenMode::FlatMode :
    tmp = m_flatGenerator() * ( max - min ) + min ;
    return tmp;
  default:
    error() << "Unknown Generation Mode" << endreq;
    return 0.;
  }
}
