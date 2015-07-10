//
//  SensitiveSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveSurface.h"

Reco::SensitiveSurface::SensitiveSurface(long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
m_volumeID(volumeID),
m_segmentation(segmentation)
{
//    m_IDs.open("IDs.dat", std::ofstream::out);
}

Reco::SensitiveSurface::SensitiveSurface(const SensitiveSurface& senssurf) :
m_volumeID(senssurf.m_volumeID),
m_segmentation(senssurf.m_segmentation)
{}

Reco::SensitiveSurface::~SensitiveSurface()
{
//    m_IDs.close();
}

Reco::SensitiveSurface& Reco::SensitiveSurface::operator=(const Reco::SensitiveSurface& senssurf)
{
    if (this!=&senssurf) {
        m_volumeID = senssurf.m_volumeID;
        m_segmentation = senssurf.m_segmentation;
    }
    return (*this);
}

long long int Reco::SensitiveSurface::cellID(const Alg::Point2D& locpos) const
{
    Alg::Point3D global(0.,0.,0.);
    this->surfaceRepresentation()->localToGlobal(locpos,Alg::Vector3D(),global);
    Alg::Vector3D ddloc (locpos.X(),locpos.Y(),0.);
    Alg::Vector3D ddglo (global.X(),global.Y(),global.Z());
 //   DD4hep::Geometry::Position ddloc(locpos.X(),locpos.Y(),0.);
    //typedef of XYZector
 //   DD4hep::Geometry::Position ddglo(global.X(),global.Y(),global.Z());
    long long int cellid = 0;
    if (m_segmentation) {
        cellid =m_segmentation->cellID(ddloc,ddglo,m_volumeID);
    }
    return(cellid);
    //uniquely identifies the local position on the surface
//    unsigned long localID = bin(locpos);
    //create the 64 bit global unique cellID out of the two 32bit surfaceID and localID
//    m_IDs << "surfaceID: " <<  m_surfaceID <<  std::endl;
//    m_IDs << "localID: " << localID << std::endl;
//    return (m_surfaceID | (localID << 32));
}
//noch cellId to locpos machen


Alg::Point2D Reco::SensitiveSurface::locpos(long long int cellID) const
{
    Alg::Vector3D loc3D = m_segmentation->position(cellID);
    
    return (Alg::Point2D(loc3D.x(),loc3D.y()));

}

const std::set<long long int> Reco::SensitiveSurface::neighbours(long long int cellID) const
{
    std::set<long long int> neighbours;
    m_segmentation->segmentation()->neighbours(cellID,neighbours);
    return(neighbours);
}
/*
unsigned long Reco::SensitiveSurface::bin(const Alg::Point2D&locpos) const
{
    return(0);
  //  return(m_segmentation->bin(locpos));
}

Alg::Point2D Reco::SensitiveSurface::binToLocpos(unsigned long bin) const
{
    return(Alg::Point2D(0.,0.));
  //  return (m_segmentation->binToLocpos(bin));
}

float Reco::SensitiveSurface::binWidth(const Alg::Point2D& locpos, size_t ba) const
{
    return (0.);
    //return(m_segmentation->binwidth(locpos, ba));
}
*/
/*const std::vector<unsigned long> Reco::SensitiveSurface::compatibleBins(const Alg::Point2D& locpos) const
{
    //return(m_segmentation->compatibleBins(locpos));
}*/
