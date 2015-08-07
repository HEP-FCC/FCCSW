// local
#include "Pythia8Gun.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Event
#include "Event/GenCollision.h"

#include <math.h>

//=============================================================================
// Implementation file for class: Pythia8Gun
// 
// 2013-08-21: chitsanu.khurewathanakul@epfl.ch
//
// Derived from JetProduction by Neal Gueissaz (2007)
//
//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Pythia8Gun )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

Pythia8Gun::Pythia8Gun(const std::string& type,
                        const std::string& name,
                        const IInterface* parent)
  :Pythia8Production( type, name, parent){

  declareInterface< IProductionTool >( this ) ;

  // List of particles to be the bullets of the gun.
  // Each particle will be generate INDENPENDENTLY from each other, 
  // subjected to the Energy/Theta/Phi below.
  // Note that they must be colorless (So, don't even think about b-QUARK-gun)
  declareProperty("PDGList", m_listPdg);

  // Energy of jet. Unit in GeV (For interfacing. But Pythia's default is MeV)
  declareProperty("EnergyMin"     , m_energyMin   = 10.*Gaudi::Units::GeV);
  declareProperty("EnergyMax"     , m_energyMax   = 50.*Gaudi::Units::GeV);
  declareProperty("EnergyMean"    , m_energyMean  = 20.*Gaudi::Units::GeV);
  declareProperty("EnergySigma"   , m_energySigma = 50.*Gaudi::Units::GeV);
  declareProperty("EnergyGenMode" , m_energyGenMode = GenMode::FlatMode);

  // Angle theta (between pz,pt). Unit in radian
  declareProperty("ThetaMin"     , m_thetaMin     = 0.022); // eta ~ 4.50
  declareProperty("ThetaMax"     , m_thetaMax     = 0.27);  // eta ~ 1.99
  declareProperty("ThetaMean"    , m_thetaMean    = 0.1575);
  declareProperty("ThetaSigma"   , m_thetaSigma   = 0.1);
  declareProperty("ThetaGenMode" , m_thetaGenMode = GenMode::FlatMode);

  // Angle phi (between px,py). Unit in radian
  declareProperty("PhiMin"     , m_phiMin     = 0.0);
  declareProperty("PhiMax"     , m_phiMax     = M_PI*2);
  declareProperty("PhiMean"    , m_phiMean    = M_PI);
  declareProperty("PhiSigma"   , m_phiSigma   = 1.0);
  declareProperty("PhiGenMode" , m_phiGenMode = GenMode::FlatMode);

}

//=============================================================================
// Destructor 
//=============================================================================
Pythia8Gun::~Pythia8Gun() { ; }


//=============================================================================
// Initialize method
//=============================================================================
StatusCode Pythia8Gun::initialize(){
  
  // initialize
  // Plan A: Call super.initialize, then override from it. Failed.
  // Plan B: Do the strip-down version of super.initialize

  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc ;

  always() << "=============================================================" << endmsg;
  always() << "Using as production engine  " << this->type() << endmsg;
  always() << "=============================================================" << endmsg;

  //---------------------------------
  // MAIN ENGINE
  //---------------------------------
  // get xmlpath
  std::string xmlpath = "" ;
  if ( System::getEnv("PYTHIA8XML") != "UNKNOWN" ) 
    xmlpath  = System::getEnv( "PYTHIA8XML" ) ;
  //Initializing the pythia object
  m_pythia = new Pythia8::Pythia( xmlpath, true );

  
  //------------------------------------
  // Tuning (Pythia's style)
  //------------------------------------
  bool success = false ;
  // set default LHCb tuning options
  std::string optspath = "" ;
  if ( System::getEnv("LBPYTHIA8ROOT") != "UNKNOWN" ) {
    optspath  = System::getEnv( "LBPYTHIA8ROOT" ) ;
    success = m_pythia->readFile(optspath+"/options/"+m_tuningFile);
  }
  if (!success)
    Warning( "Cannot find LBPYTHIA8ROOT/options/"+m_tuningFile+", thus default pythia8 options are parsed" ) ;

  // add user defined tuning options
  if (m_tuningUserFile!="")
    success = m_pythia->readFile(m_tuningUserFile);
  if (!success)
    Warning( "Cannot find "+m_tuningUserFile+", thus default LHCb tune is not overwritten" ) ;

  // also read a vector of commands if any is provided
  // this will overwrite anything that is passed through a user tuning file
  // should be done as the last step of the reading of the options,
  // so that everything can be overwritten by the user.
  for (unsigned int count = 0; count<m_userSettings.size(); ++count) {
    debug() << m_userSettings[count] << endmsg;
    success = m_pythia->readString(m_userSettings[count]);
    if (!success)
      Warning("User's commandVector > readString found bad command: \n\t" + m_userSettings[count]); 
  }

  //--------------------------------------
  //initialize random number generator.
  //--------------------------------------

  IRndmGenSvc* m_RGS = svc<IRndmGenSvc> ( "RndmGenSvc", true );
  sc = m_flatGenerator.initialize( m_RGS ,Rndm::Flat(0.,1.));
  if ( ! sc.isSuccess())
    return Error("Could not initialize flat generator");
  sc = m_gaussGenerator.initialize( m_RGS, Rndm::Gauss(0.,1.));
  if ( ! sc.isSuccess())
    return Error("Could not initialize gaussian generator");


  //-----------------------------
  // Sanity check of input data
  //-----------------------------
  double temp;
  if(m_energyMin > m_energyMax){
    warning() << "EnergyMin > EnergyMax. I'll swap the two: " << endmsg;
    temp = m_energyMin;
    m_energyMin = m_energyMax;
    m_energyMax = temp;;
  }
  if(m_thetaMin > m_thetaMax){
    warning() << "ThetaMin > ThetaMax. I'll swap the two: " << endmsg;
    temp = m_thetaMin;
    m_thetaMin  = m_thetaMax;
    m_thetaMax  = temp;;
  }
  if(m_phiMin > m_phiMax){
    warning() << "PhiMin > PhiMax. I'll swap the two: " << endmsg;
    temp = m_phiMin;
    m_phiMin  = m_phiMax;
    m_phiMax  = temp;;
  }


  //-------------------------------------------------
  // *** KEY COMMAND for Pythia8JetProduction ***
  //-------------------------------------------------
  // Based on: http://home.thep.lu.se/~torbjorn/pythia81html/HadronLevelStandalone.html
  m_pythia->readString("ProcessLevel:all = off");
  m_pythia->readString("Standalone:allowResDec = on");

  // Summary info for user before exit init
  m_pythia->settings.listChanged();
  m_pythia->particleData.listChanged();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Function called to generate one event with Pythia 
//=============================================================================
StatusCode Pythia8Gun::generateEvent( HepMC::GenEvent * theEvent , 
                                      LHCb::GenCollision * theCollision ){

  // Temporary var for the pdg_loop
  double id, e, m, p, px, py, pz, pt, theta, phi;
  double status = 23; // Hardest outgoing particle. See http://home.thep.lu.se/~torbjorn/pythia81html/ParticleProperties.html

  // ** IMPORTANT **: Need to reset the existing event before crafting a custom one.
  m_pythia->event.reset();

  for(size_t i=0; i<m_listPdg.size(); ++i){
    id    = m_listPdg.at(i);
    m     = m_pythia->particleData.m0(id);
    e     = generateValue(m_energyGenMode , m_energyMean, m_energySigma , m_energyMin , m_energyMax);
    theta = generateValue(m_thetaGenMode  , m_thetaMean , m_thetaSigma  , m_thetaMin  , m_thetaMax);
    phi   = generateValue(m_phiGenMode    , m_phiMean   , m_phiSigma    , m_phiMin    , m_phiMax);
    e     /= 1000;  // Pythia's default is MeV!!
    p     = sqrt( pow(e,2) - pow(m,2) );
    pz    = p*cos(theta);
    pt    = p*sin(theta);
    px    = pt*cos(phi);    // Careful: Depends on convention!
    py    = pt*sin(phi);    // Careful: Depends on convention!
    debug() << "Appending event (id,px,py,pz,e,m) = (" << id << ", " << px << ", " << py 
           << ", " << pz << ", " << e << ", " << m << ")" << endreq;
    m_pythia->event.append(id, status, 0,0, px,py,pz,e,m);  
  }

  // Event is ready, run it!
  m_pythia->next();
  m_event = m_pythia->event;    // Legacy line
  if (msgLevel(MSG::VERBOSE)){
    m_event.list();  
  }

  if(theCollision->isSignal() || m_pythia->flag("HadronLevel:all")){
    return toHepMC( theEvent, theCollision);
  } else {
    return StatusCode::SUCCESS;
  }
}


//===============================================================
// generateValue: Generate random value from given constraint
//===============================================================

double Pythia8Gun::generateValue(const int mode, const double mean,
                                 const double sigma, const double min, const double max ) {
  double tmp;
  int i = 0;
  const int MAX_RETRY = 1000 ;

  switch (mode) {
    // Generate value according to Gaussian distribution: Use only mean & sigma
    case GenMode::GaussMode:
      do {
        tmp = m_gaussGenerator()*sigma + mean;
        i++;
      } while (( (tmp<min) || (tmp>max) ) && (i<MAX_RETRY));
      if ( i >= MAX_RETRY ) {
        error() << "Could not generate value in range within 1000 trials."<<endreq;
        error() << tmp << " not in [ " << min << " , " << max << " ]" << endreq;
        error() << "Please check consistency between the given " 
                << "sigma value and the (min, max) domain." << endreq;
      }
      return tmp;

    case GenMode::FlatMode:
      return min + m_flatGenerator()*(max-min);

    default :
      error() << "Unknown Generation Mode" << endreq;
      error() << "Please set Mode =1 (gaussian) or =2 (flat)" << endreq;
      return 0.;
  }  
}




