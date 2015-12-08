#ifndef SIMG4FAST_G4FASTIO_H
#define SIMG4FAST_G4FASTIO_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4IOTool.h"
#include "FWCore/DataHandle.h"

// albers
class ParticleCollection;
class ParticleMCAssociationCollection;

/** @class sim::G4FastIO SimG4Fast/G4FastIO.h G4FastIO.h
 *
 *  A tool defining inputs and outputs of Geant4 fast simulation.
 *
 *  @author Anna Zaborowska
 */

class G4FastIO: public GaudiTool, virtual public IG4IOTool {
public:
   G4FastIO(const std::string& aType , const std::string& aName,
               const IInterface* aParent);
   virtual ~G4FastIO();

   virtual StatusCode initialize();
   virtual StatusCode saveOutput();
private:
   /// Handle for the particles to be written
   DataHandle<ParticleCollection> m_recphandle;
   /// Handle for the associations between particles and MC particles to be written
   DataHandle<ParticleMCAssociationCollection> m_partassociationhandle;
   std::string m_tmp;
};

#endif
