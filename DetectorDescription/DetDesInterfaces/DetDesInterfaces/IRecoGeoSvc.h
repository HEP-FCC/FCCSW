//
//  IRecoGeoSvc.h
//  
//
//  Created by Julia Hrdinka on 25/02/15.
//
//

#ifndef IRECOGEOSVC_H
#define IRECOGEOSVC_H

#include "GaudiKernel/IService.h"
#include "RecoGeometry/ContainerVolume.h"

class GAUDI_API IRecoGeoSvc: virtual public IService {
    
public:

    /// InterfaceID
    DeclareInterfaceID(IRecoGeoSvc,1,0);
    
    virtual StatusCode buildGeometry () = 0;
    
    virtual StatusCode destroyGeometry () = 0;
    
    virtual void setWorldVolume(std::shared_ptr<const Reco::ContainerVolume> worldVolume) = 0;
    
    virtual const Reco::ContainerVolume* getWorldVolume() = 0;
    
    virtual ~IRecoGeoSvc() {}
    
};

#endif //IRECOGEOSVC_H
