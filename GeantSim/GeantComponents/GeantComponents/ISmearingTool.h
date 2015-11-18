#ifndef GEANTFAST_ISMEARINGTOOL_H
#define GEANTFAST_ISMEARINGTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Geant4
#include "G4ThreeVector.hh"

/**
   @brief     Interface to the smearing tool.
   @author    Anna Zaborowska
*/

static const InterfaceID IID_ISmearingTool("ISmearingTool",1,0);

class ISmearingTool : virtual public IAlgTool {
public:
   static const InterfaceID& interfaceID() { return IID_ISmearingTool; }

   /// Smear the momentum of the particle
   virtual StatusCode smearMomentum( G4ThreeVector& aMom ) = 0;

   /// Smear the energy of the particle
   virtual StatusCode smearEnergy( double& aMom ) = 0;
};
#endif
