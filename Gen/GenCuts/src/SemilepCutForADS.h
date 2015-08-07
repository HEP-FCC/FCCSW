#ifndef GENCUTS_SEMILEPCUTFORADS_H 
#define GENCUTS_SEMILEPCUTFORADS_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IGenCutTool.h"
#include "GaudiKernel/Transform4DTypes.h"

/** @class SemilepCutForADS SemilepCutForADS.h 
 *  
 *  Tool to keep Bu -> D0 mu nu events with Bu_M > a value
 *  and particles in LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Paolo Gandini
 *  @date   2011-02-18
 */

class SemilepCutForADS : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  SemilepCutForADS( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);
  
  virtual ~SemilepCutForADS( ); // Destructor

  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

 private:
  /** Study a particle a returns true when all stable daughters
   *  are in LHCb acceptance
   */
  bool passCuts( const HepMC::GenParticle * theSignal ) const ;  

  double m_chargedThetaMin ;
  double m_chargedThetaMax ;
  double m_neutralThetaMin ;
  double m_neutralThetaMax ;
  double m_massMIN         ;
};
#endif // GENCUTS_SEMILEPCUTFORADS_H
