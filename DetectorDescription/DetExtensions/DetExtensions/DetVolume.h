//
//  DetVolume.h
//  
//
//  Created by Julia Hrdinka on 09/01/15.
//
//

#ifndef DET_DETVOLUME_H
#define DET_DETVOLUME_H

#include "DD4hep/LCDD.h"
#include "DetExtensions/IExtension.h"

namespace Det {
    
    class DetVolume : public IExtension {
    
    public:
        
        DetVolume()
        {}
        DetVolume(const DetVolume, const DD4hep::Geometry::DetElement&)
        {}
        virtual ~DetVolume()
        {}
    };
}

#endif //DET_DETVOLUME_H
