//
//  SurfaceData.h
//  
//
//  Created by Julia Hrdinka on 24/04/15.
//
//

#ifndef SURFACEDATA_H
#define SURFACEDATA_H

#include "GaudiKernel/DataObject.h"
#include "RecoGeometry/Surface.h"

class GAUDI_API SurfaceData : public DataObject {
    
public:
    
    SurfaceData()
    {}
    
    virtual ~SurfaceData()
    {}
    
    Reco::Surface* getSurface()
    {
        return m_surface;
    }
    
    void setSurface (Reco::Surface* surface)
    {
        m_surface = surface;
    }
    
private:
    
    Reco::Surface* m_surface;
};


#endif //SURFACEDATA_H
