//
//  MaterialMap.cxx
//  
//
//  Created by Julia Hrdinka on 05/01/15.
//
//

#include "RecoGeometry/MaterialMap.h"

Reco::MaterialMap::MaterialMap(Trk::BinUtility* binutility, std::map<std::pair<int,int>,Reco::Material*>* material) :
    m_binutility(binutility),
    m_binvector(0),
    m_material(material)
{}

Reco::MaterialMap::MaterialMap(Trk::BinUtility* binutility, std::vector<Trk::BinUtility*>* binvector, std::map<std::pair<int,int>,Reco::Material*>* material) :
m_binutility(binutility),
m_binvector(binvector),
m_material(material)
{}

Reco::MaterialMap::MaterialMap(const MaterialMap& materialmap) :
m_binutility(0),
m_binvector(0),
m_material(0)
{
    m_binutility = materialmap.m_binutility;
    m_material   = materialmap.m_material;
}

Reco::MaterialMap::~MaterialMap()
{
    m_binutility  = 0;
    m_binvector   = 0;
    m_material    = 0;
}

const Reco::Material* Reco::MaterialMap::material(std::pair<int,int> bins) const
{
    return (m_material->at(bins));
}

const Trk::BinUtility* Reco::MaterialMap::binutility() const
{
    return (m_binutility);
}

const Trk::BinUtility* Reco::MaterialMap::binvectorAt(size_t bin) const
{
    return (m_binvector->at(bin));
}

/*size_t Reco::MaterialMap::bin(const Alg::Point2D& lposition, size_t ba) const
{
    return (m_binutility->bin(lposition, ba));
}

size_t Reco::MaterialMap::vectorBin(size_t bin, const Alg::Point2D& lposition, size_t ba) const
{
    return (m_binvector.at(bin)->bin(locpos,ba));
}
*/
//const Reco::Material& Reco::MaterialMap::BoxMaterial(Alg::Point2D& locpos) const
//{
//    if (!m_binvector) {
 //       int binx = bin(locpos,0);
 //       int biny = bin(locpos,1);
        //schauen, ob eh passt
 //       return (m_material->at(std::make_pair(binx,biny)));
  //  }
 //   else {
  //      int biny = bin(locpos,0);
 //       int binx = m_binvector[biny]->bin(locpos,0);
        
  //  }
//}

