// $Id: CollidingBeams.cpp,v 1.5 2007-02-08 17:46:06 gcorti Exp $
// Include files 

// local
#include "CollidingBeams.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

// From Kernel
#include "GaudiKernel/SystemOfUnits.h"

// From Event
#include "Event/BeamParameters.h"
#include "Event/BeamForInitialization.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CollidingBeams
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( CollidingBeams )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CollidingBeams::CollidingBeams( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IBeamTool >( this ) ;
    declareProperty( "BeamParameters" , 
                     m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
CollidingBeams::~CollidingBeams( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode CollidingBeams::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Initialize the number generator
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  
  sc = m_gaussianDist.initialize( randSvc , Rndm::Gauss( 0. , 1. ) )  ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize Gaussian random generator" , sc ) ;
  release( randSvc ) ;

  return sc ;
}

//=============================================================================
// Mean value of the beam momentum
//=============================================================================
void CollidingBeams::getMeanBeams( Gaudi::XYZVector & pBeam1 , 
                                   Gaudi::XYZVector & pBeam2 )
  const {
  // Retrieve beam parameters from the static class
  LHCb::BeamParameters * beam = 
    BeamForInitialization::getInitialBeamParameters() ;
  if ( 0 == beam ) 
    Exception( "No beam parameters in initialization" ) ;

  double p1x, p1y, p1z, p2x, p2y, p2z ;
  
  p1x = beam -> energy() * 
    sin( beam -> horizontalCrossingAngle() +
         beam -> horizontalBeamlineAngle() ) ;
  p1y = beam -> energy() * 
    sin( beam -> verticalCrossingAngle() + 
         beam -> verticalBeamlineAngle() ) ;
  p1z = beam -> energy() ;
  pBeam1.SetXYZ( p1x, p1y, p1z ) ;

  p2x = beam -> energy() * 
    sin( beam -> horizontalCrossingAngle() - 
         beam -> horizontalBeamlineAngle() ) ;
  p2y = beam -> energy() * 
    sin( beam -> verticalCrossingAngle() - 
         beam -> verticalBeamlineAngle() ) ;
  p2z = -beam -> energy() ;
  pBeam2.SetXYZ( p2x, p2y, p2z ) ;
}

//=============================================================================
// Current value of the smeared beams
//=============================================================================
void CollidingBeams::getBeams( Gaudi::XYZVector & pBeam1 , 
                               Gaudi::XYZVector & pBeam2 ) {
  // Retrieve beam parameters
  LHCb::BeamParameters * beam = get< LHCb::BeamParameters >( m_beamParameters ) ;
  if ( 0 == beam ) Exception( "No beam parameters in TES" ) ;

  double p1x, p1y, p1z, p2x, p2y, p2z ;
  p1x = beam -> energy() * 
    sin( beam -> horizontalCrossingAngle() + 
         beam -> horizontalBeamlineAngle() + 
         m_gaussianDist() * beam -> angleSmear() ) ;
  p1y = beam -> energy() * 
    sin( beam -> verticalCrossingAngle() + 
         beam -> verticalBeamlineAngle() +
         m_gaussianDist() * beam -> angleSmear() ) ;
  p1z = beam -> energy() ;
  pBeam1.SetXYZ( p1x, p1y, p1z ) ;

  p2x = beam -> energy() * 
    sin( beam -> horizontalCrossingAngle() - 
         beam -> horizontalBeamlineAngle() + 
         m_gaussianDist() * beam -> angleSmear() ) ;
  p2y = beam -> energy() * 
    sin( beam -> verticalCrossingAngle() - 
         beam -> verticalBeamlineAngle() +
         m_gaussianDist() * beam -> angleSmear() ) ;
  p2z = -beam -> energy() ;
  pBeam2.SetXYZ( p2x, p2y, p2z ) ;
}
