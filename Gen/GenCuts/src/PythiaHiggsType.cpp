// Include files
// local
#include "PythiaHiggsType.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"

#include "Event/ParticleProperty.h"
#include "Event/ParticleID.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PythiaHiggsType
//
// 2009-Oct-1 : Neal Gauvin, Victor Coco
//
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( PythiaHiggsType )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PythiaHiggsType::PythiaHiggsType( const std::string & type , 
                                  const std::string & name ,
                                  const IInterface * parent )
  : GaudiTool  ( type, name , parent ) ,
    m_thetaMax         ( 400 * Gaudi::Units::mrad ) ,
    m_nbLepton         ( 1          ) ,
    m_TypeLepton       ( ) ,  
    m_ptMin            ( 10 * Gaudi::Units::GeV   ) ,
    m_leptonFromMother ( true       ) ,
    m_motheroflepton   ( ) ,
    m_MinMass          (-1.),
    m_nbbquarks        ( 1          ) ,
    m_motherofb_id     ( "H_10" ) ,
    m_motherofb_pid    ( 0 ) ,
    m_b_pid            ( 0 ) ,
    m_ppSvc            ( 0 )
{
  declareInterface< IGenCutTool >( this ) ;

  // default properties are set for Higgs production with the 2b 
  // in the acceptance and a Lepton with pt>10GeV from WZ

  //Rough definition of the acceptance
  declareProperty( "ThetaMax" , m_thetaMax) ; //default=400 * mrad

  //Number of leptons requiered to be in the acceptance
  declareProperty( "NumberOfLepton" , m_nbLepton) ; //max 2 leptons -- 
                                                    //0 - 1 - 2 default=1
  m_TypeLepton.push_back ( "e+"  ) ;
  m_TypeLepton.push_back ( "mu+" ) ;                                                 
  //Type of leptons requiered to be in the acceptance // default e and mu
  declareProperty( "TypeOfLepton" , m_TypeLepton) ; 

  //LeptonPtMin is the minimum value the pt of lepton can have
  declareProperty( "LeptonPtMin" , m_ptMin) ; //default=10*GeV
	
  //LeptonIsFromMother enabled will requiered that the lepton comes from a specific mother
  declareProperty( "LeptonIsFromMother" , m_leptonFromMother) ; //default=true

  m_motheroflepton.push_back ( "W+"  ) ;
  m_motheroflepton.push_back ( "Z0" ) ; 
  //MotherOfLepton is the list of particles from which the lepton comes
  declareProperty( "MotherOfLepton" , m_motheroflepton) ; //default W and Z
  declareProperty( "MotherOfLeptonMinMass" , m_MinMass) ;	//use for Z/gamma*
  //Number of b quarks required to be in the acceptance
  declareProperty( "NumberOfbquarks" , m_nbbquarks ) ; //max 2 b quarks -- 
  // 0(only with pzb>0), 1, 2  and -1(desactivated) default=1
  // BECAREFULL NumberOfbquarks=0 will search b comming from mother 
  // with pz>0 if you don't want b (ex: Z->mumu) put -1.

  // MotherOfThebquarks define the mother of the b quarks
  declareProperty( "MotherOfThebquarks" , m_motherofb_id) ; //default=Higgs H_10

  //You should be able to substitute the b by another particle in this program.
  //For example a mu
  declareProperty( "SubstitutebBy" , m_subb = "No" );

}

//=============================================================================
// Destructor
//=============================================================================
PythiaHiggsType::~PythiaHiggsType( ) { ; }

//=============================================================================
// Initialise
//============================================================================
StatusCode PythiaHiggsType::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return sc ;
  
  if ( ( "t" == m_motherofb_id ) && ( 2 == m_nbbquarks ) ) { // remplacer ici par les fonction de PID
    info() << "You want to have both b in the acceptance, " 
           << "check in pythia commands that you ask both top to decay in b" 
           << endmsg;
  }

  if ( ( 1 != m_nbLepton ) && ( 2 != m_nbLepton ) && ( 0 != m_nbLepton ) ) {
    fatal() << "The only choices for NumberOfLepton property are 0, 1 or 2" 
            << endmsg;
    return StatusCode::FAILURE;
  }

  if ( ( 1 != m_nbbquarks ) && ( 2 != m_nbbquarks ) && ( 0 != m_nbbquarks ) && 
       ( -1 != m_nbbquarks ) ) {
    fatal() << "The only choices for NumberOfbquarks property are -1,0,1 or 2"
            << endmsg;
    return StatusCode::FAILURE;
  }
  m_ppSvc = svc<LHCb::IParticlePropertySvc>("ParticlePropertySvc", true);
  m_motherofb_pid = abs( m_ppSvc->find( m_motherofb_id )->pdgID().pid() );
  if( m_subb == "No" ){
    m_b_pid = abs( m_ppSvc->find("b" )->pdgID().pid() );
  } else {
    m_b_pid = abs( m_ppSvc->find( m_subb )->pdgID().pid() );
  }

  return sc ;
}

