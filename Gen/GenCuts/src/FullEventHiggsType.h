#ifndef GENCUTS_FullEventHiggsType_H
#define GENCUTS_FullEventHiggsType_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCb
#include "Event/IParticlePropertySvc.h"

// from Generators
#include "MCInterfaces/IFullGenEventCutTool.h"

/** @class FullEventHiggsType FullEventHiggsType.h component/FullEventHiggsType.h
 *
 *  Tool to select events with, 1 or 2 b quarks in the acceptance coming 
 *  from a given mother, and/or 1 or 2 leptons in the acceptance with given 
 *  minimal pt and comming (or not) from
 *  a W or Z boson
 *
 *  @author Neal Gauvin
 *  @date   2009-10-12
 */
class FullEventHiggsType : public GaudiTool , virtual public IFullGenEventCutTool {
public:
  /// Standard constructor
  FullEventHiggsType( const std::string & type , const std::string & name ,
                   const IInterface * parent ) ;
  
  virtual ~FullEventHiggsType( ); ///< Destructor  

  /// initialize function
  virtual StatusCode initialize( );

  virtual bool studyFullEvent( LHCb::HepMCEvents * , 
                               LHCb::GenCollisions * ) const;

private:        
  /// Return true if the particle is a b quark
  bool Isb( const HepMC::GenParticle * p ) const ;

  /// Return true if the particle is a lepton
  bool IsLepton( const HepMC::GenParticle * p ) const ;

  /// Max theta angle to define the LHCb acceptance
  double m_thetaMax ;

  /// Number of leptons required in the acceptance
  int m_nbLepton;

  /// Type of lepton requiered in the acceptance
  std::vector< std::string > m_TypeLepton;

  /// Minimum pT of the lepton
  double m_ptMin ;

  /// If true, the lepton is required to come from a given mother
  bool m_leptonFromMother;

  /// List of mother of the lepton
  std::vector< std::string > m_motheroflepton;

  /// Number of quarks required to be in the acceptance
  int m_nbbquarks;

  /// PDG id of the mother of the b quarks
  std::string m_motherofb_id;

  /// Substitute of the b quark, if any reqested
  std::string m_subb;

  int m_motherofb_pid;
  int m_b_pid;

  const LHCb::IParticlePropertySvc* m_ppSvc;
  
};
#endif // GENCUTS_FullEventHiggsType_H
