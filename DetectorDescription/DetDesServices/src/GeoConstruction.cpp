// $Id: Geant4DetectorConstruction.cpp 1822 2015-07-12 02:24:02Z markus.frank@cern.ch $
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

// Framework include files
#include "GeoConstruction.h"
#include "DDG4/Geant4HierarchyDump.h"
#include "DDG4/Geant4Converter.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/LCDD.h"
#include "TGeoManager.h"

// Geant4 include files
#include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4VSensitiveDetector.hh"
#include "G4VUserDetectorConstruction.hh"

// C/C++ include files
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

namespace det {
GeoConstruction::GeoConstruction(Geometry::LCDD& lcdd)
  : m_lcdd(lcdd), m_world(nullptr) {}

GeoConstruction::~GeoConstruction() {}

void GeoConstruction::ConstructSDandField() {
  typedef Geometry::GeoHandlerTypes::SensitiveVolumes _SV;
  typedef Geometry::GeoHandlerTypes::ConstVolumeSet VolSet;
  Geant4GeometryInfo* p = Geant4Mapping::instance().ptr();
  _SV& vols = p->sensitives;

  for(_SV::const_iterator iv=vols.begin(); iv != vols.end(); ++iv)  {
    Geometry::SensitiveDetector sd = (*iv).first;
    string typ = sd.type(), nam = sd.name();
    G4VSensitiveDetector* g4sd = 
      PluginService::Create<G4VSensitiveDetector*>(typ, nam, &m_lcdd);
    if (!g4sd) {
      string tmp = typ;
      tmp[0] = ::toupper(tmp[0]);
      typ = "Geant4" + tmp;
      g4sd = PluginService::Create<G4VSensitiveDetector*>(typ, nam, &m_lcdd);
      if ( !g4sd ) {
        PluginDebug dbg;
        g4sd = PluginService::Create<G4VSensitiveDetector*>(typ, nam, &m_lcdd);
        if ( !g4sd )  {
          throw runtime_error("ConstructSDandField: FATAL Failed to "
                              "create Geant4 sensitive detector " + nam + 
                              " of type " + typ + ".");
        }
      }
    }
    g4sd->Activate(true);
    G4SDManager::GetSDMpointer()->AddNewDetector(g4sd);
    const VolSet& sens_vols = (*iv).second;
    for(VolSet::const_iterator i=sens_vols.begin(); i!= sens_vols.end(); ++i)   {
      const TGeoVolume* vol = *i;
      G4LogicalVolume* g4v = p->g4Volumes[vol];
      if ( !g4v )  {
        throw runtime_error("ConstructSDandField: Failed to access G4LogicalVolume for SD "+
                            nam + " of type " + typ + ".");
      }
      G4SDManager::GetSDMpointer()->AddNewDetector(g4sd);
      g4v->SetSensitiveDetector(g4sd);
    }
  }
  printout(INFO,"Geant4Converter", "++ Handled %ld sensitive detectors.",vols.size());
}

G4VPhysicalVolume* GeoConstruction::Construct() {
  Geant4Mapping& g4map = Geant4Mapping::instance();
  Geometry::DetElement world = m_lcdd.world();
  Geant4Converter conv(m_lcdd, INFO);
  Geant4GeometryInfo* geo_info = conv.create(world).detach();
  g4map.attach(geo_info);
  m_world = geo_info->world();
  m_lcdd.apply("DD4hepVolumeManager", 0, 0);
  // Create Geant4 volume manager
  g4map.volumeManager();
  return m_world;
}
}
