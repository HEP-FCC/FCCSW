//
//  MaterialMap2D.h
//  
//
//  Created by Julia Hrdinka on 18/05/15.
//
//

#ifndef RECO_MATERIALMAP2D_H
#define RECO_MATERIALMAP2D_H

#include "RecoGeometry/MaterialMap.h"

namespace Reco {
    
    class MaterialMap2D : public MaterialMap {
        
    public:
        MaterialMap2D(std::map<std::pair<size_t,size_t>, Material*>* material, Trk::BinUtility* binutility);
        MaterialMap2D(const MaterialMap2D& materialmap);
        ~MaterialMap2D();
        //clone method
        virtual MaterialMap2D* clone() const override;
        virtual const Material* material(const Alg::Point2D& locpos) const override;
        
    private:
        const Trk::BinUtility*          m_binutility;
    };
}


#endif //RECO_MATERIALMAP2D_H