//=============================================================================
// b quark tagging
//=============================================================================
bool PythiaHiggsType::Isb( const HepMC::GenParticle * p) const {
  if ( abs(p->pdg_id())== m_b_pid ) {
    HepMC::GenVertex * thePV =  p->production_vertex() ;
    HepMC::GenVertex::particle_iterator iter ;
    for(iter = thePV->particles_begin( HepMC::parents);
        iter != thePV->particles_end(HepMC::parents); ++iter){
      if( abs( m_motherofb_pid ) == abs( (*iter)->pdg_id() )  ) return true;
    }
  }
  return false;
}

//=============================================================================
// look for lepton
//=============================================================================
bool PythiaHiggsType::IsLepton( const HepMC::GenParticle * p ) const {
  bool isalepton = false ;
  for ( std::vector< std::string >::const_iterator iPart = m_TypeLepton.begin();
	iPart != m_TypeLepton.end(); ++iPart ){
    std::string thepid = *iPart;
    if ( abs(p->pdg_id()) == abs (m_ppSvc->find(thepid)->pdgID().pid())) 
      isalepton = true;
  }
  if( isalepton == true ){
    if ( ! m_leptonFromMother ) return true ;
    else {
      HepMC::GenVertex * thePV =  p -> production_vertex() ;
      HepMC::GenVertex::particle_iterator iter ;
      for(iter = thePV -> particles_begin( HepMC::parents);
          iter != thePV -> particles_end(HepMC::parents); ++iter){
        for ( std::vector< std::string >::const_iterator iPart = m_motheroflepton.begin();
              iPart != m_motheroflepton.end(); ++iPart )
	  {
	    std::string thepid = *iPart;
	    if(  abs( m_ppSvc->find(thepid)->pdgID().pid() ) == abs( (*iter)->pdg_id() ) )
	      if( (*iter)->momentum().m() > m_MinMass )
		return true;
	  }
      }
    }
  }
  return false;
}

