// local
#include "DMuCascadeInAcc.h"
#include <iostream>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Kernel
#include "Event/ParticleID.h"
#include "GaudiKernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "Event/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DMuCascadeInAcc
//
// 2011-07-26 : Adam Webber
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DMuCascadeInAcc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DMuCascadeInAcc::DMuCascadeInAcc( const std::string& type,
    const std::string& name,
    const IInterface* parent )
: GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10   * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400  * Gaudi::Units::mrad ) ;
  declareProperty( "MuonPtMin"       , m_muonptmin       = 0.5  * Gaudi::Units::GeV ) ;
  declareProperty( "MuonPMin"        , m_muonpmin        = 1.0  * Gaudi::Units::GeV ) ;
  declareProperty( "HadronPtMin"     , m_hadronptmin     = 0.25 * Gaudi::Units::GeV ) ;
  declareProperty( "HadronPMin"      , m_hadronpmin      = 2.0  * Gaudi::Units::GeV ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
DMuCascadeInAcc::~DMuCascadeInAcc( ) { ; }

//=============================================================================
// Cut Function
//=============================================================================
bool DMuCascadeInAcc::applyCut( ParticleVector & theParticleVector ,
    const HepMC::GenEvent * /* theEvent */ ,
    const LHCb::GenCollision * /* theHardInfo */ )
const {
  ParticleVector::iterator it ;
  int countPar = 0;
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ++it ) {
    
    // Initialise event filter conditions:
    int numCharmDau=0;
    bool hasMuon = 0;
    bool hasHadrons = 0;
    bool hasMuonAndHadrons = 0;

    // Check parent isn't stable:
    HepMC::GenVertex *EV = (*it) -> end_vertex(); 
    if ( EV == 0 ) continue;

    if(msgLevel(MSG::DEBUG)){
      countPar++;
      debug() << "=========================================\n" <<  endmsg ;
      debug() << "Parent number: " << countPar << " with ID = " << (*it)->pdg_id() << endmsg ;
    }

    LHCb::ParticleID parPID( (*it) -> pdg_id() ) ;
    if(!parPID.hasBottom()) continue;
    if(msgLevel(MSG::DEBUG)) debug() << "Parent contains a b quark" << endmsg ;

    // Loop through daughters:
    HepMC::GenVertex::particle_iterator iterDAU ;
    int countDau = 0;
    for ( iterDAU = EV->particles_begin( HepMC::children ); 
        iterDAU != EV->particles_end( HepMC::children ) ; ++iterDAU ){
      countDau++;
      if(msgLevel(MSG::DEBUG)) debug() << "   Daughter:   "<< (*iterDAU) -> pdg_id()<< endmsg ;
      LHCb::ParticleID dauPID( (*iterDAU) -> pdg_id() ) ;
      
      if (!dauPID.hasCharm()) continue;
      numCharmDau++;
      if(msgLevel(MSG::DEBUG)){
        debug() << "   Daughter " << countDau << " contains a c quark." << endmsg ;
        debug() << "   Current Number of charm daughters =  " << numCharmDau << endmsg ;
        debug() << "   Looking at granddaughters of daughter: " << countDau << endmsg ;
      }
      
      // Check if a daughter passes the cuts:
      passCuts(*iterDAU, hasMuon, hasHadrons, hasMuonAndHadrons);

    }// end of loop over daughters.

    if(msgLevel(MSG::DEBUG)){
      debug() << "\nEnd of parent summary:" << endmsg;
      debug() << "numCharmDau = " << numCharmDau << endmsg;
      debug() << "hasMuon = " << hasMuon << endmsg;
      debug() << "hasHadrons = " << hasHadrons << endmsg;
      debug() << "hasMuonAndHadrons = " << hasMuonAndHadrons << "\n" << endmsg;
    }

    if(numCharmDau>=2){
      if(    (hasMuon && hasHadrons)
          || (hasMuon && hasMuonAndHadrons)
          || (hasHadrons && hasMuonAndHadrons)
        ) {
        if(msgLevel(MSG::DEBUG)) debug() << "PASSED EVENT!!! \n" << endmsg ;
        return true ;
      }
    }  
  } // End of loop over parent particles.

  // If here return false:
  return false;
}

