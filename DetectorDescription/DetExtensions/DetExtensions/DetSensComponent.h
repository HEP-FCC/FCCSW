//
//  DetSensComponent.h
//  
//
//  Created by Julia Hrdinka on 15/06/15.
//
//

#ifndef DET_DETSENSCOMPONENT_H
#define DET_DETSENSCOMPONENT_H

#include "DetExtensions/IDetExtension.h"
#include <memory>

namespace DD4hep {
    namespace Geometry {
        class DetElement;
        class Segmentation;
    }
}

namespace Det {
    
    class DetSensComponent : public IDetExtension {
    
    public:
        
        DetSensComponent(const DD4hep::Geometry::Segmentation segmentation) :
        m_segmentation(segmentation)
        {
            
        }
        DetSensComponent (const DetSensComponent&, const DD4hep::Geometry::DetElement&)
        {}
        virtual ~DetSensComponent()
        {}
        const DD4hep::Geometry::Segmentation segmentation()
        {
            return (m_segmentation);
        }
        virtual ExtensionType type()
        {
            return ExtensionType::SensComponent;
        }
        
    private:
        
        const DD4hep::Geometry::Segmentation m_segmentation;
    };
}

#endif //DET_DETSENSCOMPONENT_H
