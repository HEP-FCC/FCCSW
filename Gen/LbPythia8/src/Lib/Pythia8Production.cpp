// Gaudi.
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/ParticleProperty.h"
#include "Event/IParticlePropertySvc.h"
#include "GaudiKernel/PhysicalConstants.h"
 
// Event.
#include "Event/GenCollision.h"

// Generators.
#include "Generators/IBeamTool.h"

// HepMC.
#include "HepMC/GenEvent.h"
#include "HepMC/HEPEVT_Wrapper.h"

// LbPythia8.
#include "LbPythia8/Pythia8Production.h"

//-----------------------------------------------------------------------------
// Implementation file for class: Pythia8Production
//
// 2007-07-31 : Arthur de Gromard, Philip Ilten
//-----------------------------------------------------------------------------

//=============================================================================
// Default constructor.
//=============================================================================
Pythia8Production::Pythia8Production(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
  : GaudiTool(type, name, parent), m_pythia(0), m_hooks(0), m_lhaup(0),
    m_beamTool(0), m_pythiaBeamTool(0), m_randomEngine(0), m_nEvents(0),
    m_showBanner(false), m_xmlLogTool(0) {
 
  // Declare the tool properties.
  declareInterface<IProductionTool>(this);
  declareProperty("Commands", m_userSettings,
		  "List of commands to pass to Pythia 8.");
  declareProperty("BeamToolName", m_beamToolName = "CollidingBeams",
		  "The beam tool to use.");
  declareProperty("ValidateHEPEVT", m_validate_HEPEVT = false,
		  "Flag to validate the Pythia 8 event record.");
  declareProperty("ListAllParticles", m_listAllParticles = false,
		  "List all the particles being used by Pythia 8."); 
  declareProperty("CheckParticleProperties", m_checkParticleProperties = false,
		  "Check the particle properties for consistency.");
  declareProperty("Tuning", m_tuningFile = "LHCbDefault.cmd",
		  "Name of the tuning file to use.");
  declareProperty("UserTuning", m_tuningUserFile = "",
		  "Name of the user tuning file to use. Using the tune subrun "
		  "will overwrite the default LHCb tune."); 
  declareProperty("ShowBanner", m_showBanner = false,
		  "Flag to print the Pythia 8 banner at initialization.");

  // Set the special particles.
  for (int i = 1; i <= 8; ++i)   m_special.insert(i);
  for (int i = 33; i <= 34; ++i) m_special.insert(i);
  for (int i = 41; i <= 44; ++i) m_special.insert(i);
  for (int i = 81; i <= 85; ++i) m_special.insert(i);
  for (int i = 88; i <= 99; ++i) m_special.insert(i);
  m_special.insert(17);      m_special.insert(1103); m_special.insert(4301);   
  m_special.insert(18);      m_special.insert(2101); m_special.insert(4303);   
  m_special.insert(21);	     m_special.insert(2103); m_special.insert(4401);   
  m_special.insert(110);     m_special.insert(2201); m_special.insert(4403);   
  m_special.insert(990);     m_special.insert(2203); m_special.insert(5101);   
  m_special.insert(37);	     m_special.insert(3101); m_special.insert(5103);   
  m_special.insert(39);	     m_special.insert(3103); m_special.insert(5201);   
  m_special.insert(9900110); m_special.insert(3201); m_special.insert(5203);   
  m_special.insert(9900210); m_special.insert(3203); m_special.insert(5301);   
  m_special.insert(9900220); m_special.insert(3301); m_special.insert(5303);   
  m_special.insert(9900330); m_special.insert(3303); m_special.insert(5401);   
  m_special.insert(9900440); m_special.insert(4101); m_special.insert(5403);   
  m_special.insert(9902110); m_special.insert(4103); m_special.insert(5501);   
  m_special.insert(9902210); m_special.insert(4201); m_special.insert(5503);   
  m_special.insert(1101);    m_special.insert(4203); m_special.insert(1000022);
  m_special.insert(1000024);
}

//=============================================================================
// Default destructor. 
//=============================================================================
Pythia8Production::~Pythia8Production() {}

//=============================================================================
// Initialize the tool.
//=============================================================================
StatusCode Pythia8Production::initialize() {  

  // Print the initialization banner.
  always() << "============================================================="
	   << "=====" << endmsg;
  always() << "Using as production engine " << this->type() << endmsg;
  always() << "============================================================="
	   << "=====" << endmsg;

  // Initialize the Gaudi tool.
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
    Exception("Failed to initialize the Gaudi tool.");
  
  // Initialize the random number generator.
  IRndmGenSvc* randSvc(0);
  try {randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);}
  catch (const GaudiException& e) 
    {Exception("Failed to initialize the RndmGenSvc.");}
  m_randomEngine = new GaudiRandomForPythia8(randSvc , sc);
  if (!sc.isSuccess()) 
    Exception("Failed to initialize GaudiRandomForPythia8.");
  release(randSvc);

  // Initialize the beam tool.
  m_beamTool = tool<IBeamTool>(m_beamToolName, this);
  if (!m_beamTool) Exception("Failed to initialize the IBeamTool.");

  // Initialize the user hooks.
  if (!m_hooks) m_hooks = new Pythia8::LhcbHooks();

  // Initialze the XML log file.
  m_xmlLogTool = tool<ICounterLogFile >("XmlCounterLogFile");

  // Create the Pythia 8 generator.
  std::string xmlpath("UNKNOWN" != System::getEnv("PYTHIA8XML") ?
		      System::getEnv("PYTHIA8XML") : ""); 
  m_pythia = new Pythia8::Pythia(xmlpath, m_showBanner); 
  if (!m_pythia) return StatusCode::FAILURE;

  // Initialize the Pythia beam tool.
  m_pythiaBeamTool = new BeamToolForPythia8(m_beamTool, m_pythia->settings, sc);
  if (!sc.isSuccess()) 
    return Error("Failed to initialize the BeamToolForPythia8."); 
  return sc;
}

