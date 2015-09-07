#ifndef GEANTFAST_SIMPLESMEAR_H
#define GEANTFAST_SIMPLESMEAR_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
//#include "GaudiKernel/RndmGenerators.h"

// FCCSW
#include "GeantFast/ISmearingTool.h"

class SimpleSmear: public GaudiTool, virtual public ISmearingTool
{
public:
   SimpleSmear(const std::string& type , const std::string& name,
               const IInterface* parent);
   virtual ~SimpleSmear();

   virtual StatusCode initialize();

   /// Implementation of ISmearingTool method that smears the momentum of the particle
   virtual StatusCode smearMomentum(G4ThreeVector& aMom);

   /// Implementation of ISmearingTool method that smears the energy of the particle
   virtual StatusCode smearEnergy(G4double& aEn);
private:
   /// Tmp: example resolution for the smearing
   double m_sigma;
   /// Flat random number generator
   //Rndm::Gauss m_gaussDist;
};

#endif
