// $Id: PythiaLSP.cpp,v 1.3 2009-10-22 17:08:20 robbep Exp $
// Include files 

// local
#include "PythiaLSP.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;

//-----------------------------------------------------------------------------
// Implementation file for class : PythiaLSP
//
// 2008-September-1 : Neal Gauvin (Gueissaz)
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( PythiaLSP )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PythiaLSP::PythiaLSP( const std::string & type , 
                                      const std::string & name ,
                                      const IInterface * parent )
  : GaudiTool ( type, name , parent )
{
    declareInterface< IGenCutTool >( this );
    declareProperty( "LSPID" , m_LSPID );
    declareProperty( "NbLSP" , m_NbLSP = 1 );
    declareProperty( "AtLeast" , m_AtLeast = true );
    declareProperty( "LSPCond" , m_LSPCond = 1 );
    declareProperty( "DgtsInAcc" , m_Dgts );
    declareProperty( "EtaMin" , m_EtaMin = 1.8 );
    declareProperty( "EtaMax" , m_EtaMax = 4.9 );
    declareProperty( "DistToPVMin" , m_DistToPVMin = -1. );
    declareProperty( "DistToPVMax" , m_DistToPVMax = -1.0 );
    declareProperty( "ZPosMin" , m_ZPosMin = -500.0*mm );
    declareProperty( "ZPosMax" , m_ZPosMax = 1.0*km );
    if( m_LSPID.size() == 0 ){ m_LSPID.push_back( 1000022); }
    
}
//=============================================================================
// Destructor
//=============================================================================
PythiaLSP::~PythiaLSP() { ; } 

//=============================================================================
// Accept function
//=============================================================================
bool PythiaLSP::applyCut( ParticleVector & /* theParticleVector */ ,
                            const HepMC::GenEvent * theEvent ,
                            const LHCb::GenCollision * /* theCollision */ ) 
  const {

  //theEvent->print( std::cout );
  if ( m_NbLSP <= 0 || m_LSPCond <= 0 ) return true;

  vector< HepMC::GenParticle* > LSP;
  HepMC::GenVertex* PV = 0;

  //Find PV
  HepMC::GenEvent::particle_const_iterator p= theEvent->particles_begin();
  PV = (*p)->end_vertex();
  debug()<<"--------------------------------------------------" << endmsg;
  debug()<<"Primary Vertex " << Print(PV->position()) << endmsg;

  //Find the interesting particles
  for( p= theEvent->particles_begin(); p!= theEvent->particles_end();++p){
    verbose()<<"Particle " << (*p)->pdg_id() <<" "<< (*p)->status() << endmsg;
    if( IsLSP( *p ) ){

      //The LSP must not decay into itself (W decays)
      if( (*p)->end_vertex() != NULL ){
	HepMC::GenVertex* vtx = (*p)->end_vertex();
	HepMC::GenVertex::particles_out_const_iterator dp = 
	  vtx->particles_out_const_begin();
	if( IsLSP( *dp ) ) continue;
      }
      LSP.push_back( (*p) );
    }
  }

  debug()<<"Found "<< LSP.size() <<" LSP's ";
  if( LSP.size() < ( (unsigned int) abs(m_NbLSP) ) ){
    debug() << ": Not enough LSP in event !" << endmsg;
    return false;
  }
  debug()<< "!" << endmsg;

  //Does the LSP's satisfy criteria ?
  int nbok = 0, nbinacc = 0;
  for( vector< HepMC::GenParticle* >::iterator i = LSP.begin(); 
       i < LSP.end(); ++i ){

    debug()<<"LSP mass [MeV] : "<< (*i)->momentum().m()
	   <<", eta : "<< (*i)->momentum().eta() << ", Decay Vertices " 
	   << Print((*i)->end_vertex()->position()) << endmsg;

    if( m_LSPCond == 1 ){
      double eta = (*i)->momentum().eta();
      if( eta < m_EtaMin || eta > m_EtaMax ) continue;
    }

    HepMC::GenVertex* vtx = (*i)->end_vertex();
    if( m_LSPCond > 1 ){
      //Loop on daughters
      bool ok = true;
      HepMC::GenVertex::particles_out_const_iterator dp;
      for( dp = vtx->particles_out_const_begin(); 
	   dp !=  vtx->particles_out_const_end(); ++dp){
	double eta = (*dp)->momentum().eta();

	debug() << "Daughter id " << (*dp)->pdg_id()<< endmsg;
	if( !IsDgts( *dp ) ) continue;
	debug() << "Daughter id " << (*dp)->pdg_id() 
		<< " eta " << eta << endmsg;

	if ( eta < m_EtaMin || eta > m_EtaMax ){
	  ok = false; break;
	}
      }
      if( !ok ) continue;

    }
    nbinacc++;

    //Check z position of LSP
    double z = (*i)->end_vertex()->position().z();
    if( z < m_ZPosMin || z > m_ZPosMax ) continue;

    //Compute distance of flight
    double flight = Dist( PV, vtx );
    debug() <<"Distance of flight : " << flight << endmsg;

    //Keep on events with LSP decaying within a certain range
    if( m_DistToPVMin > 0. || m_DistToPVMax > 0. ){
      if( flight < m_DistToPVMin || flight > m_DistToPVMax ) continue;
    }

    debug() << "LSP satisfies criterias !" << endmsg;
    nbok++;
  }

  debug() <<"Number of LSP satisfying the cuts "<< nbok;

  //If AtLeast is false, the LSP that didn't satisfied the cut must be
  //out of the acceptance
  if( !m_AtLeast ){
    if( nbok == m_NbLSP && nbok == nbinacc ){
      debug() << " : Event accepted !"<< endmsg;
      return true;
    }
  } else {
    if( nbok >= m_NbLSP && m_AtLeast ){ 
      debug() << " : Event accepted !"<< endmsg;
      return true;
    } 
    debug() << " : Event rejected !"<< endmsg;
    return false; 
  }
  
  return false;
}

