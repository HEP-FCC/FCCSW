//
//  IRecoGeoSvc.h
//  
//
//  Created by Julia Hrdinka on 31/03/15.
//
//

#ifndef IRECOGEOSVC_H
#define IRECOGEOSVC_H

#include "GaudiKernel/IService.h"
//RecoGeometry
#include "RecoGeometry/ContainerVolume.h"

class GAUDI_API IRecoGeoSvc: virtual public IService {

public:
    //InterfaceID
    DeclareInterfaceID(IRecoGeoSvc,1,0);
    //receive Reconstruction geometry
    virtual const Reco::ContainerVolume* getRecoGeo() = 0;
    virtual StatusCode buildGeometry() = 0;
    virtual ~IRecoGeoSvc() {}
};


#endif //IRECOGEOSVC_H
