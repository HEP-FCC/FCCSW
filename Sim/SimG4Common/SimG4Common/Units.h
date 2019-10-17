#ifndef SIMG4COMMON_UNITS_H
#define SIMG4COMMON_UNITS_H

// Geant4
#include "G4SystemOfUnits.hh"

/** Conversion between units.
 *
 *  Contains conversions between default units used by EDM and by Geant.
 *  Default units in FCC-EDM are (as initially discussed on FCCSoftware Meeting on 14/10/15) GeV and mm.
 *  Geant4 default units are MeV and mm.
 *
 *  @author Anna Zaborowska
 */

namespace sim {
namespace edmdefault {
// FIXME: these should be a constexpr, but CLHEP is only const
const double length = CLHEP::mm;
const double energy = CLHEP::GeV;
}
namespace edm2g4 {
// FIXME: these should be a constexpr, but CLHEP is only const
const double length = edmdefault::length / CLHEP::mm;
const double energy = edmdefault::energy / CLHEP::MeV;
}
namespace g42edm {
// FIXME: these should be a constexpr, but CLHEP is only const
const double length = CLHEP::mm / edmdefault::length;
const double energy = CLHEP::MeV / edmdefault::energy;
}
namespace papas2edm {
// FIXME: these should be a constexpr, but CLHEP is only const
const double length = CLHEP::m / edmdefault::length;
const double energy = CLHEP::GeV / edmdefault::energy;
}
namespace edm2papas {
// FIXME: these should be a constexpr, but CLHEP is only const
const double length = edmdefault::length / CLHEP::m;
const double energy = edmdefault::energy / CLHEP::GeV;
}
}
#endif /* SIMG4COMMON_UNITS_H */