//=============================================================================
//  IsQuark : Is particle a quark ?
//=============================================================================
bool PythiaLSP::IsQuark( HepMC::GenParticle * p ) const {

  if( abs(p->pdg_id())== 1 || abs(p->pdg_id())== 2 || 
      abs(p->pdg_id())== 3 || abs(p->pdg_id())== 4 || 
      abs(p->pdg_id())== 5 || abs(p->pdg_id())== 6 ){
    return true;
  } else return false;
}

//=============================================================================
//  IsLepton : Is particle a lepton ?
//=============================================================================
bool PythiaLSP::IsLepton( HepMC::GenParticle * p ) const {

  if( abs(p->pdg_id())== 11 || 
      //abs(p->pdg_id())== 12 || 
      abs(p->pdg_id())== 13 || 
      //abs(p->pdg_id())== 14 ||
      abs(p->pdg_id())== 15 ){ 
      //abs(p->pdg_id())== 16 

    return true;
  } else return false;
}


//=============================================================================
//  IsLSP : Is particle one of the LSP ?
//             In some theoretical, there are more than one LSP ! Sepecially
//             when particles have very close masses, thus cannot decay into 
//             each other !
//=============================================================================

bool PythiaLSP::IsLSP( HepMC::GenParticle * p ) const {
  int pid = abs(p->pdg_id());
  for( vector<int>::const_iterator i = m_LSPID.begin(); i != m_LSPID.end(); 
       ++i ){
    if( pid == (*i) ){
      // t always has status 3
      if( pid == 6 && p->status() == 3 ) return true;
      if( p->status() == 2 ) return true;
    }
  }
  return false;
}

//=============================================================================
//  IsDgts : Is particle one of the daughters of the LSP that we'd like 
//           to have in acceptance ?
//=============================================================================

bool PythiaLSP::IsDgts( HepMC::GenParticle * p ) const {

  if( m_LSPCond == 3 ) return true;

  int pid = abs(p->pdg_id());
  for( vector<int>::const_iterator i = m_Dgts.begin(); i != m_Dgts.end(); 
       ++i ){
    if( pid == (*i) ) return true;
  }
  return false;
}


//=============================================================================
//  Compute distance between 2 particles/vertices
//=============================================================================
double PythiaLSP::Dist( HepMC::GenVertex* v1, HepMC::GenVertex * v2 ) const {

    double dx= v2->point3d().x()- v1->point3d().x();
    double dy= v2->point3d().y()- v1->point3d().y();
    double dz= v2->point3d().z()- v1->point3d().z();

    return sqrt( dx*dx + dy*dy + dz*dz );
}

//=============================================================================
// Return decay vertices as a string
//=============================================================================
string PythiaLSP::Print( HepMC::ThreeVector v ) const {
  stringstream kss;
  kss<<" ( "<< v.x() <<", "<< v.y() <<", "<< v.z() <<" ) ";
  return kss.str();
}

string PythiaLSP::Print( HepMC::FourVector v ) const {
  stringstream kss;
  kss<<" ( "<< v.x() <<", "<< v.y() <<", "<< v.z() <<", "<< v.t() <<" ) ";
  return kss.str();
}

//=============================================================================

//=============================================================================

