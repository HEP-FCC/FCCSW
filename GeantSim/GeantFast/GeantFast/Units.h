#ifndef GEANT_UNITS_H
#define GEANT_UNITS_H

// Geant4
#include "G4SystemOfUnits.hh"

//EDM default units: cm and GeV
//Geant4 & CLHEP default units: mm and MeV
namespace edm2g4
{
   constexpr double length = CLHEP::cm/CLHEP::mm;
   constexpr double energy = CLHEP::GeV/CLHEP::MeV;
}
namespace g42edm
{
   constexpr double length = CLHEP::mm/CLHEP::cm;
   constexpr double energy = CLHEP::MeV/CLHEP::GeV;
}

#endif
