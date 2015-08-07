// $Id: UniformSmearVertex.cpp,v 1.5 2008-07-24 22:05:38 robbep Exp $
// Include files 

// local
#include "UniformSmearVertex.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector4DTypes.h"

// from Event
#include "Event/HepMCEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UniformSmearVertex
//
// 2007-09-07 : M.Ferro-Luzzi
// 2007-09-27 : G.Corti, use twopi from PhysicalConstants.h instead of 
//              hardcoded number
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( UniformSmearVertex )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UniformSmearVertex::UniformSmearVertex( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ) , 
    m_rmaxsq( 0. ) , 
    m_deltaz( 0. ) {
    declareInterface< IVertexSmearingTool >( this ) ;
    declareProperty( "RMax"   , m_rmax   =     1. * Gaudi::Units::mm ) ;
    declareProperty( "ZMin"   , m_zmin   = -1500. * Gaudi::Units::mm ) ;
    declareProperty( "ZMax"   , m_zmax   =  1500. * Gaudi::Units::mm ) ;
    declareProperty( "BeamDirection", m_zDir = 1 );

}

//=============================================================================
// Destructor 
//=============================================================================
UniformSmearVertex::~UniformSmearVertex( ) { ; }

//=============================================================================
// Initialize 
//=============================================================================
StatusCode UniformSmearVertex::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  if ( !(m_zmin < m_zmax) ) return Error( "zMin >= zMax !" ) ;
  if ( !(m_rmax > 0.    ) ) return Error( "rMax <=  0  !" ) ;
  m_deltaz =  m_zmax - m_zmin       ;
  m_rmaxsq = m_rmax*m_rmax          ;

  IRndmGenSvc* randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_flatDist.initialize( randSvc , Rndm::Flat( 0.0 , 1.0 ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize flat random number generator" ) ;

  std::string infoMsg = " applying TOF of interaction with ";
  if ( m_zDir == -1 ) {
    infoMsg = infoMsg + "negative beam direction";
  } else if ( m_zDir == 1 ) {
    infoMsg = infoMsg + "positive beam direction";
  } else if ( m_zDir == 0 ) {
    infoMsg = " with TOF of interaction equal to zero ";
  } else {
    return Error("BeamDirection can only be set to -1 or 1, or 0 to switch off TOF");
  }

  info() << "Smearing of interaction point with flat distribution "
         << " in x, y and z " << endmsg;
  info() << infoMsg << endmsg;
  if( msgLevel(MSG::DEBUG) ) {
    debug() << " with r less than " << m_rmax / Gaudi::Units::mm 
            << " mm." << endmsg ;
    debug() << " with z between " << m_zmin / Gaudi::Units::mm 
            << " mm and " << m_zmax / Gaudi::Units::mm << " mm." << endmsg ;
  } else {
    info() << " with r <= " << m_rmax / Gaudi::Units::mm << " mm, "
           << m_zmin / Gaudi::Units::mm << " mm <= z <= " 
           << m_zmax / Gaudi::Units::mm << " mm." << endmsg;
  }

  release( randSvc ) ;
 
  return sc ;
}

//=============================================================================
// Smearing function
//=============================================================================
StatusCode UniformSmearVertex::smearVertex( LHCb::HepMCEvent * theEvent ) {
  double dx , dy , dz, dt, rsq, r, th ;
  
  // generate flat in z, r^2 and theta:
  dz  = m_deltaz   * m_flatDist( ) + m_zmin ;
  rsq = m_rmaxsq   * m_flatDist( )          ;
  th  = Gaudi::Units::twopi * m_flatDist( ) ;
  r   = sqrt(rsq) ;
  dx  = r*cos(th) ;  
  dy  = r*sin(th) ;
  dt  = m_zDir * dz/Gaudi::Units::c_light ;
  Gaudi::LorentzVector dpos( dx , dy , dz , dt ) ;
  
  HepMC::GenEvent::vertex_iterator vit ;
  HepMC::GenEvent * pEvt = theEvent -> pGenEvt() ;
  for ( vit = pEvt -> vertices_begin() ; vit != pEvt -> vertices_end() ; 
        ++vit ) {
    Gaudi::LorentzVector pos ( (*vit) -> position() ) ;
    pos += dpos ;
    (*vit) -> set_position( HepMC::FourVector( pos.x() , pos.y() , pos.z() ,
                                               pos.t() ) ) ;
  }

  return StatusCode::SUCCESS ;      
}