//=============================================================================
// Accept function
//=============================================================================
bool PythiaHiggsType::applyCut( ParticleVector & /* theParticleVector */ ,
				const HepMC::GenEvent * theEvent ,
				const LHCb::GenCollision * /* theCollision */ )
  const {
  // Selection of the b quarks
  std::list< const HepMC::GenParticle * > bList ;
  for(HepMC::GenEvent::particle_const_iterator iterall = 
        theEvent -> particles_begin() ;
      iterall!= theEvent -> particles_end();iterall++) {
    if ( Isb( *(iterall) ) ) bList.push_back( *(iterall) ) ;
  }
  
  
  const HepMC::GenParticle * theb1(0), *theb2(0);
  std::list<const HepMC::GenParticle * >::iterator iterb = bList.begin() ;
  if( 2<= bList.size() ) {
    theb1 = *(iterb); 
    theb2 = *(++iterb);
  }

  if ( ( 1 == bList.size() ) && ( m_motherofb_id != "t" ) ) {
    Warning( "No b pairs in this event!" ) ;
    //can occure in ttbar event when one top decay on another mode than bW
    return false ;
  }
	
  if( ( 1 == bList.size() ) && ( m_motherofb_id=="t" ) ) {  // remplacer ici par les fonction de PID
    theb1 = *(iterb);
    theb2 = *(iterb);
  }
  
  if( ( 0== bList.size() ) && ( m_nbbquarks != -1 ) ) {
    Warning( "No b in this event!" ) ;
    return false ;
  }

  double pzb1,thetab1;
  double pzb2,thetab2;
  if( ( m_nbbquarks != -1 ) && ( bList.size() > 0 ) ) {
    pzb1 = theb1->momentum().pz() * Gaudi::Units::GeV ;
    thetab1 = theb1->momentum().theta();
    pzb2 = theb2->momentum().pz() * Gaudi::Units::GeV ;
    thetab2 = theb2->momentum().theta();
  }

  if( ( 1== bList.size() ) && ( m_motherofb_id== "t" ) ) { 
    // necessary in order to avoid case where we ask 2b in ttbar event 
    // and that only one top decay in b in the acceptance
    pzb2=-1000;
    thetab2=1000000;
  }

  if( 0 == bList.size() ) {
    pzb2=-1000;
    thetab2=1000000;
    pzb1=-1000;
    thetab1=1000000;
  }

  // Selection of the lepton
  std::vector< const HepMC::GenParticle * > LeptonList ;
  for ( HepMC::GenEvent::particle_const_iterator iterall = 
          theEvent -> particles_begin() ;
        iterall!= theEvent -> particles_end() ; iterall++ ) {
    if ( IsLepton( *(iterall) ) ) LeptonList.push_back( *(iterall) ) ;
  }

  if( ( 0 == LeptonList.size() ) ) {
    if( msgLevel( MSG::DEBUG ) ){
      debug()<<"No lepton in this event of requiered type "
             <<"with requiered mother (if mother was asked)!"<< endmsg;   
      debug()<<"You either produced events with no leptons, "
             <<"or put the wrong type of lepton, or of lepton mother"<< endmsg;
    }
    return false ;
  }

  double pzl1,ptl1,thetal1;
  double pzl2,ptl2,thetal2;
  if ( m_nbLepton <= ( int ) LeptonList.size() ) {
    const HepMC::GenParticle * theLepton1(0), *theLepton2(0);
    std::vector<const HepMC::GenParticle * >::iterator iterLepton = 
      LeptonList.begin() ;
    if ( 2 <= LeptonList.size() ) {
      theLepton1 = *(iterLepton);
      theLepton2 = *(++iterLepton);
    }
    if( 1== LeptonList.size() ) {
      theLepton1 = *(iterLepton);
      theLepton2 = *(iterLepton);
    }



    pzl1 = theLepton1->momentum().pz() ;
    thetal1 = theLepton1->momentum().theta();
    ptl1 = theLepton1->momentum().perp();
    pzl2 = theLepton2->momentum().pz() ;
    thetal2 = theLepton2->momentum().theta();
    ptl2 = theLepton2->momentum().perp();

    if ( 1 == LeptonList.size() ) {
      thetal2= 100000;
      ptl2=-1000;
      pzl2=-1000;
    }
  } else {
    thetal1= 100000;
    ptl1=-1000;
    pzl1=-1000;
    thetal2= 100000;
    ptl2=-1000;
    pzl2=-1000;
  }

  if((m_nbbquarks==-1 &&  m_nbLepton ==0)// no cut
     ||(m_nbbquarks==-1 &&  m_nbLepton ==1 && 
	( ( ( thetal1 <= m_thetaMax )&& ( pzl1 >= 0. ) && ( ptl1 >= m_ptMin ) )
	  || ( ( thetal2 <= m_thetaMax ) && ( pzl2 >= 0. )  && 
	       ( ptl2 >= m_ptMin ) ) ) )// no b cut on 1 lepton
     ||(m_nbbquarks==-1 &&  m_nbLepton ==2 && 
        ( ( ( thetal1 <= m_thetaMax )&& ( pzl1 >= 0. ) && 
            ( ptl1 >= m_ptMin ) )&& ( ( thetal2 <= m_thetaMax ) && 
                                      ( pzl2 >= 0. )  
                                      && ( ptl2 >= m_ptMin ) ) ) )
     // no b cut on 2 lepton
     ||(m_nbbquarks==0 &&  m_nbLepton ==0 
        &&  (  ( pzb1 >= 0. )
               || (  pzb2 >= 0. ) ) ) // only cut on pzb no lepton cut
     || (m_nbbquarks==0 &&  m_nbLepton ==1 
         &&  ( ( pzb1 >= 0. )
               || ( pzb2 >= 0. ) ) &&  
         ( ( ( thetal1 <= m_thetaMax )&& ( pzl1 >= 0. ) 
             && ( ptl1 >= m_ptMin ) )|| ( ( thetal2 <= m_thetaMax ) 
                                          && ( pzl2 >= 0. )  && 
                                          ( ptl2 >= m_ptMin ) ) ) )
     // only cut on pzb and 1 lepton
     ||(m_nbbquarks==0 &&  m_nbLepton ==2 
        &&  ( ( pzb1 >= 0. )
              || ( pzb2 >= 0. ) ) &&  ( ( ( thetal1 <= m_thetaMax )
                                          && ( pzl1 >= 0. ) && 
                                          ( ptl1 >= m_ptMin ) ) && 
                                        ( ( thetal2 <= m_thetaMax ) && 
                                          ( pzl2 >= 0. )  && 
                                          ( ptl2 >= m_ptMin ) ) ) )
     // only cut on pzb and 2 lepton
     ||(m_nbbquarks==1 &&  m_nbLepton ==0 &&  ( ( ( thetab1 <= m_thetaMax ) 
                                                  && ( pzb1 >= 0. ) )
                                                || ( ( thetab2 <= m_thetaMax )
                                                     && ( pzb2 >= 0. ) ) ) )
     // cut on 1b no lepton cut
     || (m_nbbquarks==1 &&  m_nbLepton ==1 &&  ( ( ( thetab1 <= m_thetaMax ) 
                                                   && ( pzb1 >= 0. ) )
                                                 || ( ( thetab2 <= m_thetaMax )
                                                      && ( pzb2 >= 0. ) ) )
         &&  ( ( ( thetal1 <= m_thetaMax )&& ( pzl1 >= 0. ) && 
                 ( ptl1 >= m_ptMin ) )|| ( ( thetal2 <= m_thetaMax ) && 
                                           ( pzl2 >= 0. )  && 
                                           ( ptl2 >= m_ptMin ) ) ) )
     // cut on 1b and 1 lepton
     ||(m_nbbquarks==1 &&  m_nbLepton ==2 &&  
        ( ( ( thetab1 <= m_thetaMax ) && ( pzb1 >= 0. ) )
          || ( ( thetab2 <= m_thetaMax ) && ( pzb2 >= 0. ) ) ) &&  
        ( ( ( thetal1 <= m_thetaMax )&& ( pzl1 >= 0. ) && 
            ( ptl1 >= m_ptMin ) ) && ( ( thetal2 <= m_thetaMax ) && 
                                       ( pzl2 >= 0. )  && ( ptl2 >= m_ptMin ) )
          ) )// cut on 1b and 2 lepton
     ||(m_nbbquarks==2 &&  m_nbLepton ==0 &&  ( ( ( thetab1 <= m_thetaMax ) 
                                                  && ( pzb1 >= 0. ) )
                                                && ( ( thetab2 <= m_thetaMax )
                                                     && ( pzb2 >= 0. ) ) ) )
     // cut on 2b no lepton cut
     ||(m_nbbquarks==2 &&  m_nbLepton ==1 &&  ( ( ( thetab1 <= m_thetaMax ) 
                                                  && ( pzb1 >= 0. ) )
                                                && ( ( thetab2 <= m_thetaMax )
                                                     && ( pzb2 >= 0. ) ) )
        &&  ( ( ( thetal1 <= m_thetaMax )&& ( pzl1 >= 0. ) && 
                ( ptl1 >= m_ptMin ) ) || ( ( thetal2 <= m_thetaMax ) && 
                                           ( pzl2 >= 0. )  && 
                                           ( ptl2 >= m_ptMin ) ) ) )
     // cut on 2b and 1 lepton
     ||(m_nbbquarks==2 &&  m_nbLepton ==1 &&  ( ( ( thetab1 <= m_thetaMax ) 
                                                  && ( pzb1 >= 0. ) )
                                                && ( ( thetab2 <= m_thetaMax )
                                                     && ( pzb2 >= 0. ) ) ) && 
        ( ( ( thetal1 <= m_thetaMax )&& ( pzl1 >= 0. ) && ( ptl1 >= m_ptMin ) )
          && ( ( thetal2 <= m_thetaMax ) && ( pzl2 >= 0. )  && 
               ( ptl2 >= m_ptMin ) ) ) )// cut on 2b and 2 lepton
     ) {
    if( msgLevel( MSG::DEBUG ) ){
      debug()  << "Event passed with requierement of "
               << m_nbLepton << " lepton and " 
               << m_nbbquarks <<" bquarks" << endmsg ;
      debug()  << " [thetab1 = " << thetab1<< "] [thetab2 = " << thetab2 
               << "] " << "[pzb1 = " << pzb1 << "] [pzb2 = " << pzb2
               << " [thetal1 = " << thetal1 << "] [pzl1 = " << pzl1 
               << "] [ptl1 = " << ptl1 << "] [thetal2 = "
               << thetal2 << "] [pzl2 = " << pzl2 << "] [ptl2 = " << ptl2 
               << "]" << endmsg ;
    }
    return true;

  } else {
    if( msgLevel( MSG::DEBUG ) ){
      debug() << "Event rejected with requierement of "<< m_nbLepton
              << " lepton and " << m_nbbquarks << " bquarks" << endmsg ;
      debug() << " [thetab1 = " << thetab1 << "] [thetab2 = " 
              << thetab2 << "] " << "[pzb1 = " << pzb1 
              << "] [pzb2 = " << pzb2 << " [thetal1 = "<< thetal1 
              << "] [pzl1 = "<< pzl1 << "] [ptl1 = "<< ptl1 << "] [thetal2 = "
              << thetal2 << "] [pzl2 = "<< pzl2 << "] [ptl2 = "<< ptl2 << "]" 
              << endmsg ;
    }
    return false;
  }
  return false ;
}



