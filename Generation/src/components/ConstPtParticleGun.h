#ifndef GENERATION_CONSTPTPARTICLEGUN_H
#define GENERATION_CONSTPTPARTICLEGUN_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Generation/IParticleGunTool.h"

/** @class ConstPtParticleGun 
 *
 *  Particle gun, that, given a list of pt's  and an eta range, creates the desired four-momenta.
 *  To be more flexible, the gun uses only pt and eta values from a list, if given,
 *  and only if the lists are empty draws the values from a distribution between min and max.
 */
class ConstPtParticleGun : public GaudiTool, virtual public IParticleGunTool {
public:
  ConstPtParticleGun(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ConstPtParticleGun();
  virtual StatusCode initialize();
  /// Generation of particles
  virtual void generateParticle(Gaudi::LorentzVector& momentum, Gaudi::LorentzVector& origin, int& pdgId);
  virtual void printCounters() { ; };
  virtual StatusCode getNextEvent(HepMC::GenEvent&);

private:
  Gaudi::Property<std::vector<double>> m_ptList{ 
    this, "PtList", {}, "List of transverse momenta to generate. If empty, use flat distribution between PtMin and PtMax"};
  Gaudi::Property<std::vector<double>> m_etaList{
      this, "EtaList", {}, "List of particle pseudorapidities to generate. If empty, use flat distribution between EtaMin and EtaMax"};
  Gaudi::Property<double> m_minPt{this, "PtMin", 1 * Gaudi::Units::GeV, "Lower limit for the flat transverse momenta distribution of generated particles."};
  Gaudi::Property<double> m_maxPt{this, "PtMax", 100. * Gaudi::Units::GeV, "Upper limit for the flat transverse momenta distribution of generated particles"};
  Gaudi::Property<bool> m_logSpacedPt{this, "logSpacedPt", false, "Generate a log-spaced distribution (flat in  log(Pt))"};
  Gaudi::Property<double> m_minEta{this, "EtaMin", -3.5, "Lower limit for the flat pseudorapidity distribution of generated particles."};
  Gaudi::Property<double> m_maxEta{this, "EtaMax", 3.5, "Upper limit for the flat pseudorapidity distribution of generated particles."};
  Gaudi::Property<double> m_minPhi{this, "PhiMin", 0. * Gaudi::Units::rad, "Lower limit for the flat azimuth distribution of generated particles"};
  Gaudi::Property<double> m_maxPhi{this, "PhiMax", Gaudi::Units::twopi* Gaudi::Units::rad, "Upper limit for the azimuth distribution of generated particles"};
  Gaudi::Property<std::vector<int>> m_pdgCodes{this, "PdgCodes", {-211}, "List of PDG codes to produce."};
  Gaudi::Property<bool> m_writeParticleGunBranches{this, "writeParticleGunBranches", {true}, "Write additional Branches with Particle Properties (eta, pt)"};
  /// optional additional branches (switched on by  m_writeParticleGunBranches)
  std::unique_ptr<DataHandle<float>> m_datahandle_particlegun_pt;
  std::unique_ptr<DataHandle<float>> m_datahandle_particlegun_eta;
  std::unique_ptr<DataHandle<float>> m_datahandle_particlegun_costheta;
  std::unique_ptr<DataHandle<float>> m_datahandle_particlegun_phi;

  /// helper variables
  double m_deltaEta;
  double m_deltaPhi;
  /// Masses of particles to generate (derived from PDG codes)
  std::vector<double> m_masses;
  /// Names of particles to generate (derived from PDG codes)
  std::vector<std::string> m_names;
  /// Flat random number generator
  Rndm::Numbers m_flatGenerator;

};

#endif  // GENERATION_CONSTPTPARTICLEGUN_H
