//
//  Geant4GeoConverterTool.h
//  
//
//  Created by Julia Hrdinka on 10/11/14.
//
//

#ifndef GEANT4GEOCONVERTERTOOL_H
#define GEANT4GEOCONVERTERTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "IConverter/IGeoConverterTool.h"
//DetDesServices
#include "DetDesServices/DD4HepDetDesSvc.h"
#include "DetDesInterfaces/IGeant4GeoSvc.h"
//DDG4
#include "DDG4/Geant4DetectorConstruction.h"
//Geant4
#include "G4RunManager.hh"

class Geant4GeoConverterTool : public AlgTool, virtual public IGeoConverterTool {
public:
    Geant4GeoConverterTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~Geant4GeoConverterTool (){}
    
    //  virtual StatusCode initialize();
    //  virtual StatusCode finalize ();
   
    virtual StatusCode convert();
    
    //output
    MsgStream               m_log;
    //Service for retrieving the DD4Hep Geometry
    IDetDesSvc*             m_DD4HepSvc;
    //Service to hand over the world volume of the reconstruction geometry
    IGeant4GeoSvc*           m_g4GeoSvc;
};

#endif // GEANT4GEOCONVERTERTOOL_H
