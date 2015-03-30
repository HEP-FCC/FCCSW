//
//  IDetConstructionSvc.h
//  
//
//  Created by Julia Hrdinka on 09/10/14.
//
//

#ifndef IDETDESSVC_H
#define IDETDESSVC_H


#include "GaudiKernel/IService.h"
//DD4Hep
#include "DD4hep/LCDD.h"

class GAUDI_API IDetDesSvc: virtual public IService {
    
public:
    /// InterfaceID
    DeclareInterfaceID(IDetDesSvc,1,0);
    
    virtual StatusCode buildDetector () = 0;
    
    virtual StatusCode destroyDetector () = 0;
    
    virtual DD4hep::Geometry::LCDD* lcdd () = 0;
    
    virtual ~IDetDesSvc() {}
    
};



#endif //IDETDESSVC_H
