#include "GeoConstruction.h"

#include <stdexcept>

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Plugins.h"
#include "DDG4/Geant4Converter.h"
#include "TGeoManager.h"

// Geant4
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"

namespace det {
  
GeoConstruction::GeoConstruction(dd4hep::Detector& lcdd) : m_lcdd(lcdd) {}

GeoConstruction::~GeoConstruction() {}

// method borrowed from dd4hep::sim::Geant4DetectorSensitivesConstruction
//                             ::constructSensitives(Geant4DetectorConstructionContext* ctxt)
void GeoConstruction::ConstructSDandField() {
      typedef std::set<const TGeoVolume*> VolSet;
      typedef std::map<dd4hep::SensitiveDetector, VolSet> _SV;
  dd4hep::sim::Geant4GeometryInfo* p = dd4hep::sim::Geant4Mapping::instance().ptr();
  _SV& vols = p->sensitives;

  for (_SV::const_iterator iv = vols.begin(); iv != vols.end(); ++iv) {
    dd4hep::SensitiveDetector sd = (*iv).first;
    std::string typ = sd.type(), nam = sd.name();
    // Sensitive detectors are deleted in ~G4SDManager
    G4VSensitiveDetector* g4sd = dd4hep::PluginService::Create<G4VSensitiveDetector*>(typ, nam, &m_lcdd);
    if (g4sd == nullptr) {
      std::string tmp = typ;
      tmp[0] = ::toupper(tmp[0]);
      typ = "Geant4" + tmp;
      g4sd = dd4hep::PluginService::Create<G4VSensitiveDetector*>(typ, nam, &m_lcdd);
      if (g4sd == nullptr) {
        dd4hep::PluginDebug dbg;
        g4sd = dd4hep::PluginService::Create<G4VSensitiveDetector*>(typ, nam, &m_lcdd);
        if (g4sd == nullptr) {
          throw std::runtime_error("ConstructSDandField: FATAL Failed to "
                                   "create Geant4 sensitive detector " +
                                   nam + " of type " + typ + ".");
        }
      }
    }
    g4sd->Activate(true);
    G4SDManager::GetSDMpointer()->AddNewDetector(g4sd);
    const VolSet& sens_vols = (*iv).second;
    for (VolSet::const_iterator i = sens_vols.begin(); i != sens_vols.end(); ++i) {
      const TGeoVolume* vol = *i;
      G4LogicalVolume* g4v = p->g4Volumes[vol];
      if (g4v == nullptr) {
        throw std::runtime_error("ConstructSDandField: Failed to access G4LogicalVolume for SD " + nam + " of type " +
                                 typ + ".");
      }
      G4SDManager::GetSDMpointer()->AddNewDetector(g4sd);
      g4v->SetSensitiveDetector(g4sd);
    }
  }
}

// method borrowed from dd4hep::sim::Geant4DetectorConstruction::Construct()
G4VPhysicalVolume* GeoConstruction::Construct() {
  dd4hep::sim::Geant4Mapping& g4map = dd4hep::sim::Geant4Mapping::instance();
  dd4hep::DetElement world = m_lcdd.world();
  dd4hep::sim::Geant4Converter conv(m_lcdd, dd4hep::DEBUG);
  dd4hep::sim::Geant4GeometryInfo* geo_info = conv.create(world).detach();
  g4map.attach(geo_info);
  // All volumes are deleted in ~G4PhysicalVolumeStore()
  G4VPhysicalVolume* m_world = geo_info->world();
  m_lcdd.apply("DD4hepVolumeManager", 0, 0);
  // Create Geant4 volume manager
  g4map.volumeManager();
  return m_world;
}
}
