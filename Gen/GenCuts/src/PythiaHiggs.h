// $Id: PythiaHiggs.h,v 1.5 2009-10-05 06:18:58 ngueissa Exp $
#ifndef GENCUTS_PYTHIAHIGGS_H 
#define GENCUTS_PYTHIAHIGGS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "MCInterfaces/IGenCutTool.h"

/** @class PythiaHiggs PythiaHiggs.h component/PythiaHiggs.h
 *  
 *  Tool to select events with 2 b quarks from Higgs in acceptance
 *
 *  @author Patrick Robbe
 *  @date   2005-11-21
 */
class PythiaHiggs : public GaudiTool , virtual public IGenCutTool {
 public:
  /// Standard constructor
  PythiaHiggs( const std::string & type , const std::string & name ,
               const IInterface * parent ) ;
  
  virtual ~PythiaHiggs( ); ///< Destructor

  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent ,
                         const LHCb::GenCollision * theCollision ) const ;


 private:
  double m_thetaMax ;
  
};
#endif // GENCUTS_PYTHIAHIGGS_H