//=============================================================================
// Initialize the Pythia 8 generator.
//=============================================================================
StatusCode Pythia8Production::initializeGenerator() {

  // Initialize the external pointers.
  m_pythia->setRndmEnginePtr(m_randomEngine);
  m_pythia->setUserHooksPtr(m_hooks);
  m_pythia->setBeamShapePtr(m_pythiaBeamTool);

  // Set the beam configuration.
  Gaudi::XYZVector beamA, beamB;
  m_beamTool->getMeanBeams(beamA, beamB);
  beamA /= Gaudi::Units::GeV;
  beamB /= Gaudi::Units::GeV;
  m_pythia->settings.parm("Beams:pxA", beamA.X());
  m_pythia->settings.parm("Beams:pyA", beamA.Y());
  m_pythia->settings.parm("Beams:pzA", beamA.Z());
  m_pythia->settings.parm("Beams:pxB", beamB.X());
  m_pythia->settings.parm("Beams:pyB", beamB.Y());
  m_pythia->settings.parm("Beams:pzB", beamB.Z());

  // Read the tuning files.
  if ("UNKNOWN" != System::getEnv("LBPYTHIA8ROOT") && m_pythia->readFile
      (System::getEnv("LBPYTHIA8ROOT") + "/options/" + m_tuningFile));
  else
    Warning("Failed to find $LBPYTHIA8ROOT/options/" + m_tuningFile +
	    ", using default options.");
  if (m_tuningUserFile != "" && !m_pythia->readFile(m_tuningUserFile))
    Warning ("Failed to find " + m_tuningUserFile + ".");

  // Turn off minimum bias if using LHAup.
  if (m_lhaup) {
    vector<string> procs; procs.push_back("SoftQCD:"); 
    procs.push_back("HardQCD:");    procs.push_back("Onia:"); 
    procs.push_back("Charmonium:"); procs.push_back("Bottomonium:");
    for (unsigned int proc = 0; proc < procs.size(); ++proc) {
      map<string, Pythia8::FVec> fvecs = 
	m_pythia->settings.getFVecMap(procs[proc]);
      map<string, Pythia8::Flag> flags = 
	m_pythia->settings.getFlagMap(procs[proc]);
      for(map<string, Pythia8::FVec>::iterator itr = fvecs.begin(); 
	  itr != fvecs.end(); ++itr)
	m_pythia->settings.fvec(itr->first, vector<bool>
				(itr->second.valNow.size(), false));
      for(map<string, Pythia8::Flag>::iterator itr = flags.begin(); 
	  itr != flags.end(); ++itr)
	m_pythia->settings.flag(itr->first, false);
    }
  }

  // Read user settings.
  for (unsigned int setting = 0; setting < m_userSettings.size(); ++setting) {
    debug() << m_userSettings[setting] << endmsg;
    if (!m_pythia->readString(m_userSettings[setting]))
      Warning ("Failed to read the command " + m_userSettings[setting] + ".");
  }

  // Check particle properties if requested.
  if (m_checkParticleProperties) {
    int id = m_pythia->particleData.nextId(0);
    while (id != 0) {
      if (!m_pythia->particleData.hasChanged(id))
	warning() << "Data for particle with ID " << id
		  << " and name " << m_pythia->particleData.name(id)
		  << " has not been changed." << endmsg;
      id = m_pythia->particleData.nextId(id);
    }
  }
  
  // Initialize.
  if (m_lhaup) {
    if (m_pythia->init(m_lhaup)) return StatusCode::SUCCESS;
    else return Error("Failed to initialize Pythia 8 with LHAUP pointer.");
  } else if (m_pythia->init()) {return StatusCode::SUCCESS;
  } else return Error("Failed to initialize Pythia 8.");
}

