
#include "ParticleGunAlg.h"

#include "boost/tokenizer.hpp"

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/RndmGenerators.h"

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
  : GaudiAlgorithm ( name , pSvcLocator ) {
  // Generation Method
  declareProperty ( "ParticleGunTool" ,
                    m_particleGunTool ) ;
  declareProperty ( "VertexSmearingToolPGun" ,
                     m_vertexSmearingTool ) ;
  declarePrivateTool(m_vertexSmearingTool, "FlatSmearVertex/VertexSmearingToolPGun");
  declarePrivateTool(m_particleGunTool, "MomentumRangeParticleGun/ParticleGunTool");
  declareOutput("hepmc", m_hepmchandle);
}

/// Destructor
ParticleGunAlg::~ParticleGunAlg() {}

/// Initialisation. Check parameters
StatusCode ParticleGunAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize( ) ; // Initialize base class
  if ( sc.isFailure() ) return sc ;
  debug() << "==> Initialise" << endmsg ;
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
						   1 ) ; // hepmc status code for final state particle
  
  v -> add_particle_out( p ) ;
    
  theEvent -> add_vertex( v ) ;
  theEvent -> set_signal_process_id( 0 ) ;
  theEvent -> set_signal_process_vertex( v ) ;

  m_vertexSmearingTool->smearVertex(theEvent);
  
  m_hepmchandle.put(theEvent);
  return sc;
}

StatusCode ParticleGunAlg::finalize() {
  return GaudiAlgorithm::finalize();
}
