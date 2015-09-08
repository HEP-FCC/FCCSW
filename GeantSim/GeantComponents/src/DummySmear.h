#ifndef GEANTFAST_DUMMMYSMEAR_H
#define GEANTFAST_DUMMYSMEAR_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "GeantComponents/ISmearingTool.h"

class DummySmear: public GaudiTool, virtual public ISmearingTool
{
public:
   DummySmear(const std::string& type , const std::string& name,
               const IInterface* parent);
   virtual ~DummySmear();

   virtual StatusCode initialize();

   /// Implementation of ISmearingTool method that smears the momentum of the particle
   virtual StatusCode smearMomentum(G4ThreeVector& aMom);

   /// Implementation of ISmearingTool method that smears the energy of the particle
   virtual StatusCode smearEnergy(G4double& aEn);
};

#endif
