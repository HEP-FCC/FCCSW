
#include "ParticleGunAlg.h"

#include "boost/tokenizer.hpp"

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/RndmGenerators.h"

#include "IParticleGunTool.h"
#include "IVertexSmearingTool.h"
#include "HepMC/GenEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleGun
//
// 2008-05-18 : Patrick Robbe
// 2014-06-23 : Benedikt Hegner (simplification for Gaudi)
//-----------------------------------------------------------------------------

/// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ParticleGunAlg )

/// Standard constructor, initializes variables
ParticleGunAlg::ParticleGunAlg( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) ,
    m_particleGunTool       ( nullptr ),
    m_vertexSmearTool		( nullptr ) {
  // Generation Method
  declareProperty ( "ParticleGunTool" ,
                    m_particleGunToolName = "GenericGun" ) ;
  declareProperty ( "VertexSmearingTool" ,
                     m_vertexSmearingToolName = "" ) ;
  declareOutput("hepmc", m_hepmchandle);
}

/// Destructor
ParticleGunAlg::~ParticleGunAlg() {}

/// Initialisation. Check parameters
StatusCode ParticleGunAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize( ) ; // Initialize base class
  if ( sc.isFailure() ) return sc ;
  debug() << "==> Initialise" << endmsg ;
  
  // Retrieve generation method tool
  if ( "" == m_particleGunToolName )
    return Error( "No ParticleGun Generation Tool is defined. This is mandatory" ) ;
  
  m_particleGunTool = tool< IParticleGunTool >( m_particleGunToolName , this ) ;
  
  // Retrieve smearing tool, if required
  if ( "" != m_vertexSmearingToolName )
    m_vertexSmearTool = tool< IVertexSmearingTool >( m_vertexSmearingToolName , this ) ;
  
  // Retrieve
  
  return StatusCode::SUCCESS;
}

/// Main execution
StatusCode ParticleGunAlg::execute() {
  StatusCode sc = StatusCode::SUCCESS ;
  Gaudi::LorentzVector theFourMomentum ;
  Gaudi::LorentzVector origin ;
  int thePdgId ;
  
  // prepare a new HepMC event
  HepMC::GenEvent * theEvent = new HepMC::GenEvent( HepMC::Units::GEV, HepMC::Units::CM) ;
  
  m_particleGunTool->generateParticle( theFourMomentum , origin , thePdgId );
  
  // create HepMC Vertex
  HepMC::GenVertex * v = new HepMC::GenVertex( HepMC::FourVector( origin.X() ,
								  origin.Y() ,
								  origin.Z() ,
								  origin.T() ) ) ;
  // create HepMC particle
  HepMC::GenParticle * p = new HepMC::GenParticle( HepMC::FourVector( theFourMomentum.Px() ,
								      theFourMomentum.Py() ,
								      theFourMomentum.Pz() ,
								      theFourMomentum.E()  ) ,
						   thePdgId ,
						   3 ) ;
  
  v -> add_particle_out( p ) ;
    
  theEvent -> add_vertex( v ) ;
  theEvent -> set_signal_process_id( 0 ) ;
  theEvent -> set_signal_process_vertex( v ) ;

  if(m_vertexSmearTool != nullptr)
    m_vertexSmearTool->smearVertex(theEvent);
  
  m_hepmchandle.put(theEvent);
  return sc ;
}

///  Finalize
StatusCode ParticleGunAlg::finalize() {
  if ( 0 != m_particleGunTool ) release( m_particleGunTool ) ;
  if ( 0 != m_vertexSmearTool ) release( m_vertexSmearTool ) ;
  
  return GaudiAlgorithm::finalize( ) ;
}
