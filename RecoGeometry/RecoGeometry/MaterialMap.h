//
//  MaterialMap.h
//  
//
//  Created by Julia Hrdinka on 05/01/15.
//
//

#ifndef RECO_MATERIALMAP_H
#define RECO_MATERIALMAP_H

#include "TrkGeometryUtils/BinUtility.h"
#include "RecoGeometry/Material.h"

namespace Reco {
    
    class MaterialMap {
    
    public:
        MaterialMap(std::map<std::pair<size_t,size_t>, Material*>* material);
        MaterialMap(const MaterialMap& materialmap);
        ~MaterialMap();
        //clone method
        virtual MaterialMap* clone() const = 0;
        virtual const Material* material(const Alg::Point2D& locpos) const = 0;
        
    protected:
        const std::map<std::pair<size_t,size_t>, Material*>* m_material;
    };
}
#endif //RECO_MATERIALMAP_H
