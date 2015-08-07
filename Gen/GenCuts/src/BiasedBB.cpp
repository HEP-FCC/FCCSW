// $Id: BiasedBB.cpp,v 1.6 2008-07-09 14:32:00 robbep Exp $
// Include files 

// local
#include "BiasedBB.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Point4DTypes.h"
#include "GaudiKernel/Transform4DTypes.h"

// from LHCb
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h"

// from HepMC
#include "HepMC/GenParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BiasedBB
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( BiasedBB )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BiasedBB::BiasedBB( const std::string& type , const std::string& name ,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent ) , m_deltaPhiCut( false ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "EtaMax" , m_etaMax = 4.7 ) ;
    declareProperty( "EtaMin" , m_etaMin = 2.2 ) ;
    declareProperty( "PtMin" , m_ptMin = 8.4 * Gaudi::Units::GeV ) ;
    declareProperty( "VMin" , m_vMin = 26. * Gaudi::Units::GeV ) ;
    declareProperty( "CTauMin" , m_ctauMin = 0.16 * Gaudi::Units::mm ) ;
    declareProperty( "RadiusMin" , m_radiusMin = 0.0 * Gaudi::Units::mm ) ;
    declareProperty( "NumberOfBMin" , m_nBMin = 1 ) ;
    declareProperty( "MinBDeltaPhi" ,m_deltaPhiMin = -1.) ;
    declareProperty( "MinChargedDaug" , m_nStableChargedDaugthers = -1 ) ;
  }

//=============================================================================
// Destructor 
//=============================================================================
BiasedBB::~BiasedBB( ) { ; }

StatusCode BiasedBB::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize base class !" ) ;

  if ( m_deltaPhiMin > 0. ){
    m_deltaPhiCut = true;
  }
  if ( m_nStableChargedDaugthers > 0 ){
    IParticlePropertySvc * ppSvc = 
      svc< IParticlePropertySvc > ( "ParticlePropertySvc" ) ;
    const ParticleProperty * prop = ppSvc -> find( "pi+" ) ;
    m_chargedPIDList.insert( prop->pythiaID ()  );
    prop = ppSvc -> find( "mu+" ) ;
    m_chargedPIDList.insert( prop->pythiaID ()  );
    prop = ppSvc -> find( "e+" ) ;
    m_chargedPIDList.insert( prop->pythiaID ()  );
    prop = ppSvc -> find( "p+" ) ;
    m_chargedPIDList.insert( prop->pythiaID ()  );
    prop = ppSvc -> find( "K+" ) ;
    m_chargedPIDList.insert( prop->pythiaID ()  );  
    release( ppSvc ) ;
  }
  return sc ;
}


