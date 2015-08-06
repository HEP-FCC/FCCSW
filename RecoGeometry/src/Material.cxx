//
//  Material.cxx
//  
//
//  Created by Julia Hrdinka on 19/12/14.
//
//

#include "RecoGeometry/Material.h"

Reco::Material::Material(double A, double Z, double density, double tInX0, double tInLambda0, double sensPer) :
m_A(A),
m_Z(Z),
m_density(density),
m_tInX0(tInX0),
m_tInLambda0(tInLambda0),
m_sensPer(sensPer)
{}

Reco::Material::~Material ()
{}

double Reco::Material::A() const
{
    return (m_A);
}

double Reco::Material::Z() const
{
    return (m_Z);
}

double Reco::Material::density() const
{
    return (m_density);
}

double Reco::Material::tInX0() const
{
    return (m_tInX0);
}

double Reco::Material::tInLambda0() const
{
    return (m_tInLambda0);
}

double Reco::Material::sensPer() const
{
    return (m_sensPer);
}