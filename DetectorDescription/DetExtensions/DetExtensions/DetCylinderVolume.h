//
//  DetCylinderVolume.h
//  
//
//  Created by Julia Hrdinka on 19/01/15.
//
//

#ifndef DET_DETCYLINDERVOLUME_H
#define DET_DETCYLINDERVOLUME_H

#include "DD4hep/LCDD.h"
#include "DetExtensions/IExtension.h"

namespace Det {
    
    class DetCylinderVolume : public IExtension {
        
    public:
        
        DetCylinderVolume(int status) :
        m_status(status)
        {}
        DetCylinderVolume(const DetCylinderVolume& volume, const DD4hep::Geometry::DetElement&)
        {
            m_status = volume.m_status;
        }
        virtual ~DetCylinderVolume()
        {}
        int status()
        {
            return m_status;
        }
        
    private:
        
        int m_status;
    };
}

#endif //DET_DETCYLINDERVOLUME_H
