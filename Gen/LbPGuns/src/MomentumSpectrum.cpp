
// This class
#include "MomentumSpectrum.h"

// From STL
#include <cmath>

// FromGaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/IParticlePropertySvc.h"
#include "Event/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "TMath.h"
#include "TRandom3.h"
#include "Event/GenHeader.h"


//===========================================================================
// Implementation file for class: MomentumSpectrum
//
// 2014-01-03: Dan Johnson (adapted from Michel De Cian)
//===========================================================================

DECLARE_TOOL_FACTORY( MomentumSpectrum )

//===========================================================================
// Constructor
//===========================================================================
MomentumSpectrum::MomentumSpectrum( const std::string & type ,
                              const std::string & name ,
                              const IInterface * parent )
  : GaudiTool( type , name, parent ) {
    declareInterface< IParticleGunTool >( this ) ;

    declareProperty("PdgCodes", m_pdgCodes);
    declareProperty("InputFile", m_inputFileName);
    declareProperty("HistogramPath", m_histoPath);
    declareProperty("BinningVariables", m_binningVars);
}

//===========================================================================
// Destructor
//===========================================================================
MomentumSpectrum::~MomentumSpectrum() {;}

//===========================================================================
// Initialize Particle Gun parameters
//===========================================================================
StatusCode MomentumSpectrum::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return sc ;

  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_flatGenerator.initialize( randSvc , Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize flat generator" ) ;

  // Get the mass of the particle to be generated
  LHCb::IParticlePropertySvc* ppSvc =
    svc< LHCb::IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;

  // setup particle information
  m_masses.clear();

  info() << "Particle type chosen randomly from :";
  PIDs::iterator icode ;
  for ( icode = m_pdgCodes.begin(); icode != m_pdgCodes.end(); ++icode ) {
    const LHCb::ParticleProperty * particle =
      ppSvc->find( LHCb::ParticleID( *icode ) ) ;
    m_masses.push_back( ( particle->mass() ) ) ;
    m_names.push_back( particle->particle() ) ;
    info() << " " << particle->particle() ;
  }
  info() << endmsg ;

  release( ppSvc ) ;

  // -- Open the file containing the spectrum
  TFile *file = TFile::Open( m_inputFileName.c_str(), "READ" );
  if( !file ){
    error() << "Could not find spectrum template file!" << endmsg;
    return StatusCode::FAILURE;
  }

  // -- Get the histogram template file for the particle momentum spectrum
  m_hist = (TH1*) file->Get( m_histoPath.c_str() );
  if( !m_hist ){
    error() << "Could not find spectrum histogram!" << endmsg;
    return StatusCode::FAILURE;
  }

  // -- Check the binning format specified
  // Make sure accept an empty binningVars variable for backwards-compatability;
  if(m_binningVars=="")	m_binningVars="pxpypz";

  if(m_binningVars=="pxpypz") {
    if( m_hist->GetDimension()!=3 ) {
      error() << "BinningVariables set to " << m_binningVars
              << " but histogram " << m_histoPath
              << " has dimension " << m_hist->GetDimension()
              << endmsg;
      return StatusCode::FAILURE;
    }
    m_hist2d = 0;
    m_hist3d = (TH3D*) m_hist;
  } else if (m_binningVars!="ptpz" || m_binningVars!="pteta") {
    if( m_hist->GetDimension()!=2 ) {
      error() << "BinningVariables set to " << m_binningVars
              << " but histogram " << m_histoPath
              << " has dimension " << m_hist->GetDimension()
              << endmsg;
      return StatusCode::FAILURE;
    }
    m_hist2d = (TH2D*) m_hist;
    m_hist3d = 0;
  } else {
    error() << "BinningVariables set to unrecognised value: " << m_binningVars
            << ". Only \"pxpypz\" (default), \"ptpz\" or \"pteta\" allowed)" << endmsg;
    return StatusCode::FAILURE;
  }

  info() << "Using file '" << m_inputFileName << "' with histogram '"
         << m_histoPath << "' for sampling the momentum spectrum" << endmsg;

  return sc;
}

//===========================================================================
// Generate the particles
//===========================================================================
void MomentumSpectrum::generateParticle( Gaudi::LorentzVector & momentum ,
					 Gaudi::LorentzVector & origin , int & pdgId ) {

	// -- Determine which particle is generated
	unsigned int currentType = (unsigned int)( m_pdgCodes.size() * m_flatGenerator() );
	// protect against funnies
	if ( currentType >= m_pdgCodes.size() ) currentType = 0;
	pdgId = m_pdgCodes[ currentType ] ;

	// -- Set origin to (0,0,0,0)
	origin.SetCoordinates( 0. , 0. , 0. , 0. ) ;

	// -- Sample components of momentum according to template in histogram
	LHCb::GenHeader* evt =  get<LHCb::GenHeader>(  LHCb::GenHeaderLocation::Default );
	gRandom->SetSeed(evt->runNumber() * evt->evtNumber());
	if ( m_binningVars == "pxpypz" ) {
		double px(0), py(0), pz(0);
		m_hist3d->GetRandom3(px, py, pz);
		momentum.SetPx( px );
		momentum.SetPy( py );
		momentum.SetPz( pz );
	}
	else if( m_binningVars == "ptpz" ) {
		double pt(0.), pz(0.);
		m_hist2d->GetRandom2(pt , pz);
		double phi = (-1.*Gaudi::Units::pi + m_flatGenerator() * Gaudi::Units::twopi) * Gaudi::Units::rad;
		momentum.SetPx( pt*cos(phi) );
		momentum.SetPy( pt*sin(phi) );
		momentum.SetPz( pz );
	}
	else if( m_binningVars == "pteta" ) {
		double pt(0.), eta(0.);
		m_hist2d->GetRandom2(pt , eta);
		double phi = (-1.*Gaudi::Units::pi + m_flatGenerator() * Gaudi::Units::twopi) * Gaudi::Units::rad;
		momentum.SetPx( pt*cos(phi)  );
		momentum.SetPy( pt*sin(phi)  );
		momentum.SetPz( pt*sinh(eta) );
	} else
		error() << "This should never happen!" << endmsg;

	momentum.SetE( std::sqrt( m_masses[currentType] * m_masses[currentType] + momentum.P2() ) ) ;

	if (msgLevel(MSG::DEBUG))
		debug() << " -> " << m_names[ currentType ] << endmsg
	           << "   P   = " << momentum << endmsg ;
}

