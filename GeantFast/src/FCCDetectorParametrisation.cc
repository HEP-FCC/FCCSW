#include "FCCDetectorParametrisation.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

FCCDetectorParametrisation::FCCDetectorParametrisation(){}
FCCDetectorParametrisation::~FCCDetectorParametrisation(){}

G4double FCCDetectorParametrisation::GetResolution(Detector aDetector, Parametrisation aParam, G4double aMomentum)
{
   aMomentum /= GeV; // to make sure momentum's unit is GeV
   G4double res = 1;
   if(aParam == eCMS)
   {
      switch(aDetector)
      {
      case FCCDetectorParametrisation::eTRACKER:
         res = 0.013;
         break;
      case FCCDetectorParametrisation::eEMCAL:
         res = sqrt(pow(0.03/sqrt(aMomentum),2) // stochastic
                    + pow(0.12/aMomentum, 2)    // noise
                    + pow(0.003,2));      // constant
         break;
      case FCCDetectorParametrisation::eHCAL:
         res = sqrt(pow(1.1/sqrt(aMomentum),2) // stochastic
                    + pow(0.09,2));      // constant
         break;
      }
   }
   else if(aParam == eATLAS)
   {
      switch(aDetector)
      {
      case FCCDetectorParametrisation::eTRACKER:
         res = 0.01;
         break;
      case FCCDetectorParametrisation::eEMCAL:
         res = sqrt(pow(0.1/sqrt(aMomentum),2) // stochastic
                    + pow(0.0017,2));      // constant
         break;
      case FCCDetectorParametrisation::eHCAL:
         res = sqrt(pow(0.55/sqrt(aMomentum),2) // stochastic
                    + pow(0.06,2));      // constant
         break;
      }
   }
   else if(aParam == eALEPH)
   {
      switch(aDetector)
      {
      case FCCDetectorParametrisation::eTRACKER:
         res = 0.01;
         break;
      case FCCDetectorParametrisation::eEMCAL:
         res = sqrt(pow(0.18/sqrt(aMomentum),2) // stochastic
                    + pow(0.009,2));      // constant
         break;
      case FCCDetectorParametrisation::eHCAL:
         res = 0.85/sqrt(aMomentum); // stochastic
         break;
      }
   }
   else if(aParam == eATLFAST)
   {
      res = -1;
   }
   return res;
}

G4double FCCDetectorParametrisation::GetEfficiency(Detector aDetector, Parametrisation /*aParam*/, G4double /*aMomentum*/)
{
   G4double eff = 1;
   switch(aDetector)
   {
   case FCCDetectorParametrisation::eTRACKER:
      eff = 1;
      break;
   case FCCDetectorParametrisation::eEMCAL:
      eff = 1;
      break;
   case FCCDetectorParametrisation::eHCAL:
      eff = 1;
      break;
   }
   return eff;
}
