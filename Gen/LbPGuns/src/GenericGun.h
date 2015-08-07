// $Id: GenericGun.h,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
#ifndef PARTICLEGUNS_GENERICGUN_H
#define PARTICLEGUNS_GENERICGUN_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

// from ParticleGuns
#include "LbPGuns/IParticleGunTool.h"

namespace SPGGenMode {
  enum Mode { FixedMode = 1, GaussMode , FlatMode } ;
}

/** @class GenericGun GenericGun.h "GenericGun.h"
 *
 *  This code is used to generate particles with pt, eta and phi selected as
 *  specified by job options.  Three modes of generation are possible:
 *          - FixedMode (1):  Generation with a fixed value
 *          - GaussianMode (2): Generation with a gaussian of specified mean and
 *                              sigma
 *          - FlatMode (3): Generation of a flat distribution between a specified
 *                          minimum and maximum value
 *    The mode can be separately selected for pt, eta and phi.
 *    The particle pdg code can also be specified
 *
 *  @author M. Shapiro
 *  @author W. Pokorski
 *  @date 2000-03-01
 */
class GenericGun : public GaudiTool , virtual public IParticleGunTool {
 public:
  
  /// Constructor
  GenericGun( const std::string & type , const std::string & name , 
              const IInterface * parent ) ;
  
  virtual ~GenericGun() ; ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize();
  
  /// Generate the particle
  virtual void generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                 Gaudi::LorentzVector & origin , 
                                 int & pdgId ) ;
                                 
  /// Print counters
  virtual void printCounters( ) { ; } ;
  
 private:
	// Setable Properties:-
  double m_requestedPt;
  double m_requestedEta;
  double m_requestedPhi;
  double m_minPt;
  double m_minEta;
  double m_minPhi;
  double m_maxPt;
  double m_maxEta;
  double m_maxPhi;
  double m_sigmaPt;
  double m_sigmaEta;
  double m_sigmaPhi;
  int m_PtGenMode , m_EtaGenMode , m_PhiGenMode ; 
  int m_pdgCode;  

	// Local Member Data:
  double m_mass ;

	// Private Methods:
  double generateValue( const int mode , const double val , 
                        const double sigma , const double min , 
                        const double max ) ;

  // Random generators:
  Rndm::Numbers m_flatGenerator ;
  Rndm::Numbers m_gaussGenerator ;
} ;

#endif // PARTICLEGUNS_GENERICGUN_H
