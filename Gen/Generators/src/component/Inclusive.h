// $Id: Inclusive.h,v 1.7 2006-02-17 13:26:44 robbep Exp $
#ifndef GENERATORS_INCLUSIVE_H 
#define GENERATORS_INCLUSIVE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/ExternalGenerator.h" 

#include "Generators/GenCounters.h"

#include <boost/array.hpp>

// forward declarations

namespace HepMC {
  class GenParticle ;
}

class ICounterLogFile ;

/** @class Inclusive Inclusive.h "Inclusive.h" 
 *  
 *  Tool for inclusive samples generation. The particles to 
 *  request are specified by their PDG Id in a list. Concrete
 *  implementation of a ISampleGenerationTool using the 
 *  ExternalGenerator base class.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class Inclusive : public ExternalGenerator {
 public:
  /// Standard constructor
  Inclusive( const std::string& type, const std::string& name,
             const IInterface* parent);
  
  virtual ~Inclusive( ); ///< Destructor

  /** Initialize method 
   *  Find the lightest particle among the requested particles.
   *  When generating events, all particles heavier than this mass
   *  will be decayed with the IDecayTool before the event is analyzed
   *  to see if it contains particles of the inclusive list.
   */
  virtual StatusCode initialize( ) ;

  /** Generate a set of interactions.
   *  Implements ISampleGenerationTool::generate.
   *  -# Generate interactions using the IProductionTool
   *  -# Check if the interaction has a particle specified in
   *     the PID list
   *  -# Apply the IGenCutTool on the interaction containing
   *     a particle of the list.
   *  -# Parity-flip the event (z -> -z, pz -> -pz) if all particles
   *     of the inclusive list in the event have pz < 0.
   */
  virtual bool generate( const unsigned int nPileUp ,
                         LHCb::HepMCEvents * theEvents ,
                         LHCb::GenCollisions * theCollisions ) ;

  /** Print generation counters.
   *  Implements ISampleGenerationTool::printCounters.
   *  Prints (after generator level cuts):
   *  -# Fractions of B0/B+/B0s/Bc/b-Baryons (and idem for D) in 
   *     selected events.
   *  -# Fractions of B/Bstar/Bdoublestar in selected events.
   */
  virtual void printCounters( ) const ;

 protected:

 private:
  ICounterLogFile * m_xmlLogTool ; ///< XML log file for counters

  /// Type of lightest quark in list of particles to produce
  LHCb::ParticleID::Quark      m_lightestQuark     ; 

  /// Counter of events before applying generator level cut
  unsigned int  m_nEventsBeforeCut ;

  /// Counter of events after applying generator level cut
  unsigned int  m_nEventsAfterCut  ;

  /// Counter of parity-flipped events (z -> -z, pz -> -pz)
  unsigned int  m_nInvertedEvents  ;

  /// Ordered set of PDG Id of particles to produce (set by options)
  PIDs          m_pids             ;

  /// Vector to obtain list of PDG Ids from job options
  std::vector< int > m_pidVector   ;
  
  GenCounters::BHadronCounter m_bHadC ;  ///< Counter of B hadron (generated)
  
  GenCounters::BHadronCounter m_antibHadC ; ///< Counter of Bbar hadron (gen)

  /// Counter of B hadron (accepted)
  GenCounters::BHadronCounter m_bHadCAccepted ;

  /// Counter of anti-B hadron (accepted)
  GenCounters::BHadronCounter m_antibHadCAccepted ;
  
  GenCounters::BHadronCNames m_bHadCNames ; ///< Array of B counter names

  /// Array of anti-B hadrons names
  GenCounters::BHadronCNames m_antibHadCNames ; 

  GenCounters::DHadronCounter m_cHadC ; ///< Counter of D hadron (generated)

  GenCounters::DHadronCounter m_anticHadC ; ///< Counter of Dbar hadron (gen)

  /// Counter of D hadron (accepted)
  GenCounters::DHadronCounter m_cHadCAccepted ;
  
  /// Counter of D hadron (accepted) 
  GenCounters::DHadronCounter m_anticHadCAccepted ;
  
  /// Array of D counter names
  GenCounters::DHadronCNames m_cHadCNames ;

  /// Array of anti-D counter names
  GenCounters::DHadronCNames m_anticHadCNames ;

  unsigned int m_ccCounter ; ///< Counter for cc quarkonium (generated)

  unsigned int m_bbCounter ; ///< Counter for bb quarkonium (generated)

  unsigned int m_ccCounterAccepted ; ///< Counter for cc (accepted)

  unsigned int m_bbCounterAccepted ; ///<< Counter for bb (accepted)
  
  GenCounters::ExcitedCounter m_bExcitedC ; ///< Counter of B(**) (generated)

  GenCounters::ExcitedCounter m_cExcitedC ; ///< Counter of D(**) (generated)

  /// Counter of B(**) (accepted)
  GenCounters::ExcitedCounter m_bExcitedCAccepted ;
  
  /// Counter of D(**) (accepted)
  GenCounters::ExcitedCounter m_cExcitedCAccepted ;
  
  GenCounters::ExcitedCNames m_bExcitedCNames ; ///< Names excited B counters

  GenCounters::ExcitedCNames m_cExcitedCNames ; ///< Names excited D counters
};
#endif // GENERATORS_INCLUSIVE_H