//=============================================================================
// Acceptance function
//=============================================================================
bool BiasedBB::applyCut( ParticleVector & theParticleVector ,
                         const HepMC::GenEvent * /* theGenEvent */ ,
                         const LHCb::GenCollision * /* col */ ) const {
  double eta , pT , v , ctau  , radius;

  bool hasDeltaPhi = true ;

  std::vector<Gaudi::LorentzVector > lorentzVectB ;

  ParticleVector::iterator it ;
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {
    // Original Lorentz vector
    Gaudi::LorentzVector fourM( (*it) -> momentum() ) ;
    // Compute eta
    eta = fabs( fourM.Eta() ) ;    

    // Compute pT 
    pT = fourM.Pt() ;
    
    // Compute v
    v = pT + 5.360 * Gaudi::Units::GeV * eta ;

    // Compute ctau
    Gaudi::LorentzVector pEnd , pBegin ;
    

    pEnd.SetXYZT( (*it) -> end_vertex() -> position() . x() ,
                  (*it) -> end_vertex() -> position() . y() , 
                  (*it) -> end_vertex() -> position() . z() ,
                  (*it) -> end_vertex() -> position() . t() * Gaudi::Units::c_light ) ;

    pBegin.SetXYZT( (*it) -> production_vertex() -> position() . x() ,
                    (*it) -> production_vertex() -> position() . y() ,
                    (*it) -> production_vertex() -> position() . z() ,
                    (*it) -> production_vertex() -> position() . t() * 
                    Gaudi::Units::c_light ) ;  

    Gaudi::LorentzVector disp = pEnd - pBegin ;

    
    
    // Boost transformation
    ROOT::Math::Boost theBoost( fourM.BoostToCM() ) ;

    Gaudi::LorentzVector restDisp = theBoost( disp ) ;

    ctau = restDisp.t() ;

    // Compute radius from 0,0
    radius = sqrt( pEnd.x()*pEnd.x() + pEnd.y()*pEnd.y() );

    int nDaud = 0 ;
    if ( m_nStableChargedDaugthers > 0 ){
      HepMC::GenVertex * EV = (*it) -> end_vertex() ;
      if ( 0 == EV ) return true ;
      typedef std::vector< HepMC::GenParticle * > Particles ;
      Particles selectedDaughters ;
      HepMC::GenVertex::particle_iterator iter ;
      // select stable particles from B with pid in m_chargedPIDList
      for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
            iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
        if ( std::binary_search( m_chargedPIDList.begin() , 
                                 m_chargedPIDList.end() , 
                                 (*iter) -> pdg_id() ) && 0 == (*iter) -> end_vertex() ){
          
          // Don't use daughters of Lambda and KS:
          HepMC::GenParticle * theParent = *( (*iter) -> production_vertex() -> particles_in_const_begin() ) ;
          if ( 3122 == abs( theParent -> pdg_id() ) ) continue ;
          if ( 310 == theParent -> pdg_id() ) continue ;
          selectedDaughters.push_back( *iter ) ;
        }
        
      }

      // if no found, try CP conjugates
      if ( selectedDaughters.empty() ) {
        for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
              iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
          if ( std::binary_search( m_chargedPIDList.begin() , 
                                   m_chargedPIDList.end() , 
                                   -(*iter) -> pdg_id() ) && 0 == (*iter) -> end_vertex() ){
            // Don't use daughters of Lambda and KS:
            HepMC::GenParticle * theParent  = 
              *( (*iter) -> production_vertex() -> particles_in_const_begin() ) ;
            if ( 3122 == abs( theParent -> pdg_id() ) ) continue ;
            if ( 310 == theParent -> pdg_id() ) continue ;
            selectedDaughters.push_back( *iter ) ;
          }
        }
      }
      nDaud = selectedDaughters.size();
    } 

    if ( ( eta < m_etaMin ) || ( eta > m_etaMax ) || ( pT < m_ptMin ) ||
         ( v < m_vMin ) || ( ctau < m_ctauMin ) || (radius < m_radiusMin) || nDaud < m_nStableChargedDaugthers ) {
      debug() << "Particle rejected: [eta = " << eta 
              << "][pT = " << pT / Gaudi::Units::GeV << " GeV][v = " 
              << v / Gaudi::Units::GeV << " GeV][ctau = " 
              << ctau / Gaudi::Units::mm << " mm][radius = " 
              << radius / Gaudi::Units::mm << " mm]" 
              << endmsg ;
      it = theParticleVector.erase( it );
    } else {
      debug() << "Particle accepted: [eta = " << eta 
              << "][pT = " << pT / Gaudi::Units::GeV << " GeV][v = " 
              << v / Gaudi::Units::GeV << " GeV][ctau = " 
              << ctau / Gaudi::Units::mm << " mm][radius = " 
              << radius / Gaudi::Units::mm << " mm][NchDaug =" 
               << nDaud << "]"
              << endmsg ;
      if ( m_deltaPhiCut) lorentzVectB.push_back(disp);
      ++it ;
    }
  }

  double dbg_maxDphi = -1000.;
  if ( m_deltaPhiCut){
    hasDeltaPhi = false ;
    for (std::vector< Gaudi::LorentzVector >::iterator ilvB = lorentzVectB.begin() ; lorentzVectB.end() != ilvB ; ilvB++ ){
      for (std::vector< Gaudi::LorentzVector >::iterator  ilvB2 = ilvB ; lorentzVectB.end() != ilvB2 ; ilvB2++ ){
        double dphi = 0.;
        if((*ilvB).Phi() > (*ilvB2).Phi())dphi= (*ilvB).Phi() - (*ilvB2).Phi();
        else dphi= (*ilvB2).Phi() - (*ilvB).Phi();
        if( dphi >= 3.1415926 ) dphi = 2*3.1415926 - dphi;
        if (dphi>dbg_maxDphi)dbg_maxDphi=dphi;
        
        if ( dphi > m_deltaPhiMin){ 
          hasDeltaPhi = true ;
          continue;
        }
      }
      if(hasDeltaPhi) continue;
    } 
  }
  debug()<<"Size "<< theParticleVector.size()<<" deltaPhi "<<hasDeltaPhi<<" dphi= "<<dbg_maxDphi<<endreq;
  
  return (  theParticleVector.size() >=  m_nBMin  && hasDeltaPhi ) ;
}

