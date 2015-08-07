// $Id: LeptonInAcceptance.h,v 1.3 2005-12-31 17:33:12 robbep Exp $
#ifndef GENCUTS_LEPTONINACCEPTANCE_H 
#define GENCUTS_LEPTONINACCEPTANCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "MCInterfaces/IFullGenEventCutTool.h"


/** @class LeptonInAcceptance LeptonInAcceptance.h "LeptonInAcceptance.h"
 *  component/LeptonInAcceptance.h
 *  
 *  Cut on events with one lepton in LHCb acceptance + minimum pT.
 *  Concrete implementation of IFullGenEventCutTool.
 *
 *  @author Patrick Robbe
 *  @date   2005-11-21
 */
class LeptonInAcceptance : public GaudiTool , 
                           virtual public IFullGenEventCutTool {
 public:
  /// Standard constructor
  LeptonInAcceptance( const std::string& type, const std::string& name,
                      const IInterface* parent ) ;
  
  virtual ~LeptonInAcceptance( ); ///< Destructor
  
  /** Apply cut on full event.
   *  Keep events with at least one lepton in angular region around
   *  z axis (forward) and with a minimum pT.
   *  Implements IFullGenEventCutTool::studyFullEvent.
   */
  virtual bool studyFullEvent( LHCb::HepMCEvents * theEvents ,
                               LHCb::GenCollisions * theCollisions ) const ;
  
 private:
  ///< Maximum value for theta angle of lepton (set by options)
  double m_thetaMax ;

  double m_ptMin ; ///< Minimum pT of lepton (set by options)
};
#endif // GENCUTS_LEPTONINACCEPTANCE_H
