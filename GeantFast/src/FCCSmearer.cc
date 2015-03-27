#include "FCCSmearer.hh"
#include "FCCPrimaryParticleInformation.hh"
#include "G4PrimaryParticle.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include <ctime>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
FCCSmearer* FCCSmearer::fFCCSmearer = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
FCCSmearer::FCCSmearer()
{
   // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
   unsigned seed = time(NULL);
   fRandomEngine = new CLHEP::HepJamesRandom( seed );
   fRandomGauss = new CLHEP::RandGauss(fRandomEngine);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
FCCSmearer::~FCCSmearer()
{
   if(fElectronManager) delete fElectronManager;
   if(fPionManager) delete fPionManager;
   if(fMuonManager) delete fMuonManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
FCCSmearer* FCCSmearer::Instance()
{
   if(!fFCCSmearer)
   {
      fFCCSmearer = new FCCSmearer();
      fFCCSmearer->MakeManagers();
   }
   return fFCCSmearer;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void FCCSmearer::MakeManagers()
{
   //Create mangers for smeaing muons, pions and electrons
   fMuonManager = new Atlfast::MuonMatrixManager("data/Atlfast_MuonResParam_CSC.dat",time(NULL));
   fPionManager = new Atlfast::PionMatrixManager("data/Atlfast_PionResParam_DC1_NewUnits.dat",time(NULL));
   fElectronManager = new Atlfast::ElectronMatrixManager("data/Atlfast_ElectronResParam_CSC.dat","data/Atlfast_ElectronBremParam_CSC.dat",time(NULL));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector FCCSmearer::SmearMomentum(const G4Track* aTrackOriginal, G4double aResolution, FCCOutput::SaveType aSave)
{
   if(aResolution != -1)
   {
      return SmearGaussian(aTrackOriginal, aResolution);
   }
   else
   {
      return SmearPerigee(aTrackOriginal, aSave);
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double FCCSmearer::SmearEnergy(const G4Track* aTrackOriginal, G4double aResolution, FCCOutput::SaveType aSave)
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
            G4ThreeVector mom = SmearPerigee(aTrackOriginal, aSave);
            newE = sqrt(mom.mag2()+pow(aTrackOriginal->GetDynamicParticle()->GetDefinition()->GetPDGMass(),2) )-aTrackOriginal->GetDynamicParticle()->GetDefinition()->GetPDGMass();
         }
         else
            newE =  aTrackOriginal->GetKineticEnergy();
      }
   }
   return newE;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector FCCSmearer::SmearGaussian(const G4Track* aTrackOriginal, G4double aResolution)
{
   G4ThreeVector originP = aTrackOriginal->GetMomentum();
   G4ThreeVector originPos = aTrackOriginal->GetPosition();
   G4double rdm = Gauss(1,aResolution);
   G4ThreeVector smearedMom (originP.x()*rdm,  originP.y()*rdm,  originP.z()*rdm);
   return smearedMom;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double FCCSmearer::Gauss(G4double aMean, G4double aStandardDeviation)
{
   return fRandomGauss->fire(aMean, aStandardDeviation);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector FCCSmearer::SmearPerigee(const G4Track* aTrackOriginal, FCCOutput::SaveType aSave)
{
   G4double* params = Atlfast(aTrackOriginal);
   G4ThreeVector smearedMom =FCCSmearer::Instance()->ComputeMomFromParams(params);
   G4ThreeVector orgMom = aTrackOriginal->GetMomentum();
   switch(aSave)
   {
   case FCCOutput::eNoSave:
   case FCCOutput::eSaveMC:
      break;
   case FCCOutput::eSaveTracker:
   {
      ((FCCPrimaryParticleInformation*)aTrackOriginal->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation()) -> SetPerigeeTracker(params);
      break;
   }
   case FCCOutput::eSaveEMCal:
   {
      ((FCCPrimaryParticleInformation*)aTrackOriginal->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation()) -> SetPerigeeEMCal(params);
      break;
   }
   case FCCOutput::eSaveHCal:
   {
      ((FCCPrimaryParticleInformation*)aTrackOriginal->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation()) -> SetPerigeeHCal(params);
      break;
   }
   }
   return smearedMom;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double* FCCSmearer::Atlfast(const G4Track* aTrackOriginal)
{
   // original track position, momentum and charge
   // G4ThreeVector originP = aTrackOriginal->GetMomentum();
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
   double impactParameter = originParams[0] + smearVariables[0]; // [0]
   double zPerigee = originParams[1] + smearVariables[1]; //[1]
   double Phi = CheckPhi(originParams[2] + smearVariables[2]); //[2]
   double cotTheta = originParams[3] + smearVariables[3] ; //[3]
   double invPtCharge = originParams[4] + smearVariables[4]; // q/pT where q = q/|q| (just sign) //[4]

   G4double* params = new G4double[5];
   params[0] = impactParameter;
   params[1] = zPerigee;
   params[2] = Phi;
   params[3] = cotTheta;
   params[4] = invPtCharge;
   return params;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4double* FCCSmearer::ComputeTrackParams(G4double aCharge, G4ThreeVector aVertexMomentum, G4ThreeVector aVertexPosition)
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

   // calculate parameters
   double impactParameter = -q*(hCentre.perp() - radius);
   double theta = aVertexMomentum.theta();
   double cotTheta = 1./tan(theta);
   /*double eta = aVertexMomentum.pseudoRapidity();*/
   double zPerigee;
   double phi;

   //  Calculate Phi
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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector FCCSmearer::ComputePosFromParams(G4double* aParams, G4double aPhiVertex)
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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector FCCSmearer::ComputeMomFromParams(G4double* aParams)
{
   double Px = (-1)*1./aParams[4]*cos(aParams[2]);
   double Py = (-1)*1./aParams[4]*sin(aParams[2]);
   double q = (aParams[4] > 0) - (aParams[4] < 0); // returns signum(charge)
   double Pz = q/aParams[4]*aParams[3];
   G4ThreeVector P (Px,Py,Pz);
   return P;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double FCCSmearer::CheckPhi(G4double aPhi)
{
   if(aPhi<-M_PI)
      while(aPhi<-M_PI)
         aPhi+=2*M_PI;
   else if(aPhi>M_PI)
      while(aPhi>M_PI)
         aPhi-=2*M_PI;
   return aPhi;
}
