#ifndef GENCUTS_PARTICLESINACCEPTANCE_H 
#define GENCUTS_PARTICLESINACCEPTANCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "MCInterfaces/IFullGenEventCutTool.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"

/** @class ParticlesInAcceptance ParticlesInAcceptance.h component/ParticlesInAcceptance.h
 *  
 * FullGenEventCutTool to select events with particles satisfying certain cuts.
 * Usage :
 * Generation.FullGenEventCutTool="ParticlesInAcceptance";
 * Condition to apply :
 * LSPCond = 1 : LSP in acceptance, 
 *         = 2 : all daughters in DgtsInAcc in acceptance
 *         = 3 : all daughters in acceptance
 * Daughters required to be in acceptance :
 * DgtsInAcc = {3,4,5,23,24}, D = {}
 * Generation.ParticlesInAcceptance.PartCond = 1 ; // =d
 * Number of desired Part :
 * Generation.ParticlesInAcceptance.NbPart = 1 ; //=d
 * Set if at least NbPart should respect conditions or just NbPart
 * Generation.ParticlesInAcceptance.PartID = { 23, 24, 3, 4, 5 };
 * Availabe cuts :
 * Generation.ParticlesInAcceptance.DistToPVMin (Max) = x*mm ; // d=0,infty
 * Generation.ParticlesInAcceptance.ZPosMin (Max) = y*mm ;// d=-infty,infty
 * Generation.ParticlesInAcceptance.EtaMax = 1.8 ; // =d
 * Generation.ParticlesInAcceptance.EtaMin = 4.9 ; // =d
 *
 *  @author Neal Gauvin (Gueissaz)
 *  @date   2009-August-3
 **/
class ParticlesInAcceptance : public GaudiTool , 
	virtual public IFullGenEventCutTool {
public: 
  /// Standard constructor
  ParticlesInAcceptance( const std::string & type , const std::string & name ,
               const IInterface * parent ); 

  virtual ~ParticlesInAcceptance( ); ///< Destructor

  virtual bool studyFullEvent( LHCb::HepMCEvents * theEvents ,
                               LHCb::GenCollisions * theCollisions ) const ;

protected:

private:

  //void ResetParticles( HepMC::GenParticle *,CLHEP::HepLorentzVector &) const;
  bool IsQuark( HepMC::GenParticle * ) const;
  bool IsLepton( HepMC::GenParticle * ) const;
  bool IsPart( HepMC::GenParticle * ) const;
  bool IsDgts( HepMC::GenParticle * ) const;
  double Dist( HepMC::GenVertex *, HepMC::GenVertex *) const;
  std::string Print( HepMC::ThreeVector ) const;
  std::string Print( HepMC::FourVector ) const;

  std::vector<int> m_PartID;    
  std::vector<int> m_Dgts;
  int m_NbPart;
  int m_PartCond;
  bool m_AtLeast;
  double m_EtaMin, m_EtaMax;
  double m_DistToPVMin, m_DistToPVMax, m_ZPosMin, m_ZPosMax;
};
#endif // GENCUTS_PARTICLESINACCEPTANCE_H