//=============================================================================
// Finalize the tool.
//=============================================================================
StatusCode Pythia8Production::finalize() {

  // Print the statistics.
  m_pythia->statistics();

  // Write the cross-sections to the XML log.
  vector<int> codes = m_pythia->info.codesHard();
  for (unsigned int code = 0; code < codes.size(); ++code)
    m_xmlLogTool->addCrossSection(m_pythia->info.nameProc(codes[code]), 
				  codes[code],
				  m_pythia->info.nAccepted(codes[code]),
				  m_pythia->info.sigmaGen(codes[code]));
  
  // Clean up.
  if (m_pythiaBeamTool) delete m_pythiaBeamTool;
  if (m_randomEngine) delete m_randomEngine;
  if (m_lhaup) delete m_lhaup;
  if (m_hooks) delete m_hooks;
  if (m_pythia) delete m_pythia;
  return GaudiTool::finalize();
}

//=============================================================================
// Generate an event.
//=============================================================================
StatusCode Pythia8Production::generateEvent(HepMC::GenEvent* theEvent,
					    LHCb::GenCollision* theCollision) {

  // Generate the event (make 10 attempts).
  int tries(0);
  while (!m_pythia->next() && tries < 10) ++tries;
  if (tries == 10) return Error("Pythia 8 event generation failed 10 times.");
  if (!m_pythia->flag("HadronLevel:all")) m_event = m_pythia->event;  
  ++m_nEvents;

  // Convert the event to HepMC and return.
  if (theCollision->isSignal() || m_pythia->flag("HadronLevel:all")) 
    return toHepMC(theEvent, theCollision);
  else return StatusCode::SUCCESS;
}

//=============================================================================
// Convert the Pythia 8 event to HepMC format.
//=============================================================================
StatusCode Pythia8Production::toHepMC(HepMC::GenEvent* theEvent, 
				      LHCb::GenCollision* theCollision) {

  // Convert to HepMC.
  HepMC::Pythia8ToHepMC conversion;
  conversion.set_print_inconsistency(m_validate_HEPEVT);
  if (!(conversion.fill_next_event(*m_pythia, theEvent))) 
    return Error("Failed to convert Pythia 8 event to HepMC.");
  
  // Convert status codes and IDs.
  for (HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p) {
    int status = (*p)->status();
    int pid    = (*p)->pdg_id();
    if (status > 3) {
      if ((status == 71) || (status == 72) || 
	  ((status == 62) && (abs(pid) >= 22) && (abs(pid) <= 37)))
        (*p)->set_status(LHCb::HepMCEvent::DecayedByProdGen);
      else
        (*p)->set_status(LHCb::HepMCEvent::DocumentationParticle);
    } else if (status != LHCb::HepMCEvent::DecayedByProdGen
               && status != LHCb::HepMCEvent::StableInProdGen
               && status != LHCb::HepMCEvent::DocumentationParticle)
      warning() << "Unknown status rule " << status << " for particle" 
                << pid << endmsg;
    if (abs(pid) == 10221) (*p)->set_pdg_id(pid > 0 ? 30221 : -30221);
  }
  
  // Convert to LHCb units.
  for (HepMC::GenEvent::vertex_iterator v = theEvent->vertices_begin();
       v != theEvent->vertices_end(); ++v) 
    (*v)->set_position(HepMC::FourVector
		       ((*v)->position().x(), (*v)->position().y(),
			(*v)->position().z(), 
			((*v)->position().t() * Gaudi::Units::mm) 
			/ Gaudi::Units::c_light));

  // Set the process and collision info.
  int code(m_pythia->info.hasSub() ? m_pythia->info.codeSub() : 
	   m_pythia->info.code());
  theEvent->set_signal_process_id(code);
  theCollision->setProcessType(code);
  theCollision->setSHat(m_pythia->info.sHat());
  theCollision->setTHat(m_pythia->info.tHat());
  theCollision->setUHat(m_pythia->info.uHat());
  theCollision->setPtHat(m_pythia->info.pTHat());
  theCollision->setX1Bjorken(m_pythia->info.x1());
  theCollision->setX2Bjorken(m_pythia->info.x2());
  return StatusCode::SUCCESS;
}

