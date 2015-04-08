#include "Smearer.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include <ctime>

Smearer* Smearer::fSmearer = nullptr;

Smearer::Smearer()
{
   unsigned seed = time(NULL);
   fRandomEngine = new CLHEP::HepJamesRandom( seed );
   fRandomGauss = new CLHEP::RandGauss(fRandomEngine);
}
Smearer::~Smearer()
{
   if(fElectronManager) delete fElectronManager;
   if(fPionManager) delete fPionManager;
   if(fMuonManager) delete fMuonManager;
}

Smearer* Smearer::Instance()
{
   if(!fSmearer)
   {
      fSmearer = new Smearer();
      fSmearer->MakeManagers();
   }
   return fSmearer;
}

void Smearer::MakeManagers()
{
   //Create mangers for smeaing muons, pions and electrons
   fMuonManager = new Atlfast::MuonMatrixManager("../data/Atlfast_MuonResParam_CSC.dat",time(NULL));
   fPionManager = new Atlfast::PionMatrixManager("../data/Atlfast_PionResParam_DC1_NewUnits.dat",time(NULL));
   fElectronManager = new Atlfast::ElectronMatrixManager("../data/Atlfast_ElectronResParam_CSC.dat",time(NULL));
}

G4ThreeVector Smearer::SmearMomentum(const G4Track* aTrackOriginal, G4double aResolution)
{
   if(aResolution != -1)
   {
      return SmearGaussian(aTrackOriginal, aResolution);
   }
   else
   {
      return SmearPerigee(aTrackOriginal);
   }
}

G4double Smearer::SmearEnergy(const G4Track* aTrackOriginal, G4double aResolution)
{
   G4double newE = -1;
   while (newE < 0) // to ensure the resulting value is not negative (vital for energy smearing, does not change direction for momentum smearing)
   {
      if(aResolution != -1)
         newE = aTrackOriginal->GetKineticEnergy() * Gauss(1, aResolution);
      else
      {
         G4int PID = aTrackOriginal->GetDynamicParticle()->GetPDGcode();
         if( abs(PID) == 11 || abs(PID) == 13 || abs(PID) == 211)
         {
            G4ThreeVector mom = SmearPerigee(aTrackOriginal);
            newE = sqrt(mom.mag2()+pow(aTrackOriginal->GetDynamicParticle()->GetDefinition()->GetPDGMass(),2) )-aTrackOriginal->GetDynamicParticle()->GetDefinition()->GetPDGMass();
         }
         else
            newE =  aTrackOriginal->GetKineticEnergy();
      }
   }
   return newE;
}

G4ThreeVector Smearer::SmearGaussian(const G4Track* aTrackOriginal, G4double aResolution)
{
   G4ThreeVector originP = aTrackOriginal->GetMomentum();
   G4ThreeVector originPos = aTrackOriginal->GetPosition();
   G4double rdm = Gauss(1,aResolution);
   G4ThreeVector smearedMom (originP.x()*rdm,  originP.y()*rdm,  originP.z()*rdm);
   return smearedMom;
}

G4double Smearer::Gauss(G4double aMean, G4double aStandardDeviation)
{
   return fRandomGauss->fire(aMean, aStandardDeviation);
}


G4ThreeVector Smearer::SmearPerigee(const G4Track* aTrackOriginal)
{
   G4double* params = Atlfast(aTrackOriginal);
   G4ThreeVector smearedMom = fSmearer->ComputeMomFromParams(params);
   delete params;
   G4ThreeVector orgMom = aTrackOriginal->GetMomentum();
   return smearedMom;
}

