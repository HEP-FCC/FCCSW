#ifndef GEANT_FAST_SIMPLESMEAR_H
#define GEANT_FAST_SIMPLESMEAR_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
class IRndmGenSvc;

// FCCSW
#include "GeantComponents/ISmearingTool.h"

/**
   @brief     Simple smearing tool.
   @details   Simple smearing tool. It smears either momentum or energy with Gaussian which std dev is set in job option file.
   @author    Anna Zaborowska
*/

class SimpleSmear: public GaudiTool, virtual public ISmearingTool {
public:
   SimpleSmear(const std::string& type , const std::string& name,
               const IInterface* parent);
   virtual ~SimpleSmear();

   virtual StatusCode initialize();

   /// Implementation of ISmearingTool method that smears the momentum of the particle
   virtual StatusCode smearMomentum(CLHEP::Hep3Vector& aMom);

   /// Implementation of ISmearingTool method that smears the energy of the particle
   virtual StatusCode smearEnergy(double& aEn);
private:
   /// Constant resolution for the smearing (set by job options)
   double m_sigma;
   /// Random Number Service
   IRndmGenSvc* m_randSvc;
   /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
   Rndm::Numbers m_gauss;
};

#endif
