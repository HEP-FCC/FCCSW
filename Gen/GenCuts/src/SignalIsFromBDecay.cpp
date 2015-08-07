// $Id: SignalIsFromBDecay.cpp,v 1.2 2007-11-26 13:51:23 jonrob Exp $
// Include files

// local
#include "SignalIsFromBDecay.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// Kernel
#include "Event/ParticleID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalIsFromBDecay
//
// 22/11/2007 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SignalIsFromBDecay )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SignalIsFromBDecay::SignalIsFromBDecay( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool  ( type, name , parent )
{
  // interface
  declareInterface< IGenCutTool >( this ) ;
}

//=============================================================================
// Destructor
//=============================================================================
SignalIsFromBDecay::~SignalIsFromBDecay( ) { }

//=============================================================================
// Acceptance function
//=============================================================================
bool SignalIsFromBDecay::applyCut( ParticleVector & theParticleVector ,
                                   const HepMC::GenEvent * theEvent,
                                   const LHCb::GenCollision * /* theHardInfo */ )
  const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Passed " << theParticleVector.size() << " Particles" << endreq;

  // Apply from B cuts
  bool fromB = false;
  for ( ParticleVector::iterator it = theParticleVector.begin();
        it != theParticleVector.end(); ++it )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << " -> Particle PDG Code = " << (*it)->pdg_id() << endreq;
    }
    fromB = isFromB(*it);
    if ( fromB ) break;
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " -> Particle is from B = " << fromB << endreq;
  }

  // print the event record in verbose mode
  if ( msgLevel(MSG::VERBOSE) ) { theEvent->print(); }

  return fromB;
}

//=============================================================================
// method to determine if a given particle came from a b at some point
//=============================================================================
bool SignalIsFromBDecay::isFromB( const HepMC::GenParticle * part,
                                  unsigned int tree_level ) const
{
  if ( !part ) return false;
  if ( tree_level > 999 ) { Warning("Recursion Limit Reached !"); return false; }

  // Is this particle itself a b hadron ?
  if ( LHCb::ParticleID(part->pdg_id()).hasBottom() ) return true;

  // production vertex
  const HepMC::GenVertex * vert = part->production_vertex();
  if ( !vert ) return false;

  // loop over parents
  for ( HepMC::GenVertex::particles_in_const_iterator it = vert->particles_in_const_begin();
        it != vert->particles_in_const_end(); ++it )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << std::string(tree_level,' ')
              << " -> Particle PDG Code = " << (*it)->pdg_id() << endreq;
    }
    if ( isFromB(*it,tree_level+1) ) return true;
  }

  // if get here, not from a b
  return false;
}
