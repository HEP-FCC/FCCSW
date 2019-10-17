#ifndef GENERATION_UNITS_H
#define GENERATION_UNITS_H

#include "GaudiKernel/SystemOfUnits.h"
#include "HepMC/Units.h"

// FIXME: All these should be a constexpr, but in CLHEP / HepMC they are only const

namespace gen {
/// Default EDM units for the Generation package
namespace edmdefault {
const HepMC::Units::LengthUnit length = HepMC::Units::MM;
const HepMC::Units::MomentumUnit energy = HepMC::Units::GEV;
}
/// Default HepMC units
namespace hepmcdefault {
const HepMC::Units::LengthUnit length = HepMC::Units::MM;
const HepMC::Units::MomentumUnit energy = HepMC::Units::GEV;
}
/// Conversion factors from EDM to HepMC
namespace edm2hepmc {
const double length = HepMC::Units::conversion_factor(edmdefault::length, hepmcdefault::length);
const double energy = HepMC::Units::conversion_factor(edmdefault::energy, hepmcdefault::energy);
}
/// Conversion factors from HepMC to EDM
namespace hepmc2edm {
const double length = HepMC::Units::conversion_factor(hepmcdefault::length, edmdefault::length);
const double energy = HepMC::Units::conversion_factor(hepmcdefault::energy, edmdefault::energy);
}
}

#endif /*GENERATION_UNITS_H*/
