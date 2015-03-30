//
//  DD4HepDetDesSvc.h
//  
//
//  Created by Julia Hrdinka on 13/10/14.
//
//

#ifndef DD4HEPDETDESSVC_H
#define DD4HEPDETDESSVC_H

//Interface
#include "DetDesInterfaces/IDetDesSvc.h"
//Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/DataObjectHandle.h"
//DD4Hep
#include "DD4hep/LCDD.h"

class DD4HepDetDesSvc: public extends2<Service, IDetDesSvc, IIncidentListener> {

 public:
    //constructor - loads the xml-file and declares the converter tools specified in the job options
    DD4HepDetDesSvc (const std::string& name, ISvcLocator* svc);
    //destructor - calls destroyDetector()
    virtual ~DD4HepDetDesSvc ();
    //initialize - retrieves the DD4Hep Geometry, calls buildDetector(), fires an Incident that the DD4hep geometry is build and retrieves the converter tools
    virtual StatusCode initialize ();
    //finalize
    virtual StatusCode finalize ();
    //inform that new incident has occurred
    void handle(const Incident&);
    //loads the geometry from the xml-file, invokes detector building in DD4Hep and calls the converter tools
    virtual StatusCode buildDetector ();
    //destroys the DD4Hep geometry
    virtual StatusCode destroyDetector ();
    //returns pointer to the interface to the DD4Hep geometry
    virtual DD4hep::Geometry::LCDD* lcdd() override;
 
protected:

    DD4hep::Geometry::LCDD* m_lcdd;
    //xml-file with the detector description
    std::string   m_xmlFileName;
    //output
    MsgStream m_log;

};


#endif //DD4HEPDETDESSVC_H
