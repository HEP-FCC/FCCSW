#ifndef GENERATION_MOMENTUMRANGEPARTICLEGUN_H
#define GENERATION_MOMENTUMRANGEPARTICLEGUN_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "Generation/IParticleGunTool.h"

/** @class MomentumRangeParticleGun MomentumRangeParticleGun.h "MomentumRangeParticleGun.h"
 *
 *  Particle gun with given momentum range
 *
 *  @author Patrick Robbe
 *  @author Benedikt Hegner
 *  @date   2008-05-18
 */
class MomentumRangeParticleGun : public GaudiTool, virtual public IParticleGunTool {

public:
  /// Constructor
  MomentumRangeParticleGun(const std::string& type, const std::string& name, const IInterface* parent);

  /// Destructor
  virtual ~MomentumRangeParticleGun();

  /// Initialize particle gun parameters
  virtual StatusCode initialize();

  /// Generation of particles
  virtual void generateParticle(Gaudi::LorentzVector& momentum, Gaudi::LorentzVector& origin, int& pdgId);

  /// Print counters
  virtual void printCounters() { ; };

private:
  /// Minimum momentum (Set by options)
  Gaudi::Property<double> m_minMom{this, "MomentumMin", 100.0 * Gaudi::Units::GeV, "Minimal momentum"};
  /// Minimum theta angle (Set by options)
  Gaudi::Property<double> m_minTheta{this, "ThetaMin", 0.1 * Gaudi::Units::rad, "Minimal theta"};
  /// Minimum phi angle (Set by options)
  Gaudi::Property<double> m_minPhi{this, "PhiMin", 0. * Gaudi::Units::rad, "Minimal phi"};

  /// Maximum momentum (Set by options)
  Gaudi::Property<double> m_maxMom{this, "MomentumMax", 100.0 * Gaudi::Units::GeV, "Maximal momentum"};
  /// Maximum theta angle (Set by options)
  Gaudi::Property<double> m_maxTheta{this, "ThetaMax", 0.4 * Gaudi::Units::rad, "Maximal theta"};
  /// Maximum phi angle (Set by options)
  Gaudi::Property<double> m_maxPhi{this, "PhiMax", Gaudi::Units::twopi* Gaudi::Units::rad, "Maximal phi"};

  /// Momentum range
  double m_deltaMom;
  /// Theta range
  double m_deltaTheta;
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

#endif  // GENERATION_MOMENTUMRANGEPARTICLEGUN_H
