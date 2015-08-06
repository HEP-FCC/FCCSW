//
//  MaterialMap2D.cxx
//  
//
//  Created by Julia Hrdinka on 18/05/15.
//
//

#include "RecoGeometry/MaterialMap2D.h"

Reco::MaterialMap2D::MaterialMap2D(std::map<std::pair<size_t,size_t>, Material*>* material, Trk::BinUtility* binutility) :
Reco::MaterialMap(material),
m_binutility(binutility)
{
 //   if (m_binutility->dimension()==2) {
 //       m_binutility = binutility;
 //   }
 //   else throw GaudiException("MaterialMap2D", "BinUtility has wrong dimension, 2D needed", StatusCode::FAILURE);
}

Reco::MaterialMap2D::MaterialMap2D(const Reco::MaterialMap2D& materialmap) :
Reco::MaterialMap(materialmap),
m_binutility(new Trk::BinUtility(*materialmap.m_binutility))
{}

Reco::MaterialMap2D::~MaterialMap2D()
{
    m_binutility = 0;
}

Reco::MaterialMap2D* Reco::MaterialMap2D::clone() const
{
    return (new Reco::MaterialMap2D(*this));
}

const Reco::Material* Reco::MaterialMap2D::material(const Alg::Point2D& locpos) const
{
    m_binutility->dimension();
    size_t binx = m_binutility->bin(locpos,0);
    size_t biny = m_binutility->bin(locpos,1);
    return (m_material->at(std::make_pair(binx,biny)));
}