// $Id: DiLeptonInAcceptance.cpp,v 1.4 2009-09-09 17:01:57 tblake Exp $
// Include files 

// local
#include "DiLeptonInAcceptance.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/SystemOfUnits.h"

//#include "GaudiKernel/ToolFactory.h"
#include "Event/HepMCEvent.h"


//-----------------------------------------------------------------------------
// Implementation file for class : DiLeptonInAcceptance
//
// 2007-01-30 : Thomas Blake
//-----------------------------------------------------------------------------



DECLARE_TOOL_FACTORY( DiLeptonInAcceptance )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DiLeptonInAcceptance::DiLeptonInAcceptance( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IFullGenEventCutTool>( this );
  
  declareProperty( "LeptonIDOne" , m_leptonOnePDG = 13 );
  declareProperty( "LeptonIDTwo" , m_leptonTwoPDG = 13 );
  
  declareProperty( "LeptonOnePtMin", m_ptMinLOne = 0*Gaudi::Units::GeV );
  declareProperty( "LeptonOnePMin", m_pMinLOne = 0*Gaudi::Units::GeV );
  declareProperty( "LeptonTwoPtMin", m_ptMinLTwo = 0*Gaudi::Units::GeV );
  declareProperty( "LeptonTwoPMin", m_pMinLTwo = 0*Gaudi::Units::GeV );
  declareProperty( "LeptonOneMinTheta"  , m_thetaMinLOne = 10*Gaudi::Units::mrad );
  declareProperty( "LeptonOneMaxTheta"  , m_thetaMaxLOne = 400*Gaudi::Units::mrad );
  declareProperty( "LeptonTwoMinTheta"  , m_thetaMinLTwo = 10*Gaudi::Units::mrad );
  declareProperty( "LeptonTwoMaxTheta"  , m_thetaMaxLTwo = 400*Gaudi::Units::mrad );
  

  declareProperty( "MinMass"   , m_minMass = 0*Gaudi::Units::GeV );
  declareProperty( "MaxMass"   , m_maxMass = 100*Gaudi::Units::GeV );
 
  declareProperty( "RequireOppositeSign" , m_oppSign = true );
  declareProperty( "RequireSameSign" , m_sameSign = false );

  declareProperty( "PreselDoca"   , m_PreselDoca = false );
  declareProperty( "DocaCut"      , m_docaCut = 0.4*Gaudi::Units::mm );

  declareProperty( "PreselPtProd" , m_PreselPtProd = false );
  declareProperty( "PtProdMinCut" , m_ptprodMinCut = 1*Gaudi::Units::GeV*1*Gaudi::Units::GeV);
  declareProperty( "PtProdMaxCut" , m_ptprodMaxCut = 4*Gaudi::Units::GeV*4*Gaudi::Units::GeV);

}

//=============================================================================
// Destructor
//=============================================================================

DiLeptonInAcceptance::~DiLeptonInAcceptance() {} 


//=============================================================================
// Selection function
//=============================================================================
bool DiLeptonInAcceptance::studyFullEvent( LHCb::HepMCEvents * theEvents ,
                                         LHCb::GenCollisions * /* col */ )
  const {
  
  std::list< HepMC::GenParticle* > leptonOneList;
  std::list< HepMC::GenParticle* > leptonTwoList;
  
  if ( m_oppSign && m_sameSign ) {
    warning() << " Trying to force both opposite and same sign leptons! Choosing same sign " << endmsg;
  }
  
  
  for (LHCb::HepMCEvents::iterator theEventIter = theEvents->begin() ;
       theEventIter != theEvents->end() ; ++theEventIter) {
    
    
    for ( HepMC::GenEvent::particle_const_iterator
            iParticle = ( *theEventIter ) -> pGenEvt() -> particles_begin() ;
          iParticle != ( *theEventIter ) -> pGenEvt() -> particles_end() ; 
          ++iParticle ){
      
      HepMC::GenParticle* particle = (*iParticle);
      
      int absid = abs(particle->pdg_id());
      
      if ( m_leptonOnePDG == absid && //isInAcceptance( particle ) ){
           isInAcceptance( particle, m_pMinLOne, m_ptMinLOne, m_thetaMinLOne, m_thetaMaxLOne )  ){
        debug()<<" Found lepton ["<<m_leptonOnePDG<<"] in accpetance"<<endmsg;
        leptonOneList.push_back( particle );
      }
      
      if ( m_leptonTwoPDG == absid && //isInAcceptance( particle ) ) {
           isInAcceptance( particle, m_pMinLTwo, m_ptMinLTwo, m_thetaMinLTwo, m_thetaMaxLTwo )  ){
        debug()<<" Found lepton ["<<m_leptonTwoPDG<<"] in accpetance"<<endmsg;
        leptonTwoList.push_back( particle );
      }
    } 
  }
  
  
  if ( leptonOneList.empty() || leptonTwoList.empty() ){
    debug()<<"No leptons found in this event"<<endmsg;
    return false;
  }
  
  // apply dimuon mass constraint
  double dimuonMass;
  double tpx1, tpy1, tpt1, tpx2, tpy2, tpt2, tptprod  ;

  double doca; double modpd;
  double pd[3], x[3]; double p1_x, p2_x, p1_y, p2_y, p1_z, p2_z; 

  std::list< HepMC::GenParticle * >::const_iterator iterOne;
  std::list< HepMC::GenParticle * >::const_iterator iterTwo;
  
  bool passMassCut = false; 
  bool passPtProdCut = false; 

  for ( iterOne = leptonOneList.begin() ; 
        iterOne != leptonOneList.end() ; ++iterOne ) {      
    
    for ( iterTwo = leptonTwoList.begin() ; 
          iterTwo != leptonTwoList.end() ; ++iterTwo ){
      
      // Check combination
      if ( !isCombination( (*iterOne), (*iterTwo) ) ) continue;
      
      // Apply Di Muon Mass Cut
      HepMC::FourVector sum ;
      sum.setPx( (*iterOne) -> momentum().px() + (*iterTwo)->momentum().px() ) ;
      sum.setPy( (*iterOne) -> momentum().py() + (*iterTwo)->momentum().py() ) ;
      sum.setPz( (*iterOne) -> momentum().pz() + (*iterTwo)->momentum().pz() ) ;
      sum.setE ( (*iterOne) -> momentum().e()  + (*iterTwo)->momentum().e() ) ;
      dimuonMass = sum.m();
      
      passMassCut = false;
      if ( dimuonMass >= m_minMass && dimuonMass <= m_maxMass ){
        debug()<<" Combination passes mass cut "<< endmsg;
        passMassCut = true;

        if(!m_PreselDoca && !m_PreselPtProd) {
          debug()<<"PASS : passMass "<<passMassCut<<" selDoca "<<m_PreselDoca<<" selPt "<<m_PreselPtProd<<endmsg;
          return true;
        }
  
      } else { 
        debug()<<"1CONT : passMass "<<passMassCut<<" selDoca "<<m_PreselDoca<<" selPt "<<m_PreselPtProd<<endmsg;
        continue;
      }

      passPtProdCut = false;
      if(m_PreselPtProd) {
        // Apply PtProd Cut
        tpx1 = (*iterOne) -> momentum().px();
        tpy1 = (*iterOne) -> momentum().py();
	
        tpx2 = (*iterTwo) -> momentum().px();
        tpy2 = (*iterTwo) -> momentum().py();
	
        tpt1 = sqrt( tpx1*tpx1 + tpy1*tpy1);
        tpt2 = sqrt( tpx2*tpx2 + tpy2*tpy2);
	
        tptprod = tpt1*tpt2;
        debug()<<" PTPROD:: "<<tptprod<<endmsg;

        if ( tptprod <= m_ptprodMaxCut && tptprod >= m_ptprodMinCut ) {
          debug()<<" Combination passes ptprod cut "<< endmsg;
          passPtProdCut = true;
          if(passMassCut && !m_PreselDoca) {
            debug()<<"Out : passMass "<<passMassCut<<" passPt  "<<passPtProdCut<<" selDOCA "<<m_PreselDoca<<endmsg;
            return true;
          } 
    
        } else { 
          debug()<<"2CONT : passMass "<<passMassCut<<" selPt "<<m_PreselPtProd<<" passPt "<<passPtProdCut<<endmsg;
          continue;
        }
  
      }


      if(m_PreselDoca) {
        // Apply DOCA Cut
        p1_x = (*iterOne) -> momentum().px(); 	p2_x = (*iterTwo) -> momentum().px();
        p1_y = (*iterOne) -> momentum().py(); 	p2_y = (*iterTwo) -> momentum().py();
        p1_z = (*iterOne) -> momentum().pz(); 	p2_z = (*iterTwo) -> momentum().pz();
	
        pd[0] = p1_y*p2_z - p1_z*p2_y;
        pd[1] = p1_z*p2_x - p1_x*p2_z;
        pd[2] = p1_x*p2_y - p1_y*p2_x;
	
        modpd = sqrt(pd[0]*pd[0]+pd[1]*pd[1]+pd[2]*pd[2]);
        if (modpd == 0) {
          doca = -1;
          //Need to handle also parallel tracks
        } else {
          pd[0] = pd[0]/modpd;
          pd[1] = pd[1]/modpd;
          pd[2] = pd[2]/modpd;

          const HepMC::GenVertex * vert1 = (*iterOne)->production_vertex();
          const HepMC::GenVertex * vert2 = (*iterTwo)->production_vertex();

          HepMC::ThreeVector pos1 = vert1->point3d();
          HepMC::ThreeVector pos2 = vert2->point3d();

          x[0] = pos1.x()-pos2.x();
          x[1] = pos1.y()-pos2.y();
          x[2] = pos1.z()-pos2.z(); 

          //Scalar product of perp and distance
          doca = fabs(x[0]*pd[0]+x[1]*pd[1]+x[2]*pd[2]); 
        }
        debug()<<" DOCA:: "<<doca<< endmsg;
        if(doca >= 0 && doca <= m_docaCut) {
          debug()<<" Combination passes doca cut "<<endmsg;
          if(passMassCut && (passPtProdCut || !m_PreselPtProd)) {
            debug()<<"Out: passMass "<<passMassCut<<" passDOCA True passPt  "<<passPtProdCut<<" selDOCA "<<m_PreselDoca<<endmsg;
            return true;
          }
    
        } else { 
          debug()<<"3CONT: passMass "<<passMassCut<<" selPt "<<m_PreselPtProd<<" passPt "<<passPtProdCut<<" selDOCA "<<m_PreselDoca<<" passDOCA False "<<endmsg;
          continue;
        }
        
      }


    }
  }  
  
  return false;
}


bool DiLeptonInAcceptance::isCombination( const HepMC::GenParticle *particleOne, 
                                          const HepMC::GenParticle *particleTwo )
  const {
  
  if ( particleOne == particleTwo ) return false ;
  
  int combination =  (particleOne->pdg_id())*(particleTwo->pdg_id()) ;
  
  // Same sign takes precedence 
  if ( m_sameSign ) { 
    if ( combination < 0 ) return false ;
  } 
  else if ( m_oppSign ) {
    if ( combination > 0 ) return false;
  }
  
  return true ;
}



bool DiLeptonInAcceptance::isInAcceptance( const HepMC::GenParticle *p, 
                                           const double pmin, 
                                           const double ptmin, 
                                           const double thetamin, 
                                           const double thetamax ) const 
{
  
   double px, py, pz, pp, theta, pt;
   
   pz = p->momentum().pz();
   
   if (pz < 0) return false;
   
   px = p->momentum().px();
   py = p->momentum().py();
   
   pp = sqrt( px*px + py*py + pz*pz );
   if ( pp < pmin ) return false ;
   
   pt = sqrt( px*px + py*py);
   if ( pt < ptmin ) return false ;
   
   theta = acos( pz / pp );
   
   return ( theta <= thetamax && 
            theta >= thetamin );
}



//=============================================================================
