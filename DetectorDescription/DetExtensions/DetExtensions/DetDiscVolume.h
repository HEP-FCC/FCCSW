//
//  DetDiscVolume.h
//  
//
//  Created by Julia Hrdinka on 19/01/15.
//
//

#ifndef DET_DETDISCVOLUME_H
#define DET_DETDISCVOLUME_H

#include "DD4hep/LCDD.h"
#include "DetExtensions/IExtension.h"

namespace Det {
    
    class DetDiscVolume : public IExtension {
    
    public:
        
        DetDiscVolume(int status) :
        m_status(status)
        {}
        DetDiscVolume(const DetDiscVolume& volume, const DD4hep::Geometry::DetElement&)
        {
            m_status = volume.m_status;
        }
        virtual ~DetDiscVolume()
        {}
        int status()
        {
            return m_status;
        }
        
    private:
        
        int m_status;
    };
}

#endif //DET_DETDISCVOLUME_H
