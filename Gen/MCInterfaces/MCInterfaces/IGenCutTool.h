// $Id: $
#ifndef MCINTERFACES_IGENCUTTOOL_H 
#define MCINTERFACES_IGENCUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/GenCollision.h"

// Forward declarations
namespace HepMC {
  class GenParticle ; 
  class GenEvent ;
}  

namespace LHCb {
  class GenCollision ;
}

class IDecayTool ;

/** @class IGenCutTool IGenCutTool.h "MCInterfaces/IGenCutTool.h"
 *  
 *  Abstract interface to generator level cut. This type of cut is applied
 *  to the interaction containing the signal particle. The interaction, at
 *  this point contains undecayed particles (except excited heavy particles).
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IGenCutTool( "IGenCutTool" , 5 , 0 ) ;

class IGenCutTool : virtual public IAlgTool {
public:
  /// Vector of particles
  typedef std::vector< HepMC::GenParticle * > ParticleVector ;

  static const InterfaceID& interfaceID() { return IID_IGenCutTool ; }

  /** Applies the cut on the signal interaction.
   *  @param[in,out] theParticleVector  List of signal particles. The 
   *                                    generator level cut is applied to
   *                                    all these particles and particles
   *                                    which do not pass the cut are removed
   *                                    from theParticleVector.
   *  @param[in]     theGenEvent        Generated interaction. The generator
   *                                    level cut can use the particles in 
   *                                    this event to take the decision.
   *  @param[in]     theCollision       Hard process information of the
   *                                    interaction which can be used by
   *                                    the cut to take the decision.
   *  @return        true  if the event passes the generator level cut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theGenEvent ,
                         const LHCb::GenCollision * theCollision ) 
    const = 0 ;
};
#endif // MCINTERFACES_ICUTTOOL_H
