// $Id: IParticleGunTool.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_IPARTICLEGUNTOOL_H 
#define PARTICLEGUNS_IPARTICLEGUNTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Vector4DTypes.h"


/** @class IParticleGunTool IParticleGunTool.h "ParticleGuns/IParticleGunTool.h"
 *  
 *  Abstract interface to particle gun tool. Generates a single particle.
 * 
 *  @author Patrick Robbe
 *  @date   2008-05-18
 */

static const InterfaceID IID_IParticleGunTool( "IParticleGunTool" , 1 , 0 ) ;

class IParticleGunTool : virtual public IAlgTool {
public:
  typedef std::vector< int > PIDs ;
  static const InterfaceID& interfaceID() { return IID_IParticleGunTool ; }

  /** Generates one particle.
   *  @param[out] fourMomentum  four-momentum of the generated particle gun
   *  @param[out] origin        four-momentum of the origin vertex of the particle gun 
   *  @param[out] pdgId         pdgId of the generated particle
   */
  virtual void generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId ) = 0 ;

  /// Print various counters at the end of the job
  virtual void printCounters( ) = 0 ;
};
#endif // PARTICLEGUNS_IPARTICLEGUNTOOL_H
