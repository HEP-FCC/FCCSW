//
//  TrapezoidSensitiveSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveTrapezoidSurface.h"

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, size_t binsX, size_t binsY, long long int volumeID) :
TrapezoidSurface(node, trapezoid),
SensitiveSurface(volumeID)
{
    m_binutility = new Trk::BinUtility(binsY, -TrapezoidSurface::getHalfY(), TrapezoidSurface::getHalfY(), Trk::open, Trk::binY);
    m_binvector = new std::vector<Trk::BinUtility*>(binsY);
    double k = (2.*TrapezoidSurface::getHalfY())/(TrapezoidSurface::getHalfX2()-TrapezoidSurface::getHalfX1());
    double d = -k*(TrapezoidSurface::getHalfX2()+TrapezoidSurface::getHalfX1())*0.5;
    double stepsizeY = (2.*TrapezoidSurface::getHalfY())/binsY;
    double x = 0;
    double y = 0;
    for (size_t i=0; i<binsY;i++) {
        y = -TrapezoidSurface::getHalfY() + i*stepsizeY;
        x = (y-k)/d;
        Trk::BinUtility* binXutil = new Trk::BinUtility(binsX,-x,x,Trk::open,Trk::binX);
        m_binvector->at(i) = binXutil;
    }
}

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, std::shared_ptr<const Alg::Transform3D> transf, size_t binsX, size_t binsY, long long int volumeID) :
TrapezoidSurface(trapezoid, transf),
SensitiveSurface(volumeID)
{
    m_binutility = new Trk::BinUtility(binsY, -TrapezoidSurface::getHalfY(), TrapezoidSurface::getHalfY(), Trk::open, Trk::binY);
    m_binvector = new std::vector<Trk::BinUtility*>(binsY);
    double k = (2.*TrapezoidSurface::getHalfY())/(TrapezoidSurface::getHalfX2()-TrapezoidSurface::getHalfX1());
    double d = -k*(TrapezoidSurface::getHalfX2()+TrapezoidSurface::getHalfX1())*0.5;
    double stepsizeY = (2.*TrapezoidSurface::getHalfY())/binsY;
    double x = 0;
    double y = 0;
    for (size_t i=0; i<binsY;i++) {
        y = -TrapezoidSurface::getHalfY() + i*stepsizeY;
        x = (y-k)/d;
        Trk::BinUtility* binXutil = new Trk::BinUtility(binsX,-x,x,Trk::open,Trk::binX);
        m_binvector->at(i) = binXutil;
    }
}
Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, MaterialMap* materialmap, size_t binsX, size_t binsY, long long int volumeID) :
Reco::TrapezoidSurface(node, trapezoid, materialmap),
Reco::SensitiveSurface(volumeID)
{
    m_binutility = new Trk::BinUtility(binsY, -TrapezoidSurface::getHalfY(), TrapezoidSurface::getHalfY(), Trk::open, Trk::binY);
    m_binvector = new std::vector<Trk::BinUtility*>(binsY);
    double k = (2.*TrapezoidSurface::getHalfY())/(TrapezoidSurface::getHalfX2()-TrapezoidSurface::getHalfX1());
    double d = -k*(TrapezoidSurface::getHalfX2()+TrapezoidSurface::getHalfX1())*0.5;
    double stepsizeY = (2.*TrapezoidSurface::getHalfY())/binsY;
    double x = 0;
    double y = 0;
    for (size_t i=0; i<binsY;i++) {
        y = -TrapezoidSurface::getHalfY() + i*stepsizeY;
        x = (y-k)/d;
        Trk::BinUtility* binXutil = new Trk::BinUtility(binsX,-x,x,Trk::open,Trk::binX);
        m_binvector->at(i) = binXutil;
    }
}
Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, size_t binsX, size_t binsY, long long int volumeID) :
Reco::TrapezoidSurface(trapezoid, materialmap, transf),
Reco::SensitiveSurface(volumeID)
{
    m_binutility = new Trk::BinUtility(binsY, -TrapezoidSurface::getHalfY(), TrapezoidSurface::getHalfY(), Trk::open, Trk::binY);
    m_binvector = new std::vector<Trk::BinUtility*>(binsY);
    double k = (2.*TrapezoidSurface::getHalfY())/(TrapezoidSurface::getHalfX2()-TrapezoidSurface::getHalfX1());
    double d = -k*(TrapezoidSurface::getHalfX2()+TrapezoidSurface::getHalfX1())*0.5;
    double stepsizeY = (2.*TrapezoidSurface::getHalfY())/binsY;
    double x = 0;
    double y = 0;
    for (size_t i=0; i<binsY;i++) {
        y = -TrapezoidSurface::getHalfY() + i*stepsizeY;
        x = (y-k)/d;
        Trk::BinUtility* binXutil = new Trk::BinUtility(binsX,-x,x,Trk::open,Trk::binX);
        m_binvector->at(i) = binXutil;
    }
}
                                                           
Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX1, double halfX2, double halfY, size_t binsX, size_t binsY, long long int volumeID) :
Reco::TrapezoidSurface(transf, halfX1, halfX2, halfY),
Reco::SensitiveSurface(volumeID)
{
    m_binutility = new Trk::BinUtility(binsY, -TrapezoidSurface::getHalfY(), TrapezoidSurface::getHalfY(), Trk::open, Trk::binY);
    m_binvector = new std::vector<Trk::BinUtility*>(binsY);
    double k = (2.*TrapezoidSurface::getHalfY())/(TrapezoidSurface::getHalfX2()-TrapezoidSurface::getHalfX1());
    double d = -k*(TrapezoidSurface::getHalfX2()+TrapezoidSurface::getHalfX1())*0.5;
    double stepsizeY = (2.*TrapezoidSurface::getHalfY())/binsY;
    double x = 0;
    double y = 0;
    for (size_t i=0; i<binsY;i++) {
        y = -TrapezoidSurface::getHalfY() + i*stepsizeY;
        x = (y-k)/d;
        Trk::BinUtility* binXutil = new Trk::BinUtility(binsX,-x,x,Trk::open,Trk::binX);
        m_binvector->at(i) = binXutil;
    }
}
Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(const Reco::SensitiveTrapezoidSurface& senstrap) :
Reco::TrapezoidSurface(senstrap),
Reco::SensitiveSurface(senstrap),
m_binutility(new Trk::BinUtility(*senstrap.m_binutility)),
m_binvector(new std::vector<Trk::BinUtility*>(*senstrap.m_binvector))
{}

Reco::SensitiveTrapezoidSurface::~SensitiveTrapezoidSurface()
{
    delete m_binutility;
    delete m_binvector;
}

Reco::SensitiveTrapezoidSurface* Reco::SensitiveTrapezoidSurface::clone() const
{
    return (new Reco::SensitiveTrapezoidSurface(*this));
}
                                                           
Reco::SensitiveTrapezoidSurface& Reco::SensitiveTrapezoidSurface::operator=(const Reco::SensitiveTrapezoidSurface& senstrap)
{
    if (this!=&senstrap) {
        Reco::TrapezoidSurface::operator=(senstrap);
        Reco::SensitiveSurface::operator=(senstrap);
        delete m_binutility;
        m_binutility = new Trk::BinUtility(*senstrap.m_binutility);
        delete m_binvector;
        m_binvector = new std::vector<Trk::BinUtility*>(*senstrap.m_binvector);
    }
    return (*this);
}
size_t Reco::SensitiveTrapezoidSurface::bin(const Alg::Point2D& locpos) const
{
    size_t j = m_binutility->bin(locpos,0);
    size_t i = m_binvector->at(j)->bin(locpos,0);
    int n = m_binutility->bins(0);
    return(j+i*n);
}
                                    

