// $Id: ExternalGenerator.h,v 1.14 2009-12-03 15:32:49 robbep Exp $
#ifndef GENERATORS_EXTERNALGENERATOR_H
#define GENERATORS_EXTERNALGENERATOR_H 1

// Include files
#include <set>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/ISampleGenerationTool.h"

// from LHCb
#include "Event/ParticleID.h"

// forward declarations
class IProductionTool ;
class IDecayTool ;
class IGenCutTool ;
class ICounterLogFile ;

namespace LHCb { class IParticlePropertySvc ; }

namespace HepMC {
  class GenEvent ;
  class GenParticle ;
}

/** @class ExternalGenerator ExternalGenerator.h "Generators/ExternalGenerator.h"
 *
 *  Base class for sample generation tools which use external generators
 *  (Pythia, Herwig, ...)
 *  This class provides common utility functions which can be used to generate
 *  samples of events of various types.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */

class ExternalGenerator : public GaudiTool ,
                          public ISampleGenerationTool {
 public:
  /// Vector of HepMC particles
  typedef std::vector< HepMC::GenParticle * > ParticleVector ;

  /// Ordered set of integer to contain PID of particles to generate
  typedef std::set< int >                     PIDs           ;

  /// Vector of string to pass configuration commands to the external generator
  typedef std::vector< std::string >          CommandVector  ;

  /// Standard constructor
  ExternalGenerator( const std::string & type , const std::string & name ,
                     const IInterface * parent ) ;

  virtual ~ExternalGenerator( ) ; ///< Destructor

  /** Initialization function.
   *  The function:
   *  -# configures the LHAPDF library with a default set of commands (enabling
   *     or disabling LHAPDF output depending on the output level) and with
   *     optional user commands.
   *  -# loop over all particles defined in the particle property service
   *     and declare them stable for the production tool if they are known
   *     by the decay tool.
   */
  virtual StatusCode initialize( ) ;

  /** Finalization funtion.
   *  This function closes the file used to redirect LHAPDF output messages
   *  in case the output is disabled.
   */
  virtual StatusCode finalize( ) ;

 protected:
  /** Decay heavy excited particles.
   *  This function decay heavy excited particles with the DecayTool. It
   *  is necessary to decay excited particles earlier than the other because
   *  the signal particle can be produced by the decay of an excited particle
   *  eg. B*0 -> B0 gamma
   *  @param[in,out] theEvent  HepMC event which contains the particles to
   *                           decay. The event is updated with the resulting
   *                           decay trees.
   *  @param[in]     mass      Mass above which particles are considered "heavy
   *                           excited particles" and must be decayed with the
   *                           decay tool.
   *  @param[in]     pid       PID of the particle which is the ligther to
   *                           decay, the decay chain is stopped when this
   *                           particle is found.
   *  @return     StatusCode::SUCCESS
   *  @return     StatusCode::ERROR if an unkown excited particle is found (in
   *                                this case, this particle has to be added to
   *                                the ParticlePropertySvc or/and the EvtGen
   *                                decay table).
   */
  StatusCode decayHeavyParticles( HepMC::GenEvent * theEvent ,
                                  const LHCb::ParticleID::Quark theQuark ,
                                  const int signalPid ) const ;


  /** Find particles of given PIDs in an event
   *  @param[in]  pidList       Ordered (for efficiency) set of PIDs to look
   *                            for in this event.
   *  @param[in]  theEvent      Event to study.
   *  @param[out] particleList  List of particles contained in the event whose
   *                            PID is in the pidList.
   *  @return     true if the event contains a particle with correct PID.
   */
  bool checkPresence( const PIDs & pidList , const HepMC::GenEvent * theEvent ,
                      ParticleVector & particleList ) const ;

  /** Parity flip (z -> -z and pz -> -pz) the event
   *  @param[in,out] theEvent   Event to flip
   */
  void revertEvent( HepMC::GenEvent * theEvent ) const ;

  /** Count the number of particle with pz > 0
   *  @param[in] particleList  List of particles
   *  @return    Number of particle with pz > 0 in particleList.
   */
  unsigned int nPositivePz( const ParticleVector & particleList ) const ;

  /** Prepare interaction before generating it.
   *  Performs all necessary actions required to prepare LHCb event model
   *  classes before being able to generate interactions.
   *  @param[in,out] theEventVector  Vector containing the generated events
   *  @param[in,out] theHardVector   Vector containing the hard process
   *                                 informations.
   *  @param[out]    theGenEvent     Event where to store particles of the
   *                                 next generated interaction.
   *  @param[out]    theHardInfo     Object where to store hard process
   *                                 informations of the next interaction.
   */
  void prepareInteraction( LHCb::HepMCEvents * theEvents ,
                           LHCb::GenCollisions * theCollisions ,
                           HepMC::GenEvent * & theGenEvent ,
                           LHCb::GenCollision * & theGenCollision ) const ;

  /** Production tool (interface to external generator) to use to
   *  generate interactions */
  IProductionTool * m_productionTool ;

  /// Decay tool to use to decay excited heavy particles (B**)
  IDecayTool      * m_decayTool      ;

  /// Generator level cut tool to apply to generated interactions
  IGenCutTool     * m_cutTool        ;

  /// Log file tool
  ICounterLogFile * m_xmlLogTool ;

  /// Name assigned to the HepMC event and stored with it
  std::string m_hepMCName ;

 private:
  /** Decode commands (given by job options) to configure LHAPDF library.
   *  @param[in] theCommandVector  Vector of string to configure LHAPDF. The
   *                               syntax of the command is "lhacontrol block
   *                               index value" where block is lhaparm (in this
   *                               the common variable LHAPARM( index ) is set
   *                               to value) or lhavalue (to set
   *                               LHAVALUE( index ) to value).
   */
  StatusCode parseLhaPdfCommands( const CommandVector & theCommandVector )
    const ;

  /// Name of the production tool (external generator) to use
  std::string m_productionToolName ;

  /// Name of the decay tool to use
  std::string m_decayToolName ;

  /// Name of the generator level cut tool to use
  std::string m_cutToolName ;

  /// Commands to set the default configuration of LHAPDF (hardcoded)
  CommandVector m_defaultLhaPdfSettings ;

  /// Optional additional user commands to configure LHAPDF (by job options)
  CommandVector m_userLhaPdfSettings ;

  /// Keep original particle properties of the production generator
  bool m_keepOriginalProperties ;

  /// Particle property service
  LHCb::IParticlePropertySvc * m_ppSvc ;
};
#endif // GENERATORS_EXTERNALGENERATOR_H
