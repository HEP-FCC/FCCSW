#ifndef LBPYTHIA8_PYTHIA8PRODUCTION_H 
#define LBPYTHIA8_PYTHIA8PRODUCTION_H 1

// LbPythia8.
#include "LbPythia8/GaudiRandomForPythia8.h" 
#include "LbPythia8/BeamToolForPythia8.h"
#include "LbPythia8/ILHAupFortranTool.h"
#include "LbPythia8/LhcbHooks.h"

// Gaudi.
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"
#include "Generators/ICounterLogFile.h"

// Pythia8.
#include "Pythia8/Pythia.h"
#include "Pythia8/LHAFortran.h"
#include "Pythia8/Pythia8ToHepMC.h"

/** 
 * Production tool to generate events with Pythia 8.
 *  
 * Production of events using the Pythia 8 multi-purpose Monte Carlo generator
 * is provided by this class. THe Pythia 8 generator is a complete rewrite in
 * C++ of the previous FORTRAN Pythia 6 event generator. Currently the physics
 * manual for Pythia 8 is the same as for Pythia 6, arXiv:hep-ph/0603175. A
 * brief introduction to the specifics of Pythia 8 can be found in 
 * arXiv:0710.3820 while a continually updated online manual can be found at
 * http://home.thep.lu.se/~torbjorn/pythia81html/Welcome.html.
 * 
 * @class  Pythia8Production
 * @file   Pythia8Production.h 
 * @author Arthur de Gromard
 * @author Philip Ilten
 * @date   2007-06-28
 */
class Pythia8Production : public GaudiTool, virtual public IProductionTool {
public:
  typedef std::vector<std::string> CommandVector ;
  
  /// Default constructor.
  Pythia8Production(const std::string& type, const std::string& name,
		    const IInterface* parent);
  
  /// Default destructor.
  virtual ~Pythia8Production();
  
  /**
   * Initialize the tool.
   *
   * Intitializes the tool, but leaves the actual Pythia 8 initialization to
   * initializeGenerator(). Here the Gaudi tool, random number generator,
   * beam tool, user hooks (if not already supplied), and XML log file are
   * initialized.
   */
  virtual StatusCode initialize();
  
  /// Initialize the Pythia 8 generator.
  virtual StatusCode initializeGenerator();

  /// Finalize the tool.
  virtual StatusCode finalize();
  
  /// Generate an event.
  virtual StatusCode generateEvent(HepMC::GenEvent* theEvent, 
				   LHCb::GenCollision* theCollision);

  
  /**
   * Convert Pythia 8 event to HepMC format.
   * 
   * This method converts the native output of Pythia 8 to the HepMC format
   * using the conversion method provided by Pythia 8. However the status codes
   * and vertex positions must be modified to match the LHCb standard. The
   * hard process information is also set.
   */
  StatusCode toHepMC(HepMC::GenEvent* theEvent, 
		     LHCb::GenCollision* theCollision);

  /// Set particle stable.
  virtual void setStable(const LHCb::ParticleProperty* thePP);

  /// Update a particle.
  virtual void updateParticleProperties(const LHCb::ParticleProperty* thePP);

  /// Sets Pythia 8's "HadronLevel:Hadronize" flag to true.
  virtual void turnOnFragmentation();

  /// Sets Pythia 8's "HadronLevel:Hadronize" flag to false.
  virtual void turnOffFragmentation();

  /// Hadronize an event.
  virtual StatusCode hadronize(HepMC::GenEvent* theEvent, 
			       LHCb::GenCollision* theCollision);
  
  /// Save the Pythia 8 event record.
  virtual void savePartonEvent(HepMC::GenEvent* theEvent);

  /// Retrieve the Pythia 8 event record.
  virtual void retrievePartonEvent(HepMC::GenEvent* theEvent);

  /**
   * Print the running conditions.
   *
   * If the message level is at MSG::DEBUG or above and the ListAllParticles
   * property is set to true all particle data is printed. If the message level
   * is MSG::VERBOSE then all settings are listed, whereas if MSG::DEBUG then
   * only the changed settings are printed. Note that this method duplicates
   * the built in functionality of Pythia 8 and should be removed.
   */
  virtual void printRunningConditions();

  /**
   * Returns whether a particle has special status.
   *
   * If a particle has special status, then the particle cannot be modified.
   * This method checks if the particle is within the special particle set
   * built during construction of the class.
   */
  virtual bool isSpecialParticle( const LHCb::ParticleProperty* thePP) const;

  /**
   * Setup forced fragmentation.
   *
   * Used in conjuntion with the hadronize method. Here the Pythia 8 setting
   * of "PartonLevel:all" is set to off which stops both showers and 
   * hadronization from being performed.
   */
  virtual StatusCode setupForcedFragmentation(const int thePdgId);
  
  // The Pythia 8 members (needed externally).
  Pythia8::Pythia*    m_pythia; ///< The Pythia 8 generator.
  Pythia8::UserHooks* m_hooks;  ///< User hooks to veto events.
  Pythia8::LHAup*     m_lhaup;  ///< User specified hard process.
  Pythia8::Event      m_event;  ///< The Pythia 8 event record.

  // Members needed externally.
  std::string m_beamToolName;   ///< The name of the beam tool.
  
protected:

  /**
   * Return the Pythia 8 ID.
   *
   * In most cases the Pythia 8 ID is equivalent to the PDG ID. However,
   * currently the LHCb PDG ID codes are out-dated so the following conversions
   * need to be made:
   * f_0(1370): 30221 -> 10221
   * Lambda_c(2625)+: 104124 -> 4124
   * Any particle that is not found within the Pythia 8 particle database is
   * assigned an ID of 0.
   */
  int pythia8Id(const LHCb::ParticleProperty* thePP);
  
  // Additional members.
  IBeamTool* m_beamTool;                 ///< The Gaudi beam tool.
  BeamToolForPythia8* m_pythiaBeamTool;  ///< The Pythia 8 beam tool.
  GaudiRandomForPythia8* m_randomEngine; ///< Random number generator.
  int m_nEvents;                         ///< Number of generated events.
  CommandVector m_userSettings;          ///< The user settings vector.
  std::string m_tuningFile;              ///< The global tuning file.
  std::string m_tuningUserFile;          ///< The user tuning file.
  bool m_validate_HEPEVT;                ///< Flag to validate the event.
  bool m_listAllParticles;               ///< Flag to list all the particles.
  bool m_checkParticleProperties ;       ///< Flag to check particle properties.
  bool m_showBanner;                     ///< Flag to print the Pythia 8 banner.
  ICounterLogFile* m_xmlLogTool;         ///< The XML log file. 
  set<unsigned int> m_special;           ///< The set of special particles.
};

#endif // LBPYTHIA8_PYTHIA8PRODUCTION_H
