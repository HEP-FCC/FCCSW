//
//  DetLayer.h
//  
//
//  Created by Julia Hrdinka on 12/12/14.
//
//

#ifndef DET_DETLAYER_H
#define DET_DETLAYER_H

#include "DD4hep/LCDD.h"
#include "DetExtensions/IExtension.h"


namespace Det {
    
    class DetCylinderLayer : public IExtension {
        
    public:
        
        DetCylinderLayer(int modulesPhi, int modulesZ, double min, double max) :
        m_modulesPhi(modulesPhi),
        m_modulesZ(modulesZ),
        m_min(min),
        m_max(max)
        {}
        DetCylinderLayer(const DetCylinderLayer& layer, const DD4hep::Geometry::DetElement&)
        {
            m_modulesPhi = layer.m_modulesPhi;
            m_modulesZ   = layer.m_modulesZ;
        }
        virtual ~DetCylinderLayer()
        {}
        int modulesPhi()
        {
            return m_modulesPhi;
        }
        int modulesZ()
        {
            return m_modulesZ;
        }
        double min()
        {
            return m_min;
        }
        double max()
        {
            return m_max;
        }
    private:
        int m_modulesPhi;
        int m_modulesZ;
        double m_min;
        double m_max;
       
    };
}



#endif //DET_DETLAYER_H
