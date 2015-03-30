//
//  IExtension.h
//  
//
//  Created by Julia Hrdinka on 12/12/14.
//
//

#ifndef DET_IEXTENSION_H
#define DET_IEXTENSION_H

#include "DD4hep/LCDD.h"

namespace Det {
    
    class IExtension {
    
    public:
        
        virtual ~IExtension()
        {}
        
    protected:
        
        IExtension()
        {}
        
    };
}

#endif //DET_IEXTENSION_H
