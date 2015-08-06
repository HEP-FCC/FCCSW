//
//  MaterialMap.cxx
//  
//
//  Created by Julia Hrdinka on 05/01/15.
//
//

#include "RecoGeometry/MaterialMap.h"

Reco::MaterialMap::MaterialMap(std::map<std::pair<size_t,size_t>,Reco::Material*>* material) :
    m_material(material)
{}

Reco::MaterialMap::MaterialMap(const MaterialMap& materialmap) :
m_material(materialmap.m_material)
{}

Reco::MaterialMap::~MaterialMap()
{
    m_material    = 0;
}




