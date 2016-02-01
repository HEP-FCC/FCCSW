#ifndef GENERATION_MOMENTUMRANGEPARTICLEGUN_H
#define GENERATION_MOMENTUMRANGEPARTICLEGUN_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "IParticleGunTool.h"

/** @class MomentumRangeParticleGun MomentumRangeParticleGun.h "MomentumRangeParticleGun.h"
 *
 *  Particle gun with given momentum range
 *  
 *  @author Patrick Robbe
 *  @author Benedikt Hegner
 *  @date   2008-05-18
 */
class MomentumRangeParticleGun : public GaudiTool , virtual public IParticleGunTool {

public:
  /// Constructor
  MomentumRangeParticleGun( const std::string & type , const std::string& name, 
                 const IInterface * parent ) ;
  
  /// Destructor
  virtual ~MomentumRangeParticleGun();
  
  /// Initialize particle gun parameters
  virtual StatusCode initialize();
  
  /// Generation of particles
  virtual void generateParticle( Gaudi::LorentzVector & momentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId ) ;
  
  /// Print counters
  virtual void printCounters( ) { ; } ;

private:  
  double m_minMom;   ///< Minimum momentum (Set by options)
  double m_minTheta; ///< Minimum theta angle (Set by options)
  double m_minPhi;   ///< Minimum phi angle (Set by options)
  
  double m_maxMom;   ///< Maximum momentum (Set by options)
  double m_maxTheta; ///< Maximum theta angle (Set by options)
  double m_maxPhi;   ///< Maximum phi angle (Set by options)
  
  double m_deltaMom; ///< Momentum range
  double m_deltaTheta; ///< Theta range
  double m_deltaPhi; ///< Phi range
  
  /// Pdg Codes of particles to generate (Set by options)
  std::vector<int>         m_pdgCodes;
  
  /// Masses of particles to generate
  std::vector<double>      m_masses;
  
  /// Names of particles to generate
  std::vector<std::string> m_names;
  
  /// Flat random number generator
  Rndm::Numbers m_flatGenerator ;
};

#endif // GENERATION_MOMENTUMRANGEPARTICLEGUN_H
