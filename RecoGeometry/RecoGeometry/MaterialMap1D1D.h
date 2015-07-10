//
//  MaterialMap1D1D.h
//  
//
//  Created by Julia Hrdinka on 18/05/15.
//
//

#ifndef RECO_MATERIALMAP1D1D_H
#define RECO_MATERIALMAP1D1D_H

#include "RecoGeometry/MaterialMap.h"

namespace Reco {
    
    class MaterialMap1D1D : public MaterialMap {
    
    public:
        MaterialMap1D1D(std::map<std::pair<size_t,size_t>, Material*>* material, Trk::BinUtility* binutility, std::vector<Trk::BinUtility*>* binvector);
        MaterialMap1D1D(const MaterialMap1D1D& materialmap);
        ~MaterialMap1D1D();
        //clone method
        virtual MaterialMap1D1D* clone() const override;
        virtual const Material* material(const Alg::Point2D& locpos) const override;
        
    private:
        const Trk::BinUtility*          m_binutility;
        std::vector<Trk::BinUtility*>*  m_binvector;
    };
}

#endif //RECO_MATERIALMAP1D1D_H
