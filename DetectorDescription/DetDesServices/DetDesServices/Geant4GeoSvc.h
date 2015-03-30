//
//  Geant4GeoSvc.h
//  
//
//  Created by Julia Hrdinka on 26/02/15.
//
//

#ifndef GEANT4GEOSVC_H
#define GEANT4GEOSVC_H

//Interface
#include "DetDesInterfaces/IGeant4GeoSvc.h"
//Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"
//DetectorConverter
#include "IConverter/IGeoConverterTool.h"
//DDG4
//#include "DDG4/Geant4DetectorConstruction.h"

class Geant4GeoSvc: public extends1<Service, IGeant4GeoSvc> {
    
public:
    //constructor
    Geant4GeoSvc(const std::string& name, ISvcLocator* svc);
    //destructor
    virtual ~Geant4GeoSvc();
    //initialize
    virtual StatusCode initialize();
    //finalize
    virtual StatusCode finalize();
    //build detector
    virtual StatusCode buildGeometry() override;
    //destroy detector
    virtual StatusCode destroyGeometry() override;
    //set the world volume - done bu RecoGeoConverterTool
    virtual void setDetector(std::shared_ptr<G4VUserDetectorConstruction> detector) override;
    //get the reconstruction geometry
    virtual G4VUserDetectorConstruction* getDetector() override;
    
private:
    //pointer to the detector construction of DDG4
    std::shared_ptr<G4VUserDetectorConstruction> m_detector;
    //ToolHandle to the reconstruction geometry conversion tool
    ToolHandle <IGeoConverterTool> m_g4Converter;
    //output
    MsgStream m_log;
    
};

#endif //GEANT4GEOSVC_H
