//
//  DetDiscLayer.h
//  
//
//  Created by Julia Hrdinka on 07/01/15.
//
//

#ifndef DET_DETDISCLAYER
#define DET_DETDISCLAYER

#include "DD4hep/LCDD.h"
#include "DetExtensions/IExtension.h"

namespace Det {
    
    class DetDiscLayer : public IExtension {
    
    public:
        
        DetDiscLayer(int modulesR, std::vector<std::pair<float,float>> rValues, int modulesPhi, double min, double max) :
        m_modulesR(modulesR),
        m_rValues(rValues),
        m_modulesPhi(modulesPhi),
        m_min(min),
        m_max(max)
        {}
        DetDiscLayer(const DetDiscLayer& layer, const DD4hep::Geometry::DetElement&)
        {
            m_modulesR   = layer.m_modulesR;
            m_modulesPhi = layer.m_modulesPhi;
        }
        virtual ~DetDiscLayer()
        {}
        int modulesR()
        {
            return m_modulesR;
        }
        
        std::vector<std::pair<float,float>> rValues()
        {
            return m_rValues;
        }
        
        int modulesPhi()
        {
            return m_modulesPhi;
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
        int m_modulesR;
        std::vector<std::pair<float,float>> m_rValues;
        int m_modulesPhi;
        double m_min;
        double m_max;
    };
}

#endif //DET_DETDISCLAYER
