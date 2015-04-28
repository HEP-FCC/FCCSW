//
//  SensitiveSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveSurface.h"

Reco::SensitiveSurface::SensitiveSurface(unsigned long surfaceID) :
m_surfaceID(surfaceID)
{
//    m_IDs.open("IDs.dat", std::ofstream::out);
}

Reco::SensitiveSurface::SensitiveSurface(const SensitiveSurface& senssurf) :
m_surfaceID(senssurf.m_surfaceID)
{}

Reco::SensitiveSurface::~SensitiveSurface()
{
//    m_IDs.close();
}

Reco::SensitiveSurface& Reco::SensitiveSurface::operator=(const Reco::SensitiveSurface& senssurf)
{
    if (this!=&senssurf) {
        m_surfaceID = senssurf.m_surfaceID;
    }
    return (*this);
}

unsigned long long Reco::SensitiveSurface::cellID(const Alg::Point2D& locpos) const
{
    //uniquely identifies the local position on the surface
    unsigned long localID = bin(locpos);
    //create the 64 bit global unique cellID out of the two 32bit surfaceID and localID
//    m_IDs << "surfaceID: " <<  m_surfaceID <<  std::endl;
//    m_IDs << "localID: " << localID << std::endl;
    return (m_surfaceID | (localID << 32));
}
//noch cellId to locpos machen

Alg::Point2D Reco::SensitiveSurface::locpos(unsigned long long cellID) const
{
    unsigned long localID = cellID >> 32;
    
    return (binToLocpos(localID));
}