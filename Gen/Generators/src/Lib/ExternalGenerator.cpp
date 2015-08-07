// $Id: ExternalGenerator.cpp,v 1.27 2009-12-03 15:32:49 robbep Exp $
// Include files

// local
#include "Generators/ExternalGenerator.h"

// Boost
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string/erase.hpp"
// Gaudi
#include "Event/IParticlePropertySvc.h"
#include "Event/ParticleProperty.h"
// Kernal
#include "MCInterfaces/IGenCutTool.h"
#include "MCInterfaces/IDecayTool.h"

// from Generators
#include "Generators/IProductionTool.h"
//TODO #include "Generators/LhaPdf.h"
#include "Generators/StringParse.h"
#include "Generators/ICounterLogFile.h"
#include "Event/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ExternalGenerator
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ExternalGenerator::ExternalGenerator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_productionTool( 0 ) ,
    m_decayTool( 0 ) ,
    m_cutTool  ( 0 ) ,
    m_xmlLogTool( 0 ) ,
    m_ppSvc    ( 0 ) {
    m_defaultLhaPdfSettings.clear() ;
    declareInterface< ISampleGenerationTool >( this ) ;
    declareProperty( "ProductionTool" ,
                     m_productionToolName = "PythiaProduction" ) ;
    declareProperty( "DecayTool" , m_decayToolName = "EvtGenDecay" ) ;
    declareProperty( "CutTool" , m_cutToolName = "LHCbAcceptance" ) ;
    declareProperty( "LhaPdfCommands" , m_userLhaPdfSettings ) ;
    declareProperty( "KeepOriginalProperties" , m_keepOriginalProperties =
                     false ) ;
    m_defaultLhaPdfSettings.push_back( "lhacontrol lhaparm 17 LHAPDF" ) ;
    m_defaultLhaPdfSettings.push_back( "lhacontrol lhaparm 16 NOSTAT" ) ;
  }

//=============================================================================
// Destructor
//=============================================================================
ExternalGenerator::~ExternalGenerator( ) { ; }

//=============================================================================
// Initialization method
//=============================================================================
StatusCode ExternalGenerator::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  if ( msgLevel( MSG::DEBUG ) ) {
    //TODO LhaPdf::lhacontrol().setlhaparm( 19 , "DEBUG" ) ;
  }
  else {
    //TODO LhaPdf::lhacontrol().setlhaparm( 19 , "SILENT" ) ;
  }

  // Set default LHAPDF parameters:
  //TODO  sc = parseLhaPdfCommands( m_defaultLhaPdfSettings ) ;
  // Set user LHAPDF parameters:
  // TODO sc = parseLhaPdfCommands( m_userLhaPdfSettings ) ;
  if ( ! sc.isSuccess() )
    return Error( "Unable to read LHAPDF commands" , sc ) ;

  // retrieve the particle property service
  m_ppSvc = svc< LHCb::IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;

  // obtain the Decay Tool
  // (ATTENTION: it has to be initialized before the production tool)
  if ( "" != m_decayToolName )
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;

  // obtain the Production Tool
  if ( "" != m_productionToolName )
    m_productionTool = tool< IProductionTool >( m_productionToolName , this ) ;

  // update the particle properties of the production tool
  if ( 0 != m_productionTool ) {
    LHCb::IParticlePropertySvc::iterator iter ;
    for ( iter = m_ppSvc -> begin() ; iter != m_ppSvc -> end() ; ++iter ) {
      if ( ( ! m_productionTool -> isSpecialParticle( *iter ) ) &&
           ( ! m_keepOriginalProperties ) )
        m_productionTool -> updateParticleProperties( *iter ) ;
      // set stable in the Production generator all particles known to the
      // decay tool
      if ( 0 != m_decayTool )
        if ( m_decayTool -> isKnownToDecayTool( (*iter)->pdgID().pid() ) )
          m_productionTool -> setStable( *iter ) ;
    }
  }

  // obtain the cut tool
  if ( "" != m_cutToolName )
    m_cutTool = tool< IGenCutTool >( m_cutToolName , this ) ;

  if ( 0 != m_productionTool )
    m_productionTool -> initializeGenerator();

  // obtain the log tool
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  // now debug printout of Production Tool
  // has to be after all initializations to be sure correct values are printed
  if ( 0 != m_productionTool ) {
    m_productionTool -> printRunningConditions( ) ;

    boost::char_separator<char> sep(".");
    boost::tokenizer< boost::char_separator<char> >
      strList( m_productionTool -> name() , sep ) ;


    std::string result = "" ;
    for ( boost::tokenizer< boost::char_separator<char> >::iterator
            tok_iter = strList.begin();
          tok_iter != strList.end(); ++tok_iter)
      result = (*tok_iter) ;
    m_hepMCName = boost::algorithm::ierase_last_copy( result , "Production" ) ;
  } else {
    m_hepMCName = "DecayAlone" ;
  }

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Decay heavy excited particles
//=============================================================================
StatusCode ExternalGenerator::decayHeavyParticles( HepMC::GenEvent * theEvent,
     const LHCb::ParticleID::Quark theQuark , const int signalPid ) const {
  StatusCode sc ;

  if ( 0 == m_decayTool ) return StatusCode::SUCCESS ;

  m_decayTool -> disableFlip() ;

  HepMCUtils::ParticleSet particleSet ;

  HepMC::GenEvent::particle_iterator it ;
  switch ( theQuark ) {

  case LHCb::ParticleID::bottom: // decay only B
    for ( it = theEvent -> particles_begin() ;
          it != theEvent -> particles_end() ; ++it )
      if ( LHCb::ParticleID( (*it) -> pdg_id() ).hasQuark( theQuark ) )
        particleSet.insert( *it ) ;
    break ;

  case LHCb::ParticleID::charm: // decay B + D
    for ( it = theEvent -> particles_begin() ;
          it != theEvent -> particles_end() ; ++it ) {
      LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
      if ( ( pid.hasQuark( theQuark ) ) ||
           ( pid.hasQuark( LHCb::ParticleID::bottom ) ) )
        particleSet.insert( *it ) ;
    }
    break ;

  default:

    if ( 15 == LHCb::ParticleID(signalPid).abspid() ) // tau ?
    {
      for ( it = theEvent -> particles_begin() ;
            it != theEvent -> particles_end() ; ++it ) {
        LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
        if ( ( pid.hasQuark( LHCb::ParticleID::charm  ) ) ||
             ( pid.hasQuark( LHCb::ParticleID::bottom ) ) )
          particleSet.insert( *it ) ;
      }
      break ;
    }
    else
    {
      // decay all what is heavier than the signal
      for ( it = theEvent -> particles_begin() ;
            it != theEvent -> particles_end() ; ++it ) {
        LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
        if ( (*it) -> generated_mass() >
             m_ppSvc -> find( LHCb::ParticleID( signalPid ) ) -> mass() )
          particleSet.insert( *it ) ;
        // if signal is KS then decay also K0
        else if ( ( signalPid == 310 ) && ( pid.abspid() == 311 ) )
          particleSet.insert( *it ) ;
      }
    }
    break ;
  }

  for ( HepMCUtils::ParticleSet::iterator itHeavy = particleSet.begin() ;
        itHeavy != particleSet.end() ; ++itHeavy )

    if ( ( LHCb::HepMCEvent::StableInProdGen == (*itHeavy) -> status() ) &&
         ( signalPid != abs( (*itHeavy) -> pdg_id() ) ) ) {

      if ( m_decayTool -> isKnownToDecayTool( (*itHeavy) -> pdg_id() ) ) {
        sc = m_decayTool -> generateDecayWithLimit( *itHeavy , signalPid ) ;
        if ( ! sc.isSuccess() ) return sc ;
      }
    }

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Check the presence of a particle of given type in event
// Attention : pidList must be sorted before begin used in this function
//=============================================================================
bool ExternalGenerator::checkPresence( const PIDs & pidList ,
                                       const HepMC::GenEvent * theEvent ,
                                       ParticleVector & particleList ) const {
  particleList.clear( ) ;
  HepMC::GenEvent::particle_const_iterator it ;
  for ( it = theEvent -> particles_begin() ;
        it != theEvent -> particles_end() ; ++it )
    if ( std::binary_search( pidList.begin() , pidList.end() ,
                             (*it) -> pdg_id() ) )
      if ( ( LHCb::HepMCEvent::DocumentationParticle != (*it) -> status() )
           && ( HepMCUtils::IsBAtProduction( *it ) ) )
        particleList.push_back( *it ) ;

  std::sort( particleList.begin() , particleList.end() ,
             HepMCUtils::compareHepMCParticles ) ;

  return ( ! particleList.empty() ) ;
}

//=============================================================================
// invert the event
//=============================================================================
void ExternalGenerator::revertEvent( HepMC::GenEvent * theEvent ) const {
  HepMC::GenEvent::vertex_iterator itv ;
  double x, y, z, t ;
  for ( itv = theEvent -> vertices_begin() ;
        itv != theEvent -> vertices_end() ; ++itv ) {
    x = (*itv) -> position().x() ;
    y = (*itv) -> position().y() ;
    z = (*itv) -> position().z() ;
    t = (*itv) -> position().t() ;
    (*itv) -> set_position( HepMC::FourVector( x, y, -z, t ) ) ;
  }

  HepMC::GenEvent::particle_iterator itp ;
  double px, py, pz, E ;
  for ( itp = theEvent -> particles_begin() ;
        itp != theEvent -> particles_end() ; ++itp ) {
    px = (*itp) -> momentum().px() ;
    py = (*itp) -> momentum().py() ;
    pz = (*itp) -> momentum().pz() ;
    E  = (*itp) -> momentum().e() ;
    (*itp) -> set_momentum( HepMC::FourVector( px, py, -pz, E ) ) ;
  }
}

//=============================================================================
// count the number of particles with pz > 0
//=============================================================================
unsigned int ExternalGenerator::nPositivePz( const ParticleVector
                                             & particleList ) const {
  ParticleVector::const_iterator iter ;
  unsigned int nP = 0 ;
  for ( iter = particleList.begin() ; iter != particleList.end() ; ++iter )
    if ( (*iter)->momentum().pz() > 0 ) nP++ ;

  return nP ;
}


//=============================================================================
// Set up event
//=============================================================================
void ExternalGenerator::prepareInteraction( LHCb::HepMCEvents * theEvents ,
    LHCb::GenCollisions * theCollisions , HepMC::GenEvent * & theGenEvent ,
    LHCb::GenCollision * & theGenCollision ) const {
  LHCb::HepMCEvent * theHepMCEvent = new LHCb::HepMCEvent( ) ;
  theHepMCEvent -> setGeneratorName( m_hepMCName ) ;
  theGenEvent = theHepMCEvent -> pGenEvt() ;

  theGenCollision = new LHCb::GenCollision() ;
  theGenCollision -> setEvent( theHepMCEvent ) ;
  theGenCollision -> setIsSignal( false ) ;

  theEvents -> insert( theHepMCEvent ) ;
  theCollisions -> insert( theGenCollision ) ;
}

//=============================================================================
// Parse LHAPDF commands stored in a vector
//=============================================================================
StatusCode ExternalGenerator::parseLhaPdfCommands( const CommandVector &
                                                   theCommandVector ) const {
  //
  // Parse Commands and Set Values from Properties Service...
  //
  CommandVector::const_iterator iter ;
  for ( iter = theCommandVector.begin() ; theCommandVector.end() != iter ;
        ++iter ) {
    debug() << " Command is: " << (*iter) << endmsg ;
    StringParse mystring( *iter ) ;
    std::string block = mystring.piece(1);
    std::string entry = mystring.piece(2);
    std::string str   = mystring.piece(4);
    int    int1  = mystring.intpiece(3);
    double fl1   = mystring.numpiece(3);

    // Note that Pythia needs doubles hence the convert here
    debug() << block << " block  " << entry << " item  " << int1
            << "  value " << fl1 << " str " << str << endmsg ;
    /*TODO
    if ( "lhacontrol" == block )
      if      ( "lhaparm" == entry )
	//TODO        LhaPdf::lhacontrol().setlhaparm( int1 , str ) ;
      else if ( "lhavalue" == entry )
        //TODO LhaPdf::lhacontrol().setlhavalue( int1 , fl1 ) ;
      else return Error( std::string( "LHAPDF ERROR, block LHACONTROL has " ) +
                         std::string( "LHAPARM and LHAVALUE: YOU HAVE " ) +
                         std::string( "SPECIFIED " ) + std::string( entry ) ) ;
    else return Error( std::string( " ERROR in LHAPDF PARAMETERS   " ) +
                       std::string( block ) +
                       std::string( " is and invalid common block name !" ) ) ;
    */
  }

  return StatusCode::SUCCESS ;
}
//=============================================================================
// Finalize method
//=============================================================================
StatusCode ExternalGenerator::finalize( ) {
  if ( 0 != m_decayTool ) release( m_decayTool ) ;
  if ( 0 != m_productionTool ) release( m_productionTool ) ;
  if ( 0 != m_cutTool ) release( m_cutTool ) ;
  if ( 0 != m_ppSvc ) release( m_ppSvc ) ;

  // set the name of the method
  m_xmlLogTool -> addMethod( this -> name() ) ;
  // set the name of the generator
  m_xmlLogTool -> addGenerator( m_hepMCName ) ;

  return GaudiTool::finalize() ;
}
