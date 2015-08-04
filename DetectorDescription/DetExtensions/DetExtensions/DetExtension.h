//
//  DetExtension.h
//  
//
//  Created by Julia Hrdinka on 12/12/14.
//
//

#ifndef DET_DETEXTENSION_H
#define DET_DETEXTENSION_H

#include "DetExtensions/IDetExtension.h"

namespace DD4hep {
    namespace Geometry {
        class DetElement;
    }
}

namespace Det {
    
    class DetExtension : public IDetExtension {
    
    public:
        
        DetExtension()
        {}
        DetExtension(const DetExtension&, const DD4hep::Geometry::DetElement&)
        {}
        virtual ~DetExtension()
        {}
        virtual ExtensionType type()
        {
            return ExtensionType::None;
        }
    };
}
#endif //DET_DETEXTENSION_H
