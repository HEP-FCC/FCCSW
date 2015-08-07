// $Id: BcNoCut.h,v 1.3 2008-09-03 09:04:49 gcorti Exp $
#ifndef GENERATORS_BCNOCUT_H 
#define GENERATORS_BCNOCUT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Transform4DTypes.h"

#include "MCInterfaces/IGenCutTool.h"

// Forward declaration
class IDecayTool ;

/** @class BcNoCut BcNoCut.h 
 *  
 *  Tool to generate Bc in full phase space
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Jibo He
 *  @author Patrick Robbe
 *  @date   2013-06-11
 */

class BcNoCut : public GaudiTool, virtual public IGenCutTool {
public:
  /// Standard constructor
  BcNoCut( const std::string& type, 
           const std::string& name,
           const IInterface* parent);
  
  virtual ~BcNoCut( ); ///< Destructor

  virtual StatusCode initialize( );   ///< Initialize method

  /** 
   *  Decay Bc
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

  virtual StatusCode finalize( );   ///< Finalize method
  
protected:
  
  
  
private:
  /// Decay tool
  IDecayTool*  m_decayTool;
  
  /// Name of the decay tool to use
  std::string m_decayToolName ;

  int  m_sigBcPID        ;  ///< PDG Id of the B_c 

};
#endif // GENERATORS_BCDAUGHTERSINLHCB_H
