#ifndef DETDESSERVICES_GEOCONSTRUCTION_H
#define DETDESSERVICES_GEOCONSTRUCTION_H

// DD4hep
#include "DDG4/Geant4GeometryInfo.h"

// Geant4
#include "G4VUserDetectorConstruction.hh"

namespace DD4hep {
namespace Geometry {
class LCDD;
}
}
/** @class GeoConstruction DetectorDescription/DetDesServices/src/GeoConstruction.h GeoConstruction.h
 *
 *  Class to create Geant4 detector geometry from TGeo representation
 *  On demand (ie. when calling "Construct") the DD4hep geometry is converted
 *  to Geant4 with all volumes, assemblies, shapes, materials etc.
 *
 *  @author Markus Frank
 *  @author Anna Zaborowska
 */

namespace det {
class GeoConstruction : public G4VUserDetectorConstruction {
public:
  /// Constructor
  GeoConstruction(DD4hep::Geometry::LCDD& lcdd);
  /// Default destructor
  virtual ~GeoConstruction();
  /// Geometry construction callback: Invoke the conversion to Geant4
  /// All volumes (including world) are deleted in ~G4PhysicalVolumeStore()
  virtual G4VPhysicalVolume* Construct() final;
  /// Construct SD
  virtual void ConstructSDandField() final;

private:
  /// Reference to geometry object
  DD4hep::Geometry::LCDD& m_lcdd;
};
}
#endif /* DETDESSERVICES_GEOCONSTRUCTION_H */
