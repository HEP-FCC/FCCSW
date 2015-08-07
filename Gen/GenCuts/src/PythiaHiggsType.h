#ifndef GENCUTS_PythiaHiggsType_H
#define GENCUTS_PythiaHiggsType_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/IParticlePropertySvc.h"

// from Generators
#include "MCInterfaces/IGenCutTool.h"

/** @class PythiaHiggsType PythiaHiggsType.h component/PythiaHiggsType.h
 *
 *  Tool to select events with, 1 or 2 b quarks in the acceptance coming 
 *  from a given mother, and/or 1 or 2 leptons in the acceptance with given 
 *  minimal pt and comming (or not) from
 *  a W or Z boson
 *
 *  @author Victor Coco, Neal Gauvin
 *  @date   2007-01-21
 */
class PythiaHiggsType : public GaudiTool , virtual public IGenCutTool {
public:
  /// Standard constructor
  PythiaHiggsType( const std::string & type , const std::string & name ,
                   const IInterface * parent ) ;
  
  virtual ~PythiaHiggsType( ); ///< Destructor  

  /// initialize function
  virtual StatusCode initialize( );

  /* Accept events following criteria given in options. 
   * Implements IGenCutTool::applyCut 
   */
  virtual bool applyCut( ParticleVector & theParticleVector ,
                         const HepMC::GenEvent * theEvent ,
                         const LHCb::GenCollision * theCollision ) const ;

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

  /// Minimum mass of the Mother of the lepton
  double m_MinMass ;


  /// Number of quarks required to be in the acceptance
  int m_nbbquarks;

  /// PDG id of the mother of the b quarks
  std::string m_motherofb_id;

  /// Substitute of the b quark, if any reqested
  std::string m_subb;

  int m_motherofb_pid;
  int m_b_pid;

  LHCb::IParticlePropertySvc* m_ppSvc;
  
};
#endif // GENCUTS_PythiaHiggsType_H
