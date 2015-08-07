// $Id: BeamToolForPythia8.cpp,v 1.1.1.1 2012-11-26 17:02:19 amartens Exp $
// Include files

// local
#include "LbPythia8/BeamToolForPythia8.h"

// from GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BeamToolForPythia8
//
// 2012-11-26 : Aurelien Martens
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamToolForPythia8::BeamToolForPythia8( IBeamTool * i , Pythia8::Settings& settings, StatusCode &sc ) {
  m_iBeamTool = i;
  sc = StatusCode::SUCCESS;
  init(settings);
}
//=============================================================================
// initialize the tool so that it knows about the beam mean params
//=============================================================================
void BeamToolForPythia8::init(Pythia8::Settings& settings) {
  m_iBeamTool->getMeanBeams(  m_meanBeam1,  m_meanBeam2 );
  allowMomentumSpread = settings.flag("Beams:allowMomentumSpread");
  allowVertexSpread   = settings.flag("Beams:allowVertexSpread");

  deltaPxA = deltaPyA = deltaPzA = deltaPxB = deltaPyB = deltaPzB
    = vertexX = vertexY = vertexZ = vertexT = 0.;
  
}
//=============================================================================
// pick decoded values from IBeamTool
//=============================================================================
void BeamToolForPythia8::pick() {
  Gaudi::XYZVector beam1;
  Gaudi::XYZVector beam2;

  m_iBeamTool->getBeams(  beam1,  beam2 );
  
  beam1 -= m_meanBeam1;
  beam2 -= m_meanBeam2;

  beam1 /= Gaudi::Units::GeV;
  beam2 /= Gaudi::Units::GeV;

  deltaPxA = beam1.X();
  deltaPyA = beam1.Y();
  deltaPzA = beam1.Z();
  
  deltaPxB = beam2.X();
  deltaPyB = beam2.Y();
  deltaPzB = beam2.Z();
}
//=============================================================================
// Destructor
//=============================================================================
BeamToolForPythia8::~BeamToolForPythia8() {
} 

//=============================================================================