G4double* Smearer::Atlfast(const G4Track* aTrackOriginal)
{
   G4ThreeVector originP = aTrackOriginal->GetVertexMomentumDirection()
      * sqrt( aTrackOriginal->GetVertexKineticEnergy() * aTrackOriginal->GetVertexKineticEnergy()
              + 2*  aTrackOriginal->GetVertexKineticEnergy() *  aTrackOriginal->GetDefinition()->GetPDGMass() ) ;
   double originCharge = aTrackOriginal->GetDynamicParticle()->GetCharge();
   G4ThreeVector originPos = aTrackOriginal->GetVertexPosition();
   G4double* originParams;
   originParams = ComputeTrackParams(originCharge, originP, originPos);

   CLHEP::HepSymMatrix sigma; // smear matrix for track
   std::vector<double> smearVariables;   // Vector of correlated gaussian variables
   G4int PID = aTrackOriginal->GetDynamicParticle()->GetPDGcode();

   if(abs(PID) == 11)
      smearVariables = fElectronManager->getVariables( *aTrackOriginal, sigma );
   else if(abs(PID) == 13)
      smearVariables = fMuonManager->getVariables( *aTrackOriginal, sigma );
   else if(abs(PID) == 211)
      smearVariables = fPionManager->getVariables( *aTrackOriginal, sigma );
   else return originParams;

   // Atlfast smeared variables
   double impactParameter = originParams[0] + smearVariables[0];
   double zPerigee = originParams[1] + smearVariables[1];
   double Phi = CheckPhi(originParams[2] + smearVariables[2]);
   double cotTheta = originParams[3] + smearVariables[3] ;
   double invPtCharge = originParams[4] + smearVariables[4];

   G4double* params = new G4double[5];
   params[0] = impactParameter;
   params[1] = zPerigee;
   params[2] = Phi;
   params[3] = cotTheta;
   params[4] = invPtCharge;
   return params;
}

G4double* Smearer::ComputeTrackParams(G4double aCharge, G4ThreeVector aVertexMomentum, G4ThreeVector aVertexPosition)
{
   // calculation of track parameter representation: implemented in Atlfast::TrackTrajectory
   double pt = aVertexMomentum.perp();
   double q  = aCharge;
   q /= abs(q); //  |q| = 1

   G4double bField =
      ((G4UniformMagField*)(G4TransportationManager::GetTransportationManager()->GetFieldManager()->GetDetectorField())) ->GetConstantFieldValue().mag();

   // Working in MeV and mm
   double radius = pt/(2*0.149898*bField);
   double x0  = aVertexPosition.x() - q* radius*sin(aVertexMomentum.phi());
   double y0  = aVertexPosition.y() + q* radius*cos(aVertexMomentum.phi());
   G4ThreeVector hCentre(x0,y0);

   double impactParameter = -q*(hCentre.perp() - radius);
   double theta = aVertexMomentum.theta();
   double cotTheta = 1./tan(theta);
   /*double eta = aVertexMomentum.pseudoRapidity();*/
   double zPerigee;
   double phi;

   phi = CheckPhi(hCentre.phi() + M_PI/2.);

   zPerigee = aVertexPosition.z() + cotTheta*radius*(phi-aVertexMomentum.phi());

   G4double* params = new G4double[5];
   params[0] = impactParameter;
   params[1] = zPerigee;
   params[2] = phi;
   params[3] = cotTheta;
   params[4] = q/pt;
   return params;
}

G4ThreeVector Smearer::ComputePosFromParams(G4double* aParams, G4double aPhiVertex)
{
   G4double x,y,z;
   G4double bField =
      ((G4UniformMagField*)(G4TransportationManager::GetTransportationManager()->GetFieldManager()->GetDetectorField())) ->GetConstantFieldValue().mag();
   double radius = 1./(2*0.149898*bField*aParams[4]);
   if(radius<0) radius = -1* radius;
   double q = (aParams[4] > 0) - (aParams[4] < 0); // returns signum(charge)

   // helix centre
   double xC = ( aParams[0] - q * radius) *sin(aParams[2]);
   double yC = ( q * aParams[0] + q * radius) *cos(aParams[2]);

   // vertex point
   x = xC + q * radius * sin(aPhiVertex);
   y = yC - q * radius * cos(aPhiVertex);
   z = aParams[1] + aParams[3]*radius*(aParams[2]-aPhiVertex);

   G4ThreeVector pos (x,y,z);
   return pos;
}

G4ThreeVector Smearer::ComputeMomFromParams(G4double* aParams)
{
   double Px = (-1)*1./aParams[4]*cos(aParams[2]);
   double Py = (-1)*1./aParams[4]*sin(aParams[2]);
   double q = (aParams[4] > 0) - (aParams[4] < 0); // returns signum(charge)
   double Pz = q/aParams[4]*aParams[3];
   G4ThreeVector P (Px,Py,Pz);
   return P;
}

G4double Smearer::CheckPhi(G4double aPhi)
{
   if(aPhi<-M_PI)
      while(aPhi<-M_PI)
         aPhi+=2*M_PI;
   else if(aPhi>M_PI)
      while(aPhi>M_PI)
         aPhi-=2*M_PI;
   return aPhi;
}
