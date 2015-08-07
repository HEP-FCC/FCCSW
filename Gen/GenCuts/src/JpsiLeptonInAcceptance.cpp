// $id: JpsiLeptonInAcceptance.cpp,v 0.1 2013-10-23 18:24:00 jwimberl Exp $
// Include files 

// local
#include "JpsiLeptonInAcceptance.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/SystemOfUnits.h"

//#include "GaudiKernel/ToolFactory.h"
#include "Event/HepMCEvent.h"

#include <cmath>

//-----------------------------------------------------------------------------
// Implementation file for class : JpsiLeptonInAcceptance
//
// 2007-01-30 : Thomas Blake
//-----------------------------------------------------------------------------



DECLARE_TOOL_FACTORY(JpsiLeptonInAcceptance)


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
JpsiLeptonInAcceptance::JpsiLeptonInAcceptance(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent)
  : GaudiTool (type, name , parent)
{
  declareInterface<IFullGenEventCutTool>(this);

  // Default type: J/psi -> dimuon + muon
  declareProperty("JpsiID"    , m_JpsiPDG = 443); // let it be a psi(2S), possibly
  declareProperty("JpsiLepID" , m_JpsiLepPDG = 13);
  declareProperty("BachLepID" , m_BachLepPDG = 13);

  // Default PT cuts: No cuts
  declareProperty("JpsiPtMin", m_JpsiPtMin = 0*Gaudi::Units::GeV);
  declareProperty("BachLepPtMin", m_BachLepPtMin = 0*Gaudi::Units::MeV);
  declareProperty("JpsiLepPtMin", m_JpsiLepPtMin = 0*Gaudi::Units::MeV);

  // Default P cuts: Accept only potential long tracks
  declareProperty("JpsiPMin", m_JpsiPMin = 0*Gaudi::Units::GeV);
  declareProperty("BachLepPMin", m_BachLepPMin = 2.9*Gaudi::Units::GeV);
  declareProperty("JpsiLepPMin", m_JpsiLepPMin = 2.9*Gaudi::Units::GeV);

  // Default angular cuts: normal cuts
  declareProperty("LepMinTheta"  , m_thetaMin = 10*Gaudi::Units::mrad);
  declareProperty("LepMaxTheta"  , m_thetaMax = 400*Gaudi::Units::mrad);
  declareProperty("LepMaxHorzTheta"  , m_thetaXZMax = 500*Gaudi::Units::mrad);
  declareProperty("LepMaxVertTheta"  , m_thetaYZMax = 500*Gaudi::Units::mrad);

  // Default MM cuts: between J/psi and Bc mass
  declareProperty("PreselMass"  , m_PreselMass = true);
  declareProperty("MinMass"   , m_minMass = 3*Gaudi::Units::GeV);
  declareProperty("MaxMass"   , m_maxMass = 6.5*Gaudi::Units::GeV);

  // Default DOCA cuts: 1 cm
  declareProperty("PreselDoca"   , m_PreselDoca = true);
  declareProperty("DocaCut"      , m_docaCut = 10.0*Gaudi::Units::mm);

}

//=============================================================================
// Destructor
//=============================================================================

JpsiLeptonInAcceptance::~JpsiLeptonInAcceptance() {} 


