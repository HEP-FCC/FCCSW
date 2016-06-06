//
//  IMagFieldSvc.h
//  
//
//  Created by Julia Hrdinka on 30/06/15.
//
//

#ifndef IMAGFIELDSVC_H
#define IMAGFIELDSVC_H

#include "GaudiKernel/IService.h"

class GAUDI_API IMagFieldSvc : virtual public IService {

public:
    //InterfaceID
    DeclareInterfaceID(IMagFieldSvc,1,0);
    virtual void getField(const double* xyz, double* bxyz, double* deriv = 0) = 0;
 //   virtual void getField(const Alg::Vector3D *xyz, Alg::Vector3D* bxyz, Alg::RotationMatrix3D *deriv = 0) = 0;

    virtual ~IMagFielSvc() {}
    
};

#endif //IMAGFIELDSVC_H
