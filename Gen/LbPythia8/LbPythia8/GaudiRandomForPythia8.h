// $Id: GaudiRandomForPythia8.h,v 1.1.1.1 2007-07-31 17:02:19 robbep Exp $
#ifndef LBPYTHIA8_GAUDIRANDOMFORPYTHIA8_H 
#define LBPYTHIA8_GAUDIRANDOMFORPYTHIA8_H 1

// Include files
#include "Pythia8/Basics.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class GaudiRandomForPythia8 GaudiRandomForPythia8.h LbPythia8/GaudiRandomForPythia8.h
 *  Random generator class to interface Gaudi Random Generator to Pythia8
 *
 *  @author Arthur de Gromard
 *  @date   2007-07-31
 */
class GaudiRandomForPythia8 : public Pythia8::RndmEngine {
public:
  /// Constructor
  GaudiRandomForPythia8( IRndmGenSvc *i , StatusCode &sc ) ;

  /// Throw random number
  virtual double flat() { return m_gaudiGenerator() ; } ;

  /// Destructor
  virtual ~GaudiRandomForPythia8( ) ;
  
private: 
  Rndm::Numbers m_gaudiGenerator ; ///< Gaudi Random Generator
} ;
#endif // LBPYTHIA8_GAUDIRANDOMFORPYTHIA8_H
