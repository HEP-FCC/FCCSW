//
//  GeoSvc.h
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#ifndef GEOSVC_H
#define GEOSVC_H

//Interface
#include "DetDesInterfaces/IGeoSvc.h"
//Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
/*#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/DataObjectHandle.h"*/
//DD4Hep
#include "DD4hep/LCDD.h"
//DDG4
//Geant4
#include "G4RunManager.hh"
#include "DDG4/Geant4DetectorConstruction.h"

class GeoSvc: public extends1<Service, IGeoSvc> {

public:
    
    GeoSvc(const std::string& name, ISvcLocator* svc);
    
    virtual ~GeoSvc();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    StatusCode buildDD4HepGeo();
    StatusCode buildGeant4Geo();
    //receive DD4hep Geometry
    virtual DD4hep::Geometry::DetElement getDD4HepGeo() override;
    virtual DD4hep::Geometry::LCDD* lcdd() override;
    //receive Geant4 Geometry
    virtual G4VUserDetectorConstruction* getGeant4Geo() override;
    
public:
    //pointer to the interface to the DD4hep geometry
    DD4hep::Geometry::LCDD* m_dd4hepgeo;
    //pointer to the detector construction of DDG4
    std::shared_ptr<G4VUserDetectorConstruction> m_geant4geo;
    //xml-file with the detector description
    std::string   m_xmlFileName;
    //output
    MsgStream m_log;
};

#endif //GEOSVC_H
