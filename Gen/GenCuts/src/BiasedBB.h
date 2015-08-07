// $Id: BiasedBB.h,v 1.3 2007-02-22 13:30:24 robbep Exp $
#ifndef GENCUTS_BIASEDBB_H 
#define GENCUTS_BIASEDBB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "MCInterfaces/IGenCutTool.h"

/** @class BiasedBB BiasedBB.h 
 *  
 *  Tool to keep events passing cuts defining the maximum biased BB sample
 *  for DC06. 
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class BiasedBB : public GaudiTool, virtual public IGenCutTool {
 public:
  
  typedef std::set< int > PIDs ;

  /// Initialization
  virtual StatusCode initialize() ;

  /// Standard constructor
  BiasedBB( const std::string& type, const std::string& name,
            const IInterface* parent);
  
  virtual ~BiasedBB( ); ///< Destructor

  /** Accept events with a cut on eta, pT, v=pT+5.360v and ctau
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theGenEvent , 
                         const LHCb::GenCollision * theCollision ) const ;
  
 private:
  /// Maximum value of eta (set by options) 
  double m_etaMax ;
  /// Minimum value of eta (set by options) 
  double m_etaMin ;
  /// Minimum value of pT (set by options) 
  double m_ptMin ;
  /// Minimum value of v=pT+5.360eta (set by options) 
  double m_vMin ;
  /// Minimum value of ctau
  double m_ctauMin ;
  /// Minimum value of radius from 0,0 (set by options) 
  double m_radiusMin ;
  /// Minimum number of B's (set by options) 
  double m_nBMin;
  /// Minimum deltaPhi between B's (set by options) 
  double m_deltaPhiMin;
  /// Minimum deltaPhi between B's
  bool m_deltaPhiCut;
  /// PDG Id of charged particle to select  
  PIDs m_chargedPIDList ;
  /// Minimum number of B stable charged daugthers
  int m_nStableChargedDaugthers ;
  

};
#endif // GENCUTS_BIASEDBB_H
