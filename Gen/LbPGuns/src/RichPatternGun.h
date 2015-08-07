// $Id: RichPatternGun.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $

#ifndef PARTICLEGUNS_RICHPATTERNGUN_H
#define PARTICLEGUNS_RICHPATTERNGUN_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

namespace HepMC { class GenEvent ; }

/** @class RichPatternGun RichPatternGun.h 
 *  
 *  Algorithm to generate RICH optical photon gun
 *
 *  @author
 *  @date   2008-06-09
 */
class RichPatternGun : public GaudiAlgorithm {
public:
  /// Standard constructor
  RichPatternGun(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~RichPatternGun();

  /** Initialize method (implemented in ParticleGunBaseAlg)
   *  @return Status Code
   */
  virtual StatusCode initialize();

  /** Execute method (implemented in ParticleGunBaseAlg)
   *  @return Status Code
   */
  virtual StatusCode execute();
  
 protected:  
  
 private:

  /// generate one particle
  StatusCode callParticleGun( HepMC::GenEvent * evt ) ;
  
  int m_curx ;
  int m_cury ;

  int m_minx , m_miny , m_maxx , m_maxy ;
  
  /// Event Type
  int m_eventType ; 	

  double m_mean;
  double m_xsigma;
  double m_ysigma;
  double m_peak;
  
  std::string m_eventLoc;
  std::string m_headerLoc;
  std::string m_collLoc;
  
  unsigned int m_nParticles ;
  double m_xvtx , m_yvtx , m_zvtx ;
  
  Rndm::Numbers m_particleGenerator;
};   

#endif  // PARTICLEGUNS_RICHPATTERNGUN_H

