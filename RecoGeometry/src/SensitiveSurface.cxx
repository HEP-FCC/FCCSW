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
{}

Reco::SensitiveSurface::SensitiveSurface(const SensitiveSurface& senssurf) :
m_surfaceID(senssurf.m_surfaceID)
{}

Reco::SensitiveSurface::~SensitiveSurface()
{}

Reco::SensitiveSurface& Reco::SensitiveSurface::operator=(const Reco::SensitiveSurface& senssurf)
{
    if (this!=&senssurf) {
        m_surfaceID = senssurf.m_surfaceID;
    }
    return (*this);
}

unsigned long long Reco::SensitiveSurface::cellID(const Alg::Point2D& locpos)
{
    //uniquely identifies the local position on the surface
    unsigned long localID = bin(locpos);
    //create the 64 bit global unique cellID out of the two 32bit surfaceID and localID
    return (m_surfaceID | (localID << 32));
}