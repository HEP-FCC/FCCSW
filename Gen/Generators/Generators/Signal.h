// $Id: Signal.h,v 1.8 2007-02-22 13:30:24 robbep Exp $
#ifndef GENERATORS_SIGNAL_H 
#define GENERATORS_SIGNAL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "Generators/ExternalGenerator.h" 

#include "Generators/GenCounters.h"

#include <boost/array.hpp>

// Forward declarations
namespace HepMC {
  class GenParticle ;
}

/** @class Signal Signal.h "Generators/Signal.h"
 *  
 *  Base class for signal samples generation tools. It contains utility 
 *  function that can be used when generating signal samples.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class Signal : public ExternalGenerator {
 public:
  /// Vector of particles
  typedef std::vector< HepMC::GenParticle * > GenParticles ;

  /// Standard constructor
  Signal( const std::string& type, const std::string& name, 
          const IInterface* parent);
  
  virtual ~Signal( ); ///< Destructor
  
  /// Initialize
  virtual StatusCode initialize( ) ;  

  /// Print counters and cut efficiencies at the end of the job.
  virtual void printCounters( ) const ;

protected:
  /// PID of the signal particle for this job (set by job options)
  PIDs          m_pids             ;

  /** Flag to indicate if clean events have to be generated (set by job 
   *  options). Clean events are events where only the signal decay tree
   *  is kept.
   */
  bool          m_cleanEvents      ;

  /// Number of events before the generator level cut.
  unsigned int  m_nEventsBeforeCut ; 

  /** Number of events accepted by the generator level cut. This counter is
   *  used to compute the generator cut efficiency on inclusive events.
   */
  unsigned int  m_nEventsAfterCut  ;

  /** Number of signal particles (w/r to anti-particles) before the generator 
   *  level cut.
   */
  unsigned int  m_nParticlesBeforeCut ;

  /// Number of signal anti-particles before the generator level cut.
  unsigned int  m_nAntiParticlesBeforeCut ;

  /** Number of signal particles accepted by the generator level cut. This 
   *  counter is used to compute the generator level cut efficiency to compute
   *  signal yields.
   */
  unsigned int  m_nParticlesAfterCut ;

  /// Number of signal anti-particles accepted by the generator level cut.
  unsigned int  m_nAntiParticlesAfterCut ;

  /// Number of events parity-flipped. (z -> -z, pz -> -pz)
  unsigned int  m_nInvertedEvents  ;

  /// Heavier quark of the signal particle
  LHCb::ParticleID::Quark m_signalQuark ; 

  unsigned int  m_signalPID        ; ///< PDG Id of the signal particles

  unsigned int  m_bbCounter        ; ///< Counter of (bb) states in tag side

  unsigned int  m_ccCounter        ; ///< Counter of (cc) states in tag side

  unsigned int  m_nSig             ; ///< Counter of generated signal

  unsigned int  m_nSigBar          ; ///< Counter of generated anti-signal

  std::string   m_sigName          ; ///< Name of signal

  std::string   m_sigBarName       ; ///< Name of anti-signal

  GenCounters::BHadronCounter m_bHadC ; ///< Counter of B hadron (tag side)

  /// Counter of Bbar hadron (tag side)
  GenCounters::BHadronCounter m_antibHadC ; 

  GenCounters::DHadronCounter m_cHadC ; ///< Counter of D hadron (tag side)

  /// Counter of Dbar hadron (tag side)
  GenCounters::DHadronCounter m_anticHadC ;

  GenCounters::BHadronCNames m_bHadCNames ; ///< Array of B counter names

  /// Array of Bbar hadron names
  GenCounters::BHadronCNames m_antibHadCNames ;
  
  GenCounters::DHadronCNames m_cHadCNames ; ///< Array of D counter names

  /// Array of Dbar hadron counter names
  GenCounters::DHadronCNames m_anticHadCNames ;

  GenCounters::ExcitedCounter m_bExcitedC ; ///< Counter of B(**) (signal side)

  GenCounters::ExcitedCounter m_cExcitedC ; ///< Counter of D(**) (signal side)

  GenCounters::ExcitedCNames m_bExcitedCNames ; ///< Names excited B couters
  
  GenCounters::ExcitedCNames m_cExcitedCNames ; ///< Names excited D counters
  
  /// Flag to indicate that a mixture of signal particles and anti-particles.
  bool          m_cpMixture        ;

  /** Branching fraction of the signal forced decay mode in the main decay 
   *  table.
   */
  double        m_signalBr         ;

  Rndm::Numbers m_flatGenerator    ; ///< Flat random nunmber generator

  /** Isolate signal particle decay tree for clean events. The resulting
   *  event is stored in the container /Event/Gen/SignalDecayTree.
   */
  StatusCode isolateSignal( const HepMC::GenParticle * theSignal ) const ;

  /** Choose randomly a particle in a list of particles 
   *  and returns the chosen particle. Invert the event if pz(Signal)<0.
   *  isInverted is set to true when the chosen particle is inverted.
   *  The signal particle is forced to decay inside this function. 
   *  hasFlipped is set to true if the particle is flipped by EvtGen
   *  to generate CP violation. 
   */
  HepMC::GenParticle * chooseAndRevert( const ParticleVector & particleList ,
                                        bool & isInverted , 
                                        bool & hasFlipped ,
					bool & hasFailed ) ;

  /** Ensures correct multiplicity of signal particles with an accept/reject
   *  algorithm.
   *  @param[in] nSignal  Number of particles in the event with flavour 
   *                      corresponding to the flavour of the signal particles.
   *  @return true if the event has to kept.
   *  @return false if the event has to be rejected.
   */
  bool ensureMultiplicity( const unsigned int nSignal ) ;

  /** Update counters for efficiency calculations.
   *  @param[in]  particleList         List of particles to count.
   *  @param[out] particleCounter      Number of particles (w/r to 
   *                                   antiparticles) in particleList.
   *  @param[out] antiparticleCounter  Number of anti-particles in particleList
   *  @param[in]  onlyForwardParticles If true, count only particles and 
   *                                   anti-particles with pz>0.
   *  @param[in]  isInverted           If true, the first particle in the
   *                                   particleList has been inverted z -> -z
   */
  void updateCounters( const ParticleVector & particleList , 
                       unsigned int & particleCounter , 
                       unsigned int & antiparticleCounter ,
                       bool onlyForwardParticles , 
                       bool isInverted ) const ;
  
 private:
  /** Add to an existing event a new decay tree.
   *  @param[out] theNewParticle  Particle to update with the decay tree
   *                              of theOldParticle.
   *  @param[in]  theOldParticle  Particle containing the decay tree to add
   *                              to theNewParticle.
   */
  StatusCode fillHepMCEvent( HepMC::GenParticle * theNewParticle ,
                             const HepMC::GenParticle * theOldParticle ) 
    const ;

  /// Temporary ector to obtain PIDs from job options.
  std::vector< int > m_pidVector   ;
};
#endif // GENERATORS_SIGNAL_H
