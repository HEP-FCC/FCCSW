//
//  GeoSvc.h
//
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#ifndef GEOSVC_H
#define GEOSVC_H

// Interface
#include "DetInterface/IGeoSvc.h"

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"

// DD4Hep
#include "DD4hep/LCDD.h"

// Geant4
#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"

class GeoSvc : public extends2<Service, IGeoSvc, IIncidentListener> {

public:
  /// Default constructor
  GeoSvc(const std::string& name, ISvcLocator* svc);

  /// Destructor
  virtual ~GeoSvc();
  /// Initialize function
  virtual StatusCode initialize() final;
  /// Finalize function
  virtual StatusCode finalize() final;
  /// This function generates the DD4hep geometry
  StatusCode buildDD4HepGeo();
  /// This function generates the Geant4 geometry
  StatusCode buildGeant4Geo();
  // receive DD4hep Geometry
  virtual DD4hep::Geometry::DetElement getDD4HepGeo() override;
  virtual DD4hep::Geometry::LCDD* lcdd() override;
  // receive Geant4 Geometry
  virtual G4VUserDetectorConstruction* getGeant4Geo() override;
  /// Inform that a new incident has occurred
  virtual void handle(const Incident& inc) final;

private:
  /// Pointer to the incident service
  ServiceHandle<IIncidentSvc> m_incidentSvc;
  /// Pointer to the interface to the DD4hep geometry
  DD4hep::Geometry::LCDD* m_dd4hepgeo;
  /// Pointer to the detector construction of DDG4
  std::shared_ptr<G4VUserDetectorConstruction> m_geant4geo;
  /// XML-files with the detector description
  Gaudi::Property<std::vector<std::string>> m_xmlFileNames{this, "detectors", {}, "Detector descriptions XML-files"};
  // output
  MsgStream m_log;
  // Flag set to true if any incident is fired from geometry constructors
  bool m_failureFlag;
};

#endif  // GEOSVC_H
