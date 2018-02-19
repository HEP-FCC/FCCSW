#ifndef GENERATION_CONSTPTPARTICLEGUN_H
#define GENERATION_CONSTPTPARTICLEGUN_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "Generation/IParticleGunTool.h"

/** @class ConstPtParticleGun ConstPtParticleGun.h "ConstPtParticleGun.h"
 *
 *  Particle gun, that, given a list of pt's  and an eta range, creates the desired four-momenta.
 */
class ConstPtParticleGun : public GaudiTool, virtual public IParticleGunTool {

public:
  /// Constructor
  ConstPtParticleGun(const std::string& type, const std::string& name, const IInterface* parent);

  /// Destructor
  virtual ~ConstPtParticleGun();

  /// Initialize particle gun parameters
  virtual StatusCode initialize();

  /// Generation of particles
  virtual void generateParticle(Gaudi::LorentzVector& momentum, Gaudi::LorentzVector& origin, int& pdgId);

  /// Print counters
  virtual void printCounters() { ; };
  virtual StatusCode getNextEvent(HepMC::GenEvent&);

private:
  /// Minimum momentum (Set by options)
  Gaudi::Property<std::vector<double>> m_ptList{
      this, "PtList", {100.0 * Gaudi::Units::GeV}, "List of transverse momenta to generate"};
  /// Minimum theta angle (Set by options)
  Gaudi::Property<double> m_minEta{this, "EtaMin", -3.5, "Minimal eta"};
  /// Minimum phi angle (Set by options)
  Gaudi::Property<double> m_minPhi{this, "PhiMin", 0. * Gaudi::Units::rad, "Minimal phi"};

  /// Maximum theta angle (Set by options)
  Gaudi::Property<double> m_maxEta{this, "EtaMax", 3.5, "Maximal eta"};
  /// Maximum phi angle (Set by options)
  Gaudi::Property<double> m_maxPhi{this, "PhiMax", Gaudi::Units::twopi* Gaudi::Units::rad, "Maximal phi"};

  /// Eta range
  double m_deltaEta;
  /// Phi range
  double m_deltaPhi;

  /// Pdg Codes of particles to generate (Set by options)
  Gaudi::Property<std::vector<int>> m_pdgCodes{this, "PdgCodes", {-211}, "list of pdg codes to produce"};

  /// Masses of particles to generate (derived from PDG codes)
  std::vector<double> m_masses;

  /// Names of particles to generate (derived from PDG codes)
  std::vector<std::string> m_names;

  /// Flat random number generator
  Rndm::Numbers m_flatGenerator;
};

#endif  // GENERATION_CONSTPTPARTICLEGUN_H