//=============================================================================
// Selection function
//=============================================================================
bool JpsiLeptonInAcceptance::studyFullEvent(LHCb::HepMCEvents * theEvents ,
                                            LHCb::GenCollisions * /* col */)
  const {
  
  std::list< HepMC::GenParticle* > JpsiList;
  std::list< HepMC::GenParticle* > BachLepList;
  
  for (LHCb::HepMCEvents::iterator theEventIter = theEvents->begin();
       theEventIter != theEvents->end();
       ++theEventIter) {
    
    for (HepMC::GenEvent::particle_const_iterator iParticle = (*theEventIter)->pGenEvt()->particles_begin();
         iParticle != (*theEventIter)->pGenEvt()->particles_end(); 
         ++iParticle){
      
      HepMC::GenParticle* particle = (*iParticle);
      
      int pdg_id = abs(particle->pdg_id());
  
      if (m_JpsiPDG == pdg_id) {
        debug() << "Examining J/psi candidate ..." << endmsg;
        
        // Check that it is in acceptance
        if (isInAcceptance(particle,m_JpsiPMin,m_JpsiPtMin,false)) {
          debug() << "--> J/psi candidate is in acceptance!" << endmsg;
          
          // Check that its mu daughters are in acceptance
          bool allgood = true;
          HepMC::GenVertex* vtx = particle->end_vertex();
          for (HepMC::GenVertex::particles_in_const_iterator iDaughter = vtx->particles_out_const_begin();
               iDaughter != vtx->particles_out_const_end();
               ++iDaughter) {
            int absid = abs((*iDaughter)->pdg_id());
            if (absid == 22) {
              continue; // ignore photons from PHOTOS
            } else if (absid == m_JpsiLepPDG) {
              debug() << "--> Examining Lep daughter ..." << endmsg;
              if (isInAcceptance(*iDaughter,m_JpsiLepPMin,m_JpsiLepPtMin,true)) {
                debug() << "--> --> Lep daughter in acceptance!" << endmsg;
                continue;
              } else {
                debug() << "--> --> Lep daughter out of acceptance!" << endmsg;
                allgood = false;
                break;
              }
            } else {
              debug() << "--> --> Wrong daughter!" << endmsg;
              allgood = false;
              break;
            }
          }
          if (allgood) {
            debug() << "--> J/psi selected!" << endmsg;
            JpsiList.push_back(particle);
          }
        } else {
          debug() << "--> J/psi candidate is out of acceptance!" << endmsg;
        }
      }
      
      else if (m_BachLepPDG == pdg_id) {
        debug() << "Examining BachLep candidate ..." << endmsg;
        if (isInAcceptance(particle,m_BachLepPMin,m_BachLepPtMin,true)) {
          debug() << "--> BachLep candidate in acceptance!" << endmsg;
          debug() << "--> BachLep selected!" << endmsg;
          BachLepList.push_back(particle);
        } else {
          debug() << "--> BachLep candidate out of acceptance!" << endmsg;
        }
      }
    }
  }
  
  if (JpsiList.empty() || BachLepList.empty()) {
    debug() << "=> No candidate found in this event" << endmsg;
    return false;
  }
  
  // apply dimuon mass constraint
  double trileptonMass;
  
  double doca, doca_P, doca_M;
  double modpd, modpd_P, modpd_M;
  double pd[3], pd_P[3], pd_M[3];
  double x[3], x_P[3], x_M[3];
  double Jpsi_px, Jpsi_py, Jpsi_pz; 
  double MuP_px, MuP_py, MuP_pz; 
  double MuM_px, MuM_py, MuM_pz; 
  double BachLep_px, BachLep_py, BachLep_pz; 
  
  std::list< HepMC::GenParticle * >::const_iterator iterJpsi;
  std::list< HepMC::GenParticle * >::const_iterator iterLep;
  
  for (iterJpsi = JpsiList.begin(); 
       iterJpsi != JpsiList.end();
       ++iterJpsi) {      
    
    for (iterLep = BachLepList.begin(); 
         iterLep != BachLepList.end();
         ++iterLep){
      
      // check that the lepton isn't the daughter of a J/psi
      bool daughter = false;
      HepMC::GenParticle* MuP = NULL;
      HepMC::GenParticle* MuM = NULL;
      HepMC::GenVertex* vtx = (*iterJpsi)->end_vertex();
      for (HepMC::GenVertex::particles_in_const_iterator iDaughter = vtx->particles_out_const_begin();
           iDaughter != vtx->particles_out_const_end();
           ++iDaughter) {
        if ((*iterLep) == (*iDaughter)) daughter = true;
        int absid = abs((*iDaughter)->pdg_id());
        if (absid == m_JpsiLepPDG) {
          if (MuP == NULL)
            MuP = *iDaughter;
          else if (MuM == NULL)
            MuM = *iDaughter;
          else
            err() << "ERROR: TOO MANY LEPTONS" << endmsg;;
        }
      }
      if (daughter) continue;
      if (MuP == NULL or MuM == NULL) {
        err() << "ERROR: NOT ENOUGH LEPTONS" << endmsg;;
        continue;
      }
      
      debug() << "Examining J/psi / BachLep pair ..." << endmsg;
    
      // Apply Mass Cut
      HepMC::FourVector sum ;
      sum.setPx((*iterJpsi)->momentum().px() + (*iterLep)->momentum().px());
      sum.setPy((*iterJpsi)->momentum().py() + (*iterLep)->momentum().py());
      sum.setPz((*iterJpsi)->momentum().pz() + (*iterLep)->momentum().pz());
      sum.setE ((*iterJpsi)->momentum().e()  + (*iterLep)->momentum().e());
      trileptonMass = sum.m();
      debug() << "--> Pair mass = " << trileptonMass << endmsg;

      if (trileptonMass >= m_minMass && trileptonMass <= m_maxMass){
        debug() << "--> Pair passes mass cut!" << endmsg;
        if (!m_PreselDoca) {
          debug() << "--> Pair selected!" << endmsg;
          return true;
        }
      } else { 
        debug() << "--> Pair fails mass cut!" << endmsg;
        continue;
      }
      
      // Apply DOCA Cut
      if (m_PreselDoca) {

        Jpsi_px = (*iterJpsi)->momentum().px();
        Jpsi_py = (*iterJpsi)->momentum().py();
        Jpsi_pz = (*iterJpsi)->momentum().pz();

        MuP_px = MuP->momentum().px();
        MuP_py = MuP->momentum().py();
        MuP_pz = MuP->momentum().pz();

        MuM_px = MuM->momentum().px();
        MuM_py = MuM->momentum().py();
        MuM_pz = MuM->momentum().pz();
        
        BachLep_px = (*iterLep)->momentum().px();
        BachLep_py = (*iterLep)->momentum().py();
        BachLep_pz = (*iterLep)->momentum().pz();

        const HepMC::GenVertex * JpsiVert = (*iterJpsi)->production_vertex();
        const HepMC::GenVertex * MuPVert = MuP->production_vertex();
        const HepMC::GenVertex * MuMVert = MuM->production_vertex();
        const HepMC::GenVertex * BachLepVert = (*iterLep)->production_vertex();
        HepMC::ThreeVector pos_Jpsi = JpsiVert->point3d();
        HepMC::ThreeVector pos_MuP = MuPVert->point3d();
        HepMC::ThreeVector pos_MuM = MuMVert->point3d();
        HepMC::ThreeVector pos_BachLep = BachLepVert->point3d();

        // Jpsi / BachLep DOCA
        pd[0] = Jpsi_py*BachLep_pz - Jpsi_pz*BachLep_py;
        pd[1] = Jpsi_pz*BachLep_px - Jpsi_px*BachLep_pz;
        pd[2] = Jpsi_px*BachLep_py - Jpsi_py*BachLep_px;
        modpd = sqrt(pd[0]*pd[0]+pd[1]*pd[1]+pd[2]*pd[2]);
        if (modpd == 0) {
          doca = -1;
        } else {
          pd[0] = pd[0]/modpd;
          pd[1] = pd[1]/modpd;
          pd[2] = pd[2]/modpd;
          x[0] = pos_Jpsi.x()-pos_BachLep.x();
          x[1] = pos_Jpsi.y()-pos_BachLep.y();
          x[2] = pos_Jpsi.z()-pos_BachLep.z(); 
          doca = fabs(x[0]*pd[0]+x[1]*pd[1]+x[2]*pd[2]);
        }
        debug() << "--> J/psi BachLep DOCA = " << doca << endmsg;

        // MuP / BachLep DOCA
        pd_P[0] = MuP_py*BachLep_pz - MuP_pz*BachLep_py;
        pd_P[1] = MuP_pz*BachLep_px - MuP_px*BachLep_pz;
        pd_P[2] = MuP_px*BachLep_py - MuP_py*BachLep_px;
        modpd_P = sqrt(pd_P[0]*pd_P[0]+pd_P[1]*pd_P[1]+pd_P[2]*pd_P[2]);
        if (modpd_P == 0) {
          doca_P = -1;
        } else {
          pd_P[0] = pd_P[0]/modpd_P;
          pd_P[1] = pd_P[1]/modpd_P;
          pd_P[2] = pd_P[2]/modpd_P;
          x_P[0] = pos_MuP.x()-pos_BachLep.x();
          x_P[1] = pos_MuP.y()-pos_BachLep.y();
          x_P[2] = pos_MuP.z()-pos_BachLep.z(); 
          doca_P = fabs(x_P[0]*pd_P[0]+x_P[1]*pd_P[1]+x_P[2]*pd_P[2]);
        }
        debug() << "--> MuP BachLep DOCA = " << doca_P << endmsg;

        // MuM / BachLep DOCA
        pd_M[0] = MuM_py*BachLep_pz - MuM_pz*BachLep_py;
        pd_M[1] = MuM_pz*BachLep_px - MuM_px*BachLep_pz;
        pd_M[2] = MuM_px*BachLep_py - MuM_py*BachLep_px;
        modpd_M = sqrt(pd_M[0]*pd_M[0]+pd_M[1]*pd_M[1]+pd_M[2]*pd_M[2]);
        if (modpd_M == 0) {
          doca_M = -1;
        } else {
          pd_M[0] = pd_M[0]/modpd_M;
          pd_M[1] = pd_M[1]/modpd_M;
          pd_M[2] = pd_M[2]/modpd_M;
          x_M[0] = pos_MuM.x()-pos_BachLep.x();
          x_M[1] = pos_MuM.y()-pos_BachLep.y();
          x_M[2] = pos_MuM.z()-pos_BachLep.z(); 
          doca_M = fabs(x_M[0]*pd_M[0]+x_M[1]*pd_M[1]+x_M[2]*pd_M[2]);
        }
        debug() << "--> MuM BachLep DOCA = " << doca_M << endmsg;

        // Do all three pass?
        if (doca >= 0 && doca_P >= 0 && doca_M >= 0 && doca <= m_docaCut && doca_P <= m_docaCut && doca_M <= m_docaCut) {
          debug() << "--> Pair passes DOCA cut!" << endmsg;
          return true;
        } else {
          debug() << "--> Pair fails DOCA cut!" << endmsg;          
          continue;
        }
      } else { 
        continue;
      }
    }
  }

  return false;
}
  
