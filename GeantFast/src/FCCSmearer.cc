#include "FCCSmearer.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include <ctime>

FCCSmearer* FCCSmearer::fFCCSmearer = nullptr;

FCCSmearer::FCCSmearer()
{
   unsigned seed = time(NULL);
   fRandomEngine = new CLHEP::HepJamesRandom( seed );
   fRandomGauss = new CLHEP::RandGauss(fRandomEngine);
}
FCCSmearer::~FCCSmearer()
{}

FCCSmearer* FCCSmearer::Instance()
{
   if(!fFCCSmearer)
   {
      fFCCSmearer = new FCCSmearer();
   }
   return fFCCSmearer;
}

G4ThreeVector FCCSmearer::SmearMomentum(const G4Track* aTrackOriginal, G4double aResolution)
{
   return SmearGaussian(aTrackOriginal, aResolution);
}

G4double FCCSmearer::SmearEnergy(const G4Track* aTrackOriginal, G4double aResolution)
{
   G4double newE = -1;
   while (newE < 0) // to ensure the resulting value is not negative (vital for energy smearing, does not change direction for momentum smearing)
   {
      if(aResolution != -1)
         newE = aTrackOriginal->GetKineticEnergy() * Gauss(1, aResolution);
      else
         newE = aTrackOriginal->GetKineticEnergy();
   }
   return newE;
}

G4ThreeVector FCCSmearer::SmearGaussian(const G4Track* aTrackOriginal, G4double aResolution)
{
   G4ThreeVector originP = aTrackOriginal->GetMomentum();
   G4ThreeVector originPos = aTrackOriginal->GetPosition();
   G4double rdm = Gauss(1,aResolution);
   G4ThreeVector smearedMom (originP.x()*rdm,  originP.y()*rdm,  originP.z()*rdm);
   return smearedMom;
}

G4double FCCSmearer::Gauss(G4double aMean, G4double aStandardDeviation)
{
   return fRandomGauss->fire(aMean, aStandardDeviation);
}

