//
//  DetLayer.h
//  
//
//  Created by Julia Hrdinka on 12/12/14.
//
//

#ifndef DET_DETLAYER_H
#define DET_DETLAYER_H

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
    };
}



#endif //DET_DETLAYER_H