bool JpsiLeptonInAcceptance::isInAcceptance(const HepMC::GenParticle *p, double pmin, double ptmin, bool doAngle) const {

  double px, py, pz, pp, theta, thetaXZ, thetaYZ, pt;

  pz = p->momentum().pz();
  
  //   if (pz < 0) {
  //     debug() << "Backwards" << endmsg;
  //     return false;
  //   }
  
  px = p->momentum().px();
  py = p->momentum().py();
  
  pp = sqrt(px*px + py*py + pz*pz);
  if (pp < pmin) {
    debug() << "P = " << pp << " < PMIN = " << pmin << endmsg;
    return false;
  }
  
  
  pt = sqrt(px*px + py*py);
  if (pt < ptmin) {
    debug() << "PT = " << pt << " < PTMIN = " << ptmin << endmsg;
    return false;
  }

  if (not doAngle) return true;
 
  theta = acos(pz / pp);
  thetaXZ = std::abs(atan(px / pz));
  thetaYZ = std::abs(atan(py / pz));
  debug() << "ANGLES: " << theta << "--" << thetaXZ << "--" << thetaYZ << endmsg;
  
  if (theta > m_thetaMax) {
    debug() << "THETA = " << theta << " < THETAMAX = " << m_thetaMax << endmsg;
    return false;
  }
  if (theta < m_thetaMin) {
    debug() << "THETA = " << theta << " < THETAMIN = " << m_thetaMin << endmsg;
    return false;
  }
  if (thetaXZ > m_thetaXZMax) {
    debug() << "THETAXZ = " << thetaXZ << " < THETAXZMAX = " << m_thetaXZMax << endmsg;
    return false;
  }
  if (thetaYZ > m_thetaYZMax) {
    debug() << "THETAYZ = " << thetaYZ << " < THETAYZMAX = " << m_thetaYZMax << endmsg;
    return false;
  }
  
  return true;
}
 
  //=============================================================================
