//
//  DetModule.h
//  
//
//  Created by Julia Hrdinka on 16/12/14.
//
//

#ifndef DET_DETMODULE_H
#define DET_DETMODULE_H

#include "DD4hep/LCDD.h"
#include "DetExtensions/IExtension.h"

namespace Det {
    
    class DetModule : public IExtension {
    
    public:
        
        DetModule ()
        {}
        DetModule (const DetModule&, const DD4hep::Geometry::DetElement&)
        {}
        virtual ~DetModule()
        {}
    };
}
#endif  //DET_MODULE_H
