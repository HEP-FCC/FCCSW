// $Id: Cosmics.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_COSMICS_H
#define PARTICLEGUNS_COSMICS_H 1

// Include Files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

// from ParticleGuns
#include "LbPGuns/IParticleGunTool.h"

/** @class Cosmics Cosmics.h "Cosmics.h"
 *
 *  Tool to generate cosmic particles inside LHCb detector
 *
 *  @author Giulia Manca
 *  @date   2008-05-18
 */

class Cosmics : public GaudiTool , virtual public IParticleGunTool {
 public:
 
  /// Constructor
  Cosmics(const std::string& type, const std::string& name , 
          const IInterface * parent ) ;
  
  /// Destructor
  virtual ~Cosmics();
  
  /// Initialize particle gun parameters
  virtual StatusCode initialize();

  /// Generation of particles
  virtual void generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                 Gaudi::LorentzVector & origin , int & pdgId ) ;


  /// Print counters
  virtual void printCounters( ) { ; } ;
    
 private:

  /// Flux function 
  double muonSpec(double muMomentum, double Theta, double vDepth, 
                              double overAngle);
                                
  /// Setable Properties:-
  
  /// Minimum momentum of the mu at detection
  double m_minMom;
  
  /// Minimum zenith angle of the cosmic ray
  double m_minTheta;

  /// Minimum phi angle
  double m_minPhi;

  /// Maximum momentum of the mu at detection
  double m_maxMom;

  /// Maximum zenith Angle of the muon
  double m_maxTheta;

  /// Maximum phi angle
  double m_maxPhi;
  
  /// Minimum x position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_minxvtx;

  /// Maximum x position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_maxxvtx;

  /// Minimum y position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_minyvtx;


  /// Maximum y position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_maxyvtx;

  /// z position of the point of interaction of the muon
  /// with the detector; the vertex position is then extrapolated
  double m_zvtx;

  ///Time I want the mu to interact; then the real
  ///time of the vertex is extrapolated
  double m_time;

  /// Theory model 
  int m_model;  

  /// Pdg Codes of particles to generate
  std::vector<int>         m_pdgCodes;

  /// Masses of particles to generate
  std::vector<double>      m_masses;

  /// Names of particles to generate
  std::vector<std::string> m_names;

  /// Flat random number generator
  Rndm::Numbers m_flatGenerator ;
};

#endif // PARTICLEGUNS_COSMICS_H
