// $Id: GeoConstruction.h 1822 2015-07-12 02:24:02Z markus.frank@cern.ch $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_GEOCONSTRUCTION_H
#define DD4HEP_GEOCONSTRUCTION_H

// Framework include files
#include "DDG4/Geant4GeometryInfo.h"

// Geant4 include files
#include "G4VUserDetectorConstruction.hh"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  // Forward declarations
  namespace Geometry {
    class LCDD;
  }
}
/// Class to create Geant4 detector geometry from TGeo representation in memory
/**
 *  On demand (ie. when calling "Construct") the DD4hep geometry is converted
 *  to Geant4 with all volumes, assemblies, shapes, materials etc.
 *  The actuak work is performed by the Geant4Converter class called by this method.
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_SIMULATION
 */
class GeoConstruction : public G4VUserDetectorConstruction {
  public:
  /// Initializing constructor for other clients
  GeoConstruction(DD4hep::Geometry::LCDD& lcdd);
  /// Default destructor
  virtual ~GeoConstruction();
  /// Geometry construction callback: Invoke the conversion to Geant4
  G4VPhysicalVolume* Construct();
  private:
  void constructSensitives();

  /// Reference to geometry object
  DD4hep::Geometry::LCDD& m_lcdd;
  /// Reference to the world after construction
  G4VPhysicalVolume* m_world;
};

#endif
