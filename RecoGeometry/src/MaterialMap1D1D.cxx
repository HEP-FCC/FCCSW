//
//  MaterialMap1D1D.cxx
//  
//
//  Created by Julia Hrdinka on 18/05/15.
//
//

#include "RecoGeometry/MaterialMap1D1D.h"

Reco::MaterialMap1D1D::MaterialMap1D1D(std::map<std::pair<size_t,size_t>, Material*>* material, Trk::BinUtility* binutility, std::vector<Trk::BinUtility*>* binvector) :
Reco::MaterialMap(material),
m_binutility(binutility),
m_binvector(binvector)
{
   // if (m_binutility->dimension()==1) {
  //      m_binutility = binutility;
  //  }
 //   else {
  //      std::cout << "dimen: " << m_binutility->dimension() << std::endl;
 //       throw GaudiException("MaterialMap1D1D", "BinUtility has wrong dimension, 1D needed", StatusCode::FAILURE);
 //   }
}

Reco::MaterialMap1D1D::MaterialMap1D1D(const Reco::MaterialMap1D1D& materialmap) :
Reco::MaterialMap(materialmap),
m_binutility(new Trk::BinUtility(*materialmap.m_binutility)),
m_binvector(new std::vector<Trk::BinUtility*>(*materialmap.m_binvector))
{}

Reco::MaterialMap1D1D::~MaterialMap1D1D()
{
    m_binutility = 0;
    m_binvector  = 0;
}

Reco::MaterialMap1D1D* Reco::MaterialMap1D1D::clone() const
{
    return (new Reco::MaterialMap1D1D(*this));
}

const Reco::Material* Reco::MaterialMap1D1D::material(const Alg::Point2D& locpos) const
{
    size_t biny = m_binutility->bin(locpos,0);
    size_t binx = m_binvector->at(biny)->bin(locpos,0);
    return (m_material->at(std::make_pair(binx, biny)));
}