//=============================================================================
// Functions to test if Xc has a daughter muon in acceptance or if it goes to 
// >= 3 charged hadrons in acceptance
//=============================================================================
bool DMuCascadeInAcc::passCuts( const HepMC::GenParticle * theSignal
                             , bool &hasMuon, bool &hasHadrons
                             , bool &hasMuonAndHadrons  
                             ) const {
  bool hasMuonInAcc=0;
  bool hasHadronsInAcc=0;

  // Loop through descendants of charm particle:
  HepMC::GenVertex *charmEV = theSignal->end_vertex();
  HepMC::GenVertex::particle_iterator iterDESC ;
  int countDESC = 0;
  int chargedHadronInAcc=0;
  for ( iterDESC = charmEV->particles_begin( HepMC::descendants ); 
      iterDESC != charmEV->particles_end( HepMC::descendants ) ; ++iterDESC ){
    countDESC++;
    if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"<< countDESC << " has pid = "<< (*iterDESC)->pdg_id()<< endmsg ;
    LHCb::ParticleID descPID( (*iterDESC)->pdg_id() ) ;
    // First check the particles is the right type and in acceptance:
    int PID = descPID.abspid();
    if (!(PID==13 || PID==211 || PID==321)) continue;
    double angle = (*iterDESC)->momentum().theta();
    if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"<< countDESC << " has angle = "<< angle/ Gaudi::Units::mrad << endmsg ;
    if(    ( fabs(sin(angle)) > fabs(sin(m_chargedThetaMax)) )
        || ( fabs(sin(angle)) < fabs(sin(m_chargedThetaMin)) )
      )  continue;
    if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter in in LHCb acceptance." << endmsg ;

    // For muon:
    if (!hasMuon && PID==13){
      if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"<< countDESC << " is a muon."<< endmsg ;
        double mpx, mpy, mpz, mpp, mpt;
        mpx = (*iterDESC)->momentum().px();
        mpy = (*iterDESC)->momentum().py();
        mpz = (*iterDESC)->momentum().pz();
        mpp = sqrt( mpx*mpx + mpy*mpy + mpz*mpz );
        mpt = sqrt( mpx*mpx + mpy*mpy);
        if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:" << countDESC 
          << " has P = " << mpp << "MeV, and Pt = " << mpt << "MeV." <<  endmsg ;
        if ( mpt<m_muonptmin || mpp<m_muonpmin) continue;
        if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"<< countDESC << " has passed momentum cuts. \n"<< endmsg ;
        hasMuonInAcc=1;
        if (hasMuonAndHadrons || hasHadrons){
          if(msgLevel(MSG::DEBUG)) debug() << "   Daughter has a muon that passes all cuts. \n"<< endmsg ;
          hasMuon=1;
          return 1;
        }
    } // end of muon search.

    // For hadrons:
    if (!hasHadrons && ( PID==211 || PID==321) ){
        if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"<< countDESC << " is a pi or K."<< endmsg ;
        double hpx, hpy, hpz, hpp, hpt;
        hpx = (*iterDESC)->momentum().px();
        hpy = (*iterDESC)->momentum().py();
        hpz = (*iterDESC)->momentum().pz();
        hpp = sqrt( hpx*hpx + hpy*hpy + hpz*hpz );
        hpt = sqrt( hpx*hpx + hpy*hpy);
        if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:" << countDESC 
          << " has P = " << hpp << "MeV, and Pt = " << hpt << "MeV." <<  endmsg ;
        if ( hpt<m_hadronptmin || hpp<m_hadronpmin) continue;
        if(msgLevel(MSG::DEBUG)) debug() << "       GrandDaughter:"<< countDESC << " has passed momentum cuts. \n"<< endmsg ;
        chargedHadronInAcc++;
        if (chargedHadronInAcc>2) hasHadronsInAcc=1;
        if (hasHadronsInAcc && (hasMuonAndHadrons || hasMuon) ){
          if(msgLevel(MSG::DEBUG)) debug() << "   Daughter has hadrons that pass all cuts. \n"<< endmsg ;
          hasHadrons=1;
          return 1;
        }
    } // end of hadron search.

  } // end of loop over Xc descendants.

  if (!hasMuonInAcc && !hasHadronsInAcc){
    if(msgLevel(MSG::DEBUG)) debug() << "   Daughter has neither muon or hadrons. \n"<< endmsg ;
    return 0;
  }
  else if (hasMuonInAcc && !hasHadronsInAcc){
    hasMuon=1;
    if(msgLevel(MSG::DEBUG)) debug() << "   Daughter has a muon but not hadrons. \n"<< endmsg ;
    return 1;
  }
  else if (!hasMuonInAcc && hasHadronsInAcc){
    hasHadrons=1;
    if(msgLevel(MSG::DEBUG)) debug() << "   Daughter has hadrons but not muons. \n"<< endmsg ;
    return 1;
  }
  else if (hasMuonInAcc && hasHadronsInAcc){
    hasMuonAndHadrons=1;
    if(msgLevel(MSG::DEBUG)) debug() << "   Daughter has both muon and hadrons. \n"<< endmsg ;
    return 1;
  }
  // Shouldn't reach here!
  else return 0;
  
} //end of passCuts.
