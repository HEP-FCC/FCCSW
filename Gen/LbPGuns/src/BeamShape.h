#ifndef PARTICLEGUNS_BEAMSHAPE_H
#define PARTICLEGUNS_BEAMSHAPE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

// from ParticleGuns
#include "LbPGuns/IParticleGunTool.h"

/** @class BeamShape BeamShape.h "BeamShape.h"
 *
 *  @author M. Lieng
 *  @date 2009-10-23
 */
class BeamShape : public GaudiTool , virtual public IParticleGunTool {
 public:
  
  /// Constructor
  BeamShape( const std::string & type , const std::string & name , 
              const IInterface * parent ) ;
  
  virtual ~BeamShape() ; ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize();
  
  /// Generate the particle
  virtual void generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId ) ;
                                 
  /// Print counters
  virtual void printCounters( ) { ; } ;
  
 private:
  // Setable Properties
  double m_xCenter;   // Beam center position
  double m_yCenter;   // Beam center position
  double m_zCenter;   // Production point
  int m_zDir;         // Beam direction (Beam 1: 1, Beam 2: -1)
  double m_theta;     // Beam angle (inclination)
  double m_phi;       // Beam angle (azimuth)
  double m_momentum;  // Beam particle momentum
  double m_xEmm;      // Transverse emmitance
  double m_yEmm;      // Transverse emmitance
  double m_xBeta;     // Optical beta function
  double m_yBeta;     // Optical beta function
  double m_xAlpha;    // Correlation function
  double m_yAlpha;    // Correlation function
  int  m_pdgCode;     // PDG particle id number

	// Local Member Data:
  double m_xSigma;
  double m_ySigma;
  double m_pxCenter;
  double m_pyCenter;
  double m_pxSigma;
  double m_pySigma;
  double m_mass;

  // Random generators:
  Rndm::Numbers m_flatGenerator ;
  Rndm::Numbers m_gaussGenerator ;
} ;

#endif // PARTICLEGUNS_BEAMSHAPE_H
