// $Id: BeamToolForPythia8.h,v 1.1.1.1 2012-11-26 17:02:19 amartens Exp $
#ifndef LBPYTHIA8_BEAMTOOLFORPYTHIA8_H 
#define LBPYTHIA8_BEAMTOOLFORPYTHIA8_H 1

// Include files
// from Pythia8
#include "Pythia8/BeamShape.h"
#include "Pythia8/Settings.h"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Vector3DTypes.h"

// from Generators
#include "Generators/IBeamTool.h"

/** @class BeamToolForPythia8 BeamToolForPythia8.h LbPythia8/BeamToolForPythia8.h
 *  Random generator class to interface Gaudi Random Generator to Pythia8
 *
 *  @author Aurelien Martens
 *  @date   2012-11-26
 */

class BeamToolForPythia8 : virtual public Pythia8::BeamShape {

public:
  // Initialize beam parameters.
  virtual void init(Pythia8::Settings& settings);

  /// Constructor
  BeamToolForPythia8( IBeamTool *i , Pythia8::Settings& settings, StatusCode &sc ) ;

  /// pick parameterisation for the beam spread and direction
  virtual void pick();

  /// Destructor
  virtual ~BeamToolForPythia8( ) ;
  
private: 
  IBeamTool* m_iBeamTool;
  Gaudi::XYZVector m_meanBeam1;
  Gaudi::XYZVector m_meanBeam2;

} ;
#endif // LBPYTHIA8_BEAMTOOLFORPYTHIA8_H
