//
//  DetDiscLayer.h
//  
//
//  Created by Julia Hrdinka on 07/01/15.
//
//

#ifndef DET_DETDISCLAYER
#define DET_DETDISCLAYER

#include "DetExtensions/IDetExtension.h"

namespace DD4hep {
    namespace Geometry {
        class DetElement;
    }
}

namespace Det {
    
    class DetDiscLayer : public IDetExtension {
    
    public:
        
        DetDiscLayer()
        {}
        
        DetDiscLayer(const DetDiscLayer&, const DD4hep::Geometry::DetElement&)
        {}
        
        virtual ~DetDiscLayer()
        {}
        
        virtual ExtensionType type()
        {
            return ExtensionType::DiscLayer;
        }
        
    };
}

#endif //DET_DETDISCLAYER
