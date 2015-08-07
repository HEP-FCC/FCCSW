#ifndef GENCUTS_DMUCASCADEINACC_H 
#define GENCUTS_DMUCASCADEINACC_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IFullGenEventCutTool.h"
#include "MCInterfaces/IGenCutTool.h"
#include "GaudiKernel/Transform4DTypes.h"

/** @class DMuCascadeInAcc DMuCascadeInAcc.h 
 *  
 *  Tool to filter generic Xb->Xc(->KKPi)Xc(->mu X) events and keep the particles in LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Adam Webber
 *  @date   2011-07-26
 */

class DMuCascadeInAcc: public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  DMuCascadeInAcc( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);
  
  virtual ~DMuCascadeInAcc( ); // Destructor

  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

 private:
  /** Check the correct particles are in LHCb acceptance. 
   */
  bool passCuts( const HepMC::GenParticle * theSignal
              , bool &hasMuon, bool &hasHadrons
              , bool &hasMuonAndHadrons  ) const ;  

  double m_chargedThetaMin ;
  double m_chargedThetaMax ;
  double m_muonptmin; 
  double m_muonpmin; 
  double m_hadronptmin;
  double m_hadronpmin; 
};
#endif // GENCUTS_DMUCASCADEINACC_H
