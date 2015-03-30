//
//  IGeant4GeoSvc.h
//  
//
//  Created by Julia Hrdinka on 26/02/15.
//
//

#ifndef IGEANT4GEOSVC_H
#define IGEANT4GEOSVC_H

#include "GaudiKernel/IService.h"
//Geant4
#include "G4RunManager.hh"

class GAUDI_API IGeant4GeoSvc: virtual public IService {
    
public:
    
    /// InterfaceID
    DeclareInterfaceID(IGeant4GeoSvc,1,0);
    
    virtual StatusCode buildGeometry () = 0;
    
    virtual StatusCode destroyGeometry () = 0;
    
    virtual void setDetector(std::shared_ptr<G4VUserDetectorConstruction> detector) = 0;
    
    virtual G4VUserDetectorConstruction* getDetector() = 0;
    
    virtual ~IGeant4GeoSvc() {}
    
};

#endif //IGEANT4GEOSVC_H
