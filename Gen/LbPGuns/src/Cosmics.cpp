// $Id: Cosmics.cpp,v 1.1.1.1 2009-09-18 16:18:24 gcorti Exp $
// Include files

// This class
#include "Cosmics.h"

// From STL
#include <cmath>

// From Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/IParticlePropertySvc.h"
#include "Event/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IRndmGenSvc.h"

//===========================================================================
// Implementation file for class: Cosmics
//
// 2008-05-18: Giulia Manca
//===========================================================================

DECLARE_TOOL_FACTORY( Cosmics )

//===========================================================================
// Constructor
//===========================================================================
Cosmics::Cosmics( const std::string& type ,
                  const std::string& name , 
                  const IInterface* parent )
  : GaudiTool( type , name, parent ) {
    declareInterface< IParticleGunTool >( this ) ;
    
    //momentum of the mu at detection
    declareProperty( "MomentumMin" , m_minMom =   1000.0 * Gaudi::Units::MeV ) ;
    //zenith angle of the mu with the vertical
    declareProperty( "ThetaMin"     , m_minTheta = 0.1 * Gaudi::Units::rad ) ;
    //azimuthal angle
    declareProperty( "PhiMin"      , m_minPhi =   0. * Gaudi::Units::rad ) ;

    declareProperty( "MomentumMax" , m_maxMom   = 100000.0 * Gaudi::Units::MeV ) ;
    declareProperty( "ThetaMax"     , m_maxTheta = 1.5708 * Gaudi::Units::rad ) ;//90
    declareProperty( "PhiMax"      , 
                     m_maxPhi   = Gaudi::Units::twopi* Gaudi::Units::rad ) ;//360

    m_pdgCodes.push_back( 13 ); // default mu-
    m_pdgCodes.push_back( -13 ); // add mu+
    declareProperty("PdgCodes",m_pdgCodes);

    //xyz define the surface of the detector I want to
    //uniformly populate. The vertex is then extrapolated to
    //a plane above the detector at y=+6m.
    //default is the all detector at zero
    declareProperty("xMin", m_minxvtx = -5000.0 * Gaudi::Units::mm ) ;
    declareProperty("yMin", m_minyvtx = -5000.0 * Gaudi::Units::mm ) ;
    declareProperty("xMax", m_maxxvtx = 5000.0 * Gaudi::Units::mm ) ;
    declareProperty("yMax", m_maxyvtx = 5000.0 * Gaudi::Units::mm ) ;
    declareProperty("z", m_zvtx = 0.0 * Gaudi::Units::mm ) ;
    //Time at which I want the mu to hit the detector;
    declareProperty("TimeOfInteraction", m_time = 0.0 * Gaudi::Units::s ) ;

    //possible to choose between 1=nucl-ex/0601019 and 0=hep-ph/0604145;
    //default is 0; model 1 is strongly discouraged
    declareProperty("TheoryModel", m_model = 0 ) ;
}

//===========================================================================
// Destructor
//===========================================================================
Cosmics::~Cosmics() { }

//===========================================================================
// Initialize Particle Gun parameters
//===========================================================================
StatusCode Cosmics::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return sc ;

  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_flatGenerator.initialize( randSvc , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() )
    return Error( "Cannot initialize flat generator" ) ;

  // Get the mass of the particle to be generated
  LHCb::IParticlePropertySvc* ppSvc =
    svc< LHCb::IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;

  // check momentum and angles
  if (
       ( ( m_minMom   > m_maxMom   ) ||
         ( m_minTheta > m_maxTheta ) ||
         ( m_minPhi   > m_maxPhi   ) ) )
    return Error( "Incorrect values for momentum, theta or phi!" ) ;

  // setup particle information
  m_masses.clear();

  info() << "Particle type chosen randomly from :";
  PIDs::const_iterator icode ;
  for ( icode = m_pdgCodes.begin(); icode != m_pdgCodes.end(); ++icode ) {
    const LHCb::ParticleProperty * particle = ppSvc->find( LHCb::ParticleID( *icode ) );
    m_masses.push_back( ( particle->mass() ) ) ;
    m_names.push_back( particle->particle() ) ;
    info() << " " << particle->particle() ;
  }

  info() << endmsg ;
  // printout vertex information
  info() << "Interaction region : ( "
         << m_minxvtx / Gaudi::Units::mm << " mm < x < " << m_maxxvtx / Gaudi::Units::mm << " mm"
         << ", " << m_minyvtx / Gaudi::Units::mm << " mm < y < " << m_maxyvtx / Gaudi::Units::mm << " mm"
         << ", " << " z = " << m_zvtx / Gaudi::Units::mm << " mm"
         << " )" << endmsg ;

  info() << "Momentum range: " << m_minMom / Gaudi::Units::GeV << " GeV <-> "
         << m_maxMom / Gaudi::Units::GeV << " GeV" << endmsg ;
  info() << "Zenith angle range: " << m_minTheta / Gaudi::Units::rad << " rad <-> "
         << m_maxTheta / Gaudi::Units::rad << " rad" << endmsg ;
  info() << "Phi range: " << m_minPhi / Gaudi::Units::rad << " rad <-> "
         << m_maxPhi / Gaudi::Units::rad << " rad" << endmsg ;

  release( ppSvc ) ;

  return sc ;
}

//===========================================================================
// Generate the particles
//===========================================================================
void Cosmics::generateParticle( Gaudi::LorentzVector & fourMomentum , 
                                Gaudi::LorentzVector & origin , 
                                int & pdgId ) {
  double px(0.), py(0.), pz(0.) ;
  double verx(0.), very(0.), verz(0.) ;

  //GM
  //get the momentum according to the cosmic spectrum from
  //two different models
  //as a function of P=x (GeV) and theta'=y (in rads)

  double t = 0;
  double p = 0;
  double flux=0;
  double fluxMax=0.0;
  if(m_model==1) fluxMax=0.002;
  else if(m_model==0) fluxMax = 0.000005;
  //take into account integration over pi
  //GM: 29.01.09: the flux defined in muonSpec does not have the
  //factor 2pi.The fluxmax is the maximum of the function
  //in muonSpec
  //fluxMax*=Gaudi::Units::twopi;
  for(int i=0; i<1000000; i++) {
    //theta uniform in radians
    t = ( m_minTheta + m_flatGenerator()*(m_maxTheta-m_minTheta) )  ;
    //translate momentum in GeV
    p = ( m_minMom/Gaudi::Units::GeV + m_flatGenerator()*(m_maxMom-m_minMom)/Gaudi::Units::GeV )  ;
    //
    //This is the function I need to use for the generation of the events;
    //* m_model==1 => flux from nucl-ex/0601019, eq (1). take out the
    //             p^3 factor and add the sin(theta)
    //* m_model==0 => flux from D.Reyna (hep-ph 0604145), adding the sin(theta)
    //             factor; flux calculated through "sphere" of cross-section 1cm^2
    if ( m_model==1 ) {
      warning()<<" You chose to use model 1; the use of this model is discouraged. You do it at your own risk "<< endmsg;
      flux = sin(t)*(18.0*(p+5.0)*pow(p+2.7/cos(t),-2.7)) / ((p*cos(t)+145.0)*(p+5.0/cos(t))) ;
    } else if(m_model==0) {
      //depth in Meters Water Equivalent,
      //80 meters * density of the standard rock w.r.t density of water = 1 = 2.65 g/cm^3
      double depth = 80.0*2.65;
      double angle = 0.0;   //flat overburden
      flux = muonSpec(p, t, depth , angle);
    }

    if(flux>fluxMax ) warning() <<"Cosmic flux =" <<flux<<" > Max = "<<fluxMax<<endmsg;
    double temp = m_flatGenerator()*fluxMax ;
    //std::cout << " temp ="<<temp<<std::endl;
    if(temp < flux)    break;
  }

  //translate back into MeV
  p*=Gaudi::Units::GeV;
  //Momentum does not need to be changed.
  const double momentum = p;
  //Phi random between +pi and -pi;
  double phiprime      = m_minPhi   + m_flatGenerator() *
    (m_maxPhi-m_minPhi);
  //
  //the angle (t) is the zenith, angle of the particle with the vertical (theta');
  //I want to translate it into theta = angle with the beam in the LHCb system.
  //
  //Now have p, theta' and phi';
  //need ^x,^y,^z=versors in cartesian system;
  //
  verx=sin(t)*cos(phiprime);
  very=-1*cos(t);
  verz=sin(t)*sin(phiprime);
  /*
    expressing ^x,^y,^z as a function of the
    LHCb coordinates phi, theta (assuming
    theta positive from -y axis => vertical track->theta = zero,y=-1,
    and phi positive from x axis)
    verx=sin(theta)*cos(phi)
    very=sin(theta)*sin(phi)
    verz=cos(theta),
    from this I get theta and phi as a function of the versors
  */
  const double theta = acos(verz);
  const double phi   = atan2(very,verx);
  //
  debug() <<" CosmicsGun : "<<endmsg;
  debug() <<" p = "<<momentum/Gaudi::Units::GeV<<" GeV; "<< " Phi = "<< phi<< endmsg;
  debug() <<" Theta = "<<theta*180.0/Gaudi::Units::pi<<" degrees, "<<theta
          <<" rad"<<" ( from t = "<<t*180.0/Gaudi::Units::pi<<" "<<t <<")"<< endmsg;
  // Transform to x,y,z coordinates of the momentum
  const double pt = momentum*sin(theta);
  px              = pt*cos(phi);
  py              = pt*sin(phi);
  pz              = momentum*cos(theta);
  // randomly choose a particle type
  unsigned int currentType =
    (unsigned int)( m_pdgCodes.size() * m_flatGenerator() );
  // protect against funnies
  if ( currentType >= m_pdgCodes.size() ) currentType = 0;
          
  pdgId = m_pdgCodes[ currentType ] ;
  fourMomentum.SetPx( px ) ;
  fourMomentum.SetPy( py ) ;
  fourMomentum.SetPz( pz ) ; 
  fourMomentum.SetE( std::sqrt( m_masses[ currentType ] * m_masses[ currentType ] + 
                                fourMomentum.P2() ) ) ;        
    
  //
  //Generate random space point anywhere inside the detector
  //(as specified by job options )
  //z=z, -5<x<5m, -5<y<5m
  //
  double xprime = m_minxvtx + m_flatGenerator()*(m_maxxvtx - m_minxvtx);
  double yprime = m_minyvtx + m_flatGenerator()*(m_maxyvtx - m_minyvtx);
  double zprime = m_zvtx ;

  //GM: Add the time component; this is the time I want the cosmics
  //to be at the point in the detector.
  double tprime = m_time;
  //
  //now extrapolate at y=6meters (above the detector), assuming
  //cosmics always going down
  //
  const double y = 6.0 * Gaudi::Units::m ;//m_maxyvtx;
  const double x = xprime + (y-yprime)*(verx/very);
  const double z = zprime + (y-yprime)*(verz/very);
  double tofLight = sqrt( (xprime-x)*(xprime-x) +
                          (yprime-y)*(yprime-y) + (zprime-z)*(zprime-z) )/Gaudi::Units::c_light ;
  //get the time at the origin
  const double time = tprime - tofLight;
          
  origin.SetCoordinates( x , y , z , time ) ;
  
  debug() << " -> " << m_names[ currentType ] << endmsg
          << "   P   = " << fourMomentum << endmsg                             
          << "   Vtx = " << origin << endmsg ;
}

double Cosmics::muonSpec(double muMomentum, double Theta, double vDepth,
		double overAngle) {

/*************************************************************************
 This is a function which is based on the surface muon spectrum
 that was developed in hep-ph/0604145.  It will return the rate of muon
 events at a given depth as a function of muon momentum and the
 zenith angle.  Energy loss is calculated based on the geometric
 path length to the surface and an energy loss of 217 MeV / mwe
 which is consistent with "standard rock".  The input and output
 are defined as follows:
  Inputs:
      muMomentum  --  momentum of the detected muon in GeV
      cosTheta    --  cos of the zenith angle (no units)
      vDepth      --  The vertical depth directly over the detector in
                      meters of water equivalent (mwe)
      overAngle   --  Angle from flat (in degrees) of the overburden.
                      this yields a generic cone in which the peak of the
                      overburden is assumed to be directly over the
                      detector with height vDepth and base at detector
                      level
                             0 = a flat overburden
  Output:
      muonSpec    -- muon rate in Hz/cm2/sr/GeV 
      
 D. Reyna  14 November 2006

 Update--Giulia Manca: 29/01/2009: 
         Changed the output to give the rate on a vertical surface.
**************************************************************************/
  double zeta, Eloss, Alpha, ctAlpha;
  double delta = 3.38;
  double result = 0;
  double cosTheta = cos(Theta);

  if (muMomentum < 0) {
    error() << "Error: unphysical muon momentum: "<<muMomentum<<endmsg;
    return(0);
  }

  /* Calculate energy loss for generic overburden */
  Eloss = vDepth;                                //hemisphere
  if ((overAngle >= 0) && (overAngle < 90)) {    //generic cone (includes flat)
    Alpha = (90 - overAngle)*Gaudi::Units::pi/180.;
    ctAlpha = cos(Alpha)/sin(Alpha);
    Eloss /= cosTheta + ctAlpha*sqrt(1-pow(cosTheta,2));
  }
  /* assume 217 MeV loss per meter of water equivalent which is roughly true
     for standard rock */
  Eloss *= 0.217;


  /* calculate rate based on the starting momentum at the surface */
  zeta = cosTheta*(muMomentum + Eloss) + delta;

  /* adding extra factor sin(theta) from d(omega) to d(theta)*/
  /* adding extra factor 2pi from integration */
  /*
  */
  //Old function
  //  result = Gaudi::Units::twopi * sin(Theta) * pow(cosTheta,2.94)*0.14*pow(zeta,-2.7)*
  //          (1/(1 + 1.11*zeta/115) + 1/(1 + 1.11*zeta/850));
  //
  //Flux Through Vertical Surface
  //extra term (sinTheta)^2 cosphi; integral -pi to pi of (cosphi)=4;
  //  result = (1-cosTheta*cosTheta)*pow(cosTheta,2.94)*0.14*pow(zeta,-2.7)*
  //(1./(1. + 1.11*zeta/115.) + 1./(1. + 1.11*zeta/850.));
  result = 4.0 * sin(Theta) * sin(Theta) * pow(cosTheta,2.94)*0.14*pow(zeta,-2.7)*
              (1/(1 + 1.11*zeta/115) + 1/(1 + 1.11*zeta/850));

  return(result);
}


