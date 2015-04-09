//
//  IGeoSvc.h
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#ifndef IGEOSVC_H
#define IGEOSVC_H


#include "GaudiKernel/IService.h"
//DD4Hep
//#include "DD4hep/LCDD.h"
//Geant4
// #include "G4RunManager.hh"


namespace DD4hep {
    namespace Geometry {
        class LCDD;
        class DetElement;
    }
}

class G4VUserDetectorConstruction;

class GAUDI_API IGeoSvc: virtual public IService {
    
public:
    /// InterfaceID
    DeclareInterfaceID(IGeoSvc,1,0);
    //receive DD4hep Geometry
    virtual DD4hep::Geometry::DetElement getDD4HepGeo() = 0;
    virtual DD4hep::Geometry::LCDD* lcdd() = 0;
    //receive Geant4 Geometry
    virtual G4VUserDetectorConstruction* getGeant4Geo() = 0;
    
    virtual ~IGeoSvc() {}
    
};

#endif //IGEOSVC_H
