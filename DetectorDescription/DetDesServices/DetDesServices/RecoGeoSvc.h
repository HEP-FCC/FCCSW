//
//  RecoGeoSvc.h
//  
//
//  Created by Julia Hrdinka on 24/02/15.
//
//

#ifndef RECOGEOSVC_H
#define RECOGEOSVC_H

//Interface
#include "DetDesInterfaces/IRecoGeoSvc.h"
//Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"
//DetectorConverter
#include "IConverter/IGeoConverterTool.h"


class RecoGeoSvc: public extends1<Service, IRecoGeoSvc> {
    
public:

    //constructor
    RecoGeoSvc(const std::string& name, ISvcLocator* svc);
    //destructor
    virtual ~RecoGeoSvc();
    //initialize
    virtual StatusCode initialize();
    //finalize
    virtual StatusCode finalize();
    //build detector
    virtual StatusCode buildGeometry() override;
    //destroy detector
    virtual StatusCode destroyGeometry() override;
    //set the world volume - done bu RecoGeoConverterTool
    virtual void setWorldVolume(std::shared_ptr<const Reco::ContainerVolume> worldVolume) override;
    //get the reconstruction geometry
    const virtual Reco::ContainerVolume* getWorldVolume() override;
    
private:
    //pointer to the world volume of the reconstruction geometry
    std::shared_ptr<const Reco::ContainerVolume> m_worldVolume;
    //ToolHandle to the reconstruction geometry conversion tool
    ToolHandle <IGeoConverterTool> m_recoConverter;
    //output
    MsgStream m_log;
    
};


#endif //RECOGEOSVC_H
