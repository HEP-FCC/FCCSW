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
#include "DetInterface/IGeoSvc.h"

//Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

//DD4Hep
#include "DD4hep/LCDD.h"

//Geant4
#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"

class GeoSvc: public extends1<Service, IGeoSvc> {

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
    //receive DD4hep Geometry
    virtual DD4hep::Geometry::DetElement getDD4HepGeo() override;
    virtual DD4hep::Geometry::LCDD* lcdd() override;
    //receive Geant4 Geometry
    virtual G4VUserDetectorConstruction* getGeant4Geo() override;

private:
    /// Pointer to the interface to the DD4hep geometry
    DD4hep::Geometry::LCDD* m_dd4hepgeo;
    /// Pointer to the detector construction of DDG4
    std::shared_ptr<G4VUserDetectorConstruction> m_geant4geo;
    /// XML-files with the detector description
    std::vector<std::string>   m_xmlFileNames;
    //output
    MsgStream m_log;
};

#endif //GEOSVC_H
