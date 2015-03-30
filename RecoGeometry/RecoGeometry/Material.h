//
//  Material.h
//  
//
//  Created by Julia Hrdinka on 19/12/14.
//
//

#ifndef RECO_MATERIAL_H
#define RECO_MATERIAL_H

namespace Reco {
    
    class Material {
    
    public:
        
        Material (double A, double Z, double density, double tInX0, double tInLambda0, double sensPer);
        ~Material ();
        double A() const;
        double Z() const;
        double density() const;
        double tInX0() const;
        double tInLambda0() const;
        double sensPer() const;
        
        
    private:
        const double m_A;
        const double m_Z;
        const double m_density;
        const double m_tInX0;
        const double m_tInLambda0;
        const double m_sensPer; //sensitive percentage
    };
}

#endif //RECO_MATERIAL_H