//=============================================================================
// Set a particle stable.
//=============================================================================
void Pythia8Production::setStable(const LHCb::ParticleProperty* thePP) {
  m_pythia->particleData.mayDecay(pythia8Id(thePP), false);
}

//=============================================================================
// Update a particle.
//=============================================================================
void Pythia8Production::updateParticleProperties(const LHCb::ParticleProperty* 
						 thePP) {
  
  // Create the particle if needed.
  int id = pythia8Id(thePP);
  string name = thePP->name();
  Pythia8::ParticleData &pd = m_pythia->particleData;
  if (id == 0) {
    const LHCb::ParticleID pid = thePP->pid();
    id = pid.pid();
    if (id < 0)
      if (pd.isParticle(abs(id))) pd.names(abs(id), pd.name(abs(id)), name);
      else pd.addParticle(id, "void", name, pid.sSpin(), -3 * thePP->charge());
    else pd.addParticle(id, name, pid.sSpin(), 3 * thePP->charge());
    id = pythia8Id(thePP);
    if (id == 0) return;
  }
  if (pd.name(id) == "void") pd.name(id, name);

  // Set the mass, width and lifetime (only non-resonant).
  pd.m0(id, thePP->mass() / Gaudi::Units::GeV);
  if (id == 6 || (id >= 23 || id <= 37)) return;
  double lifetime = thePP->lifetime()*Gaudi::Units::c_light;
  if (lifetime <= 1.e-4 * Gaudi::Units::mm || 
      lifetime >= 1.e16 * Gaudi::Units::mm) lifetime = 0;
  double width = lifetime == 0 ? 0 : Gaudi::Units::hbarc / lifetime;
  if (width < 1.5e-6*Gaudi::Units::GeV) {width = 0; pd.mMin(id, 0);} 
  else pd.mMin(id, (thePP->mass() - thePP->maxWidth()) / Gaudi::Units::GeV); 
  pd.mWidth(id, width / Gaudi::Units::GeV);
  pd.mMax(id, 0);
  pd.tau0(id, lifetime / Gaudi::Units::mm);
}

//=============================================================================
// Turn on and off fragmentation.
//=============================================================================
void Pythia8Production::turnOnFragmentation() {
  m_pythia->settings.flag("HadronLevel:Hadronize", true);
}

void Pythia8Production::turnOffFragmentation() {
  m_pythia->settings.flag("HadronLevel:Hadronize", false);
}

//=============================================================================
// Hadronize an event.
//=============================================================================
StatusCode Pythia8Production::hadronize(HepMC::GenEvent* theEvent, 
					LHCb::GenCollision* theCollision) {
  if (!m_pythia->forceHadronLevel()) return StatusCode::FAILURE;
  return toHepMC(theEvent, theCollision);
}

//=============================================================================
// Save the Pythia 8 event record.
//=============================================================================
void Pythia8Production::savePartonEvent( HepMC::GenEvent* /*theEvent*/) 
{m_event = m_pythia->event;}

//=============================================================================
// Retrieve the Pythia 8 event record.
//=============================================================================
void Pythia8Production::retrievePartonEvent(HepMC::GenEvent* /*theEvent*/)
{m_pythia->event = m_event;}

//=============================================================================
// Print the running conditions.
//=============================================================================
void Pythia8Production::printRunningConditions() { 
  if (m_nEvents == 0 && m_listAllParticles == true && msgLevel(MSG::DEBUG)) 
    m_pythia->particleData.listAll();
  if (msgLevel(MSG::VERBOSE)) m_pythia->settings.listAll();
  else if (msgLevel(MSG::DEBUG)) m_pythia->settings.listChanged();
}

//=============================================================================
// Return whether a particle has special status.
//=============================================================================
bool Pythia8Production::isSpecialParticle(const LHCb::ParticleProperty* thePP) 
  const {return m_special.find(thePP->pid().abspid()) != m_special.end();}

//=============================================================================
// Setup forced fragmentation.
//=============================================================================
StatusCode Pythia8Production::setupForcedFragmentation(const int /*thePdgId*/) {
  m_pythia->settings.flag("PartonLevel:all", false); 
  return StatusCode::SUCCESS ;  
}

//=============================================================================
// Return the Pythia 8 ID.
//=============================================================================
int Pythia8Production::pythia8Id(const LHCb::ParticleProperty* thePP) {
  int id(thePP->pid().pid());
  if (abs(id) == 30221) return id > 0 ? 10221 : -10221;
  if (abs(id) == 104124) return id > 0 ? 4124 : -4124;
  if (m_pythia->particleData.isParticle(id)) return id;
  return 0;
}

//=============================================================================
// The END.
//=============================================================================
