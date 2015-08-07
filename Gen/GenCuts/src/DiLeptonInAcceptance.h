// $Id: DiLeptonInAcceptance.h,v 1.2 2009-08-10 13:15:56 tblake Exp $
#ifndef GENCUTS_DILEPTONINACCEPTANCE_H 
#define GENCUTS_DILEPTONINACCEPTANCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "MCInterfaces/IFullGenEventCutTool.h"


/** @class DiLeptonInAcceptance DiLeptonInAcceptance.h "DiLeptonInAcceptance.h"
 *  component/DiLeptonInAcceptance.h
 *  
 *  Cut on events with two leptons in LHCb acceptance + minimum pT.
 *  and mass of lepton pair in sepcified range
 *
 *  Examples:
 *
 *  Generate a pair of opposite sign muons 
 *  ( RequireOppositeSign = true; LeptonOneID = 13; LeptonOneID = 13 )
 *
 *  NB: this is the default
 *  
 *  Generate all possible same and opposite sign combinations 
 *  ( RequireOppositeSign = false; RequireSameSign = false; )
 *  
 *  Generate same sign combinations
 *  ( RequireOppositeSign = false; RequireSameSign = true; )
 *
 *  Generate e+mu- and e-mu+ combinations
 *  ( LeptonOneID = 13; LeptonTwoID = 11; )
 *  
 *  Generate e-mu- and e+mu+ combiantions
 *  ( LeptonOneID = 13; LeptonTwoID = 11; RequireOppositeSign = false; RequireSameSign = true; )
 * 
 * 
 *  Implementation of IFullGenEventCutTool.
 *
 *  @author T Blake
 *  @date   2009-08-10
 */

class DiLeptonInAcceptance : public GaudiTool , 
                           virtual public IFullGenEventCutTool {
 public:
  /// Standard constructor
  DiLeptonInAcceptance( const std::string& type, const std::string& name,
			const IInterface* parent ) ;
  
  virtual ~DiLeptonInAcceptance( ); ///< Destructor
  
  /** Apply cut on full event.
   *  Keep events with a pair of leptons in angular region around
   *  z axis (forward) and with a minimum pT.
   *  Implements IFullGenEventCutTool::studyFullEvent.
   */
  virtual bool studyFullEvent( LHCb::HepMCEvents * theEvents ,
                               LHCb::GenCollisions * theCollisions ) const ;

  
  
  
 private:
  
  /// Check if the lepton is in the detector acceptance, has minimum p and pT.
  bool isInAcceptance( const HepMC::GenParticle* , 
                       const double, const double,
                       const double, const double ) const;


  /// Check if the combination of two leptons is allowed
  bool isCombination( const HepMC::GenParticle*, 
                      const HepMC::GenParticle* ) const;
  
  
  /// Maximum value for theta angle of the first lepton (set by options)
  double m_thetaMaxLOne ;

  /// Minimum value for theta angle of the first lepton (set by options)
  double m_thetaMinLOne ;
  
  /// Maximum value for theta angle of the first lepton (set by options)
  double m_thetaMaxLTwo ;

  /// Minimum value for theta angle of the first lepton (set by options)
  double m_thetaMinLTwo ;
  
  /// Minimum pT of first lepton (set by options)
  double m_ptMinLOne ;

  /// Minimum p of first lepton (set by options)
  double m_pMinLOne;

  /// Minimum pT of second lepton (set by options)
  double m_ptMinLTwo ;

  /// Minimum p of second lepton (set by options)
  double m_pMinLTwo;

  /// Di Lepton mass range - Lower (set by options)
  double m_minMass; 

  /// Di Lepton mass range - Upper (set by options)
  double m_maxMass; 

  /// Allowed lepton - first lepton (set by options)
  int m_leptonOnePDG ; 

  /// Allowed lepton - second lepton (set by options)
  int m_leptonTwoPDG ;  

  /// Only allow decays with opposite sign leptons 
  bool m_oppSign; 
  
  /// Only allow decays with same sign leptons. Opposite sign leptons take precedence. 
  bool m_sameSign;
  
  /// Preselects the events applying a cut on the doca of the two tracks
  bool   m_PreselDoca;
  double m_docaCut;

  /// Preselect the events applying a cut on the product of the two transv. momenta
  bool m_PreselPtProd;
  double m_ptprodMinCut;
  double m_ptprodMaxCut;
  
};

#endif // GENCUTS_DILEPTONINACCEPTANCE_H
