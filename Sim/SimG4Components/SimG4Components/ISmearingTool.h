 #ifndef GEANTFAST_ISMEARINGTOOL_H
#define GEANTFAST_ISMEARINGTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// CLHEP
namespace CLHEP{
   class Hep3Vector;
}

/**
   @brief     Interface to the smearing tool.
   @author    Anna Zaborowska
*/

static const InterfaceID IID_ISmearingTool("ISmearingTool",1,0);

class ISmearingTool : virtual public IAlgTool {
public:
   static const InterfaceID& interfaceID() { return IID_ISmearingTool; }

   /// Smear the momentum of the particle
   virtual StatusCode smearMomentum( CLHEP::Hep3Vector& aMom ) = 0;

   /// Smear the energy of the particle
   virtual StatusCode smearEnergy( double& aMom ) = 0;
};
#endif
