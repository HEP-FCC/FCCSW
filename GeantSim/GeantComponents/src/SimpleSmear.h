#ifndef GEANTFAST_SIMPLESMEAR_H
#define GEANTFAST_SIMPLESMEAR_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/ITHistSvc.h"

// ROOT
#include "TH1F.h"

// FCCSW
#include "GeantComponents/ISmearingTool.h"

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
   /// Constant resolution for the smearing (set by job options)
   double m_sigma;
   /// Bool: make control histogams or not (set by job options)
   bool m_hist;
   /// Histogram Service
   ITHistSvc* m_tHistSvc;
   /// Histogram for relative difference in smeared momentum
   TH1F* m_hSmMom;
   /// Histogram for relative difference in smeared energy
   TH1F* m_hSmEn;
   /// Random Number Service
   IRndmGenSvc* m_randSvc;
   /// Gaussian random number generator used for smearing with a constant resolution (m_sigma)
   Rndm::Numbers m_gauss;
};

#endif
