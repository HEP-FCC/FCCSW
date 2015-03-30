//
//  Extension.h
//  
//
//  Created by Julia Hrdinka on 12/12/14.
//
//

#ifndef DET_EXTENSION_H
#define DET_EXTENSION_H

#include "DetExtensions/IExtension.h"
#include "DD4hep/LCDD.h"

namespace Det {
    
    class Extension : public IExtension {
    
    public:
        
        Extension()
        {}
        Extension(const Extension&, const DD4hep::Geometry::DetElement&)
        {}
        virtual ~Extension()
        {}
        
    };
}
#endif //DET_EXTENSION_H
