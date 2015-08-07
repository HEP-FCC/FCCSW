// $Id: IDecayTool.h,v 1.2 2005-12-11 23:21:47 robbep Exp $
#ifndef MCINTERFACES_IDECAYTOOL_H 
#define MCINTERFACES_IDECAYTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace HepMC {
  class GenEvent ;
  class GenParticle ;
}

/** @class IDecayTool IDecayTool.h "Algorithms/IDecayTool.h"
 *  
 *  Abstract interface to decay engines.
 *
 *  @author Patrick Robbe
 *  @date   2003-10-15
 */

static const InterfaceID IID_IDecayTool("IDecayTool",2,0) ;


class IDecayTool : virtual public IAlgTool {
public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IDecayTool ; }
  
  /** Generate the decay of a particle.
   *  @param[in,out] theMother  Particle to decay with the decay engine. The 
   *                            particle is updated with the generated decay 
   *                            tree.
   */
  virtual StatusCode generateDecay( HepMC::GenParticle * theMother ) const 
    = 0 ;

  /** Generates the forced decay of a signal particle.
   *  @param[in,out] theMother  Particle to decay according to the forced
   *                            signal decay mode.
   *  @param[out]    flip       true if the decay engine has modify the initial
   *                            flavour of theMother (to generate CP 
   *                            violation)
   */
  virtual StatusCode generateSignalDecay( HepMC::GenParticle * theMother ,
                                          bool & flip ) const = 0 ;

  /** Generates the decay of a particle, stopping at a given PDG Id.
   *  This allows to generate decay of excited heavy particles, keeping
   *  undecayed the signal particle.
   *  @param[in,out] theMother  Heavy excited particle to decay with the 
   *                            decay engine.
   *  @param[in]     targetId   If a particle with PDG Id equal to targetId
   *                            is found in the decay tree of theMother, the 
   *                            decay tree is stopped here and the particle
   *                            with this PDG Id is left undecayed (it will
   *                            be decayed later according to the forced
   *                            decay channle because in general, it is the
   *                            signal particle).
   */
  virtual StatusCode generateDecayWithLimit( HepMC::GenParticle * theMother ,
                                             const int targetId ) const = 0 ;

  /** Enable the possibility to flip the flavour of the particle in the decay
   *  engine, to generate CP violation.
   */
  virtual void enableFlip() const = 0 ;

  /** Disalbe the possibility to flip the flavour of the particle in the decay
   *  engine, to generate CP violation.
   */
  virtual void disableFlip() const = 0 ;

  /** Check if particle is known to decay engine.
   *  @param[in] pdgId  PDG Id of the particle to check.
   *  @return true if the particle is known to the decay engine (for EvtGen,
   *  it means that it has an entry in the main decay file DECAY.DEC).
   */
  virtual bool isKnownToDecayTool( const int pdgId ) const = 0 ;

  /** Compute the branching fraction of the signal decay mode in the main
   *   inclusive decay table.
   */
  virtual double getSignalBr( ) const = 0 ;
  
  /** Check that the forced signal decay mode is included in the main 
   *  inclusive decay table. Otherwise, it should be added.
   */
  virtual bool checkSignalPresence( ) const = 0 ;

  /** Inform to the decay tool the type of signal particle.
   *  @param[in] pdgId  PDG Id of the signal particle. 
   */  
  virtual void setSignal( const int pdgId ) = 0 ;
};
#endif // MCINTERFACES_IDECAYTOOL_H
