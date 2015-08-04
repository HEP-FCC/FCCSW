//
//  DetCylinderLayer.h
//  
//
//  Created by Julia Hrdinka on 12/12/14.
//
//

#ifndef DET_DETCYLINDERLAYER_H
#define DET_DETCYLINDERLAYER_H

#include "DetExtensions/IDetExtension.h"

namespace DD4hep {
    namespace Geometry {
        class DetElement;
    }
}


namespace Det {
    
    class DetCylinderLayer : public IDetExtension {
        
    public:
        
        DetCylinderLayer()
        {}
        
        DetCylinderLayer(const DetCylinderLayer&, const DD4hep::Geometry::DetElement&)
        {}
        
        virtual ~DetCylinderLayer()
        {}
        
        virtual ExtensionType type()
        {
            return ExtensionType::CylinderLayer;
        }
    };
}



#endif //DET_DETCYLINDERLAYER_H
