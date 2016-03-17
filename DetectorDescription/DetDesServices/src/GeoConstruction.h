#ifndef DETDESSERVICES_GEOCONSTRUCTION_H
#define DETDESSERVICES_GEOCONSTRUCTION_H

// DD4hep
#include "DDG4/Geant4GeometryInfo.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  // Forward declarations
  namespace Geometry {
    class LCDD;
  }
}
/// Class to create Geant4 detector geometry from TGeo representation
/**
 *  On demand (ie. when calling "Construct") the DD4hep geometry is converted
 *  to Geant4 with all volumes, assemblies, shapes, materials etc.
 *  The actual work is performed by the Geant4Converter class called by this method.
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_SIMULATION
 */
namespace det {
class GeoConstruction : public G4VUserDetectorConstruction {
public:
  /// Initializing constructor for other clients
  GeoConstruction(DD4hep::Geometry::LCDD& lcdd);
  /// Default destructor
  virtual ~GeoConstruction();
  /// Geometry construction callback: Invoke the conversion to Geant4
  virtual G4VPhysicalVolume* Construct() final;
  /// Construct SD
  virtual void ConstructSDandField() final;
private:
  /// Reference to geometry object
  DD4hep::Geometry::LCDD& m_lcdd;
  /// Reference to the world after construction
  G4VPhysicalVolume* m_world;
};
}
#endif
