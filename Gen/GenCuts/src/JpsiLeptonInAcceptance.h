// $Id: JpsiLeptonInAcceptance.h,v 0.1 2013-10-23 18:24:00 jwimberl Exp $
#ifndef GENCUTS_JPSILEPTONINACCEPTANCE_H 
#define GENCUTS_JPSILEPTONINACCEPTANCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "MCInterfaces/IFullGenEventCutTool.h"


/** @class JpsiLeptonInAcceptance JpsiLeptonInAcceptance.h "JpsiLeptonInAcceptance.h"
 *  component/JpsiLeptonInAcceptance.h
 *  
 *  Cut on events with jpsi + mu in LHCb acceptance + minimum pT.
 *  and mass in the Bc mass range M(Jpsi) + M(mu) -- M(Bc)
 *  and a liberal DOCA cut, calculated via CTAU
 * 
 *  Implementation of IFullGenEventCutTool.
 *
 *  @author Jack Wimberley
 *  @date   2013-10-23
 */

class JpsiLeptonInAcceptance : public GaudiTool , 
                           virtual public IFullGenEventCutTool {
 public:
  /// Standard constructor
  JpsiLeptonInAcceptance( const std::string& type, const std::string& name,
			const IInterface* parent ) ;
  
  virtual ~JpsiLeptonInAcceptance( ); ///< Destructor
  
  /** Apply cut on full event.
   *  Keep events with a jpsi + mu in angular region around
   *  z axis (forward) and with a minimum pT.
   *  Implements IFullGenEventCutTool::studyFullEvent.
   */
  virtual bool studyFullEvent( LHCb::HepMCEvents * theEvents ,
                               LHCb::GenCollisions * theCollisions ) const ;

  
  
  
 private:
  
  /// Check if the particle lepton is in the detector acceptance, has minimum p and pT.
  bool isInAcceptance( const HepMC::GenParticle*, double pmin, double ptmin, bool angular) const;


  // Particle types of J/psi (or psi(2S), etc.), dilepton, and lepton
  int m_JpsiPDG;
  int m_JpsiLepPDG ; 
  int m_BachLepPDG ;  

  // PT cuts
  double m_JpsiPtMin;
  double m_JpsiLepPtMin;
  double m_BachLepPtMin;
  
  // P cuts
  double m_JpsiPMin;
  double m_JpsiLepPMin;
  double m_BachLepPMin;
  
  // Angular acceptance cuts on leptons
  double m_thetaMin;
  double m_thetaMax;
  double m_thetaXZMax;
  double m_thetaYZMax;
  
  // Preselects the events, applying a cut on the trilepton invariant mass
  bool m_PreselMass;
  double m_minMass; 
  double m_maxMass; 

  //  Preselects the events applying a cut on the doca of the two tracks
  bool m_PreselDoca;
  double m_docaCut;

};

#endif // GENCUTS_JPSILEPTONINACCEPTANCE_H
