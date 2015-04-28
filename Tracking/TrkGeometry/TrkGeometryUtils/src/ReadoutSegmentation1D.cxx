//
//  ReadoutSegmentation1D.cxx
//  
//
//  Created by Julia Hrdinka on 24/04/15.
//
//

#include "TrkGeometryUtils/ReadoutSegmentation1D.h"

Trk::ReadoutSegmentation1D::ReadoutSegmentation1D(Trk::BinUtility* binutil) :
Trk::ReadoutSegmentation(),
m_binutility(binutil)
{}

Trk::ReadoutSegmentation1D::ReadoutSegmentation1D(const Trk::ReadoutSegmentation1D& seg) :
Trk::ReadoutSegmentation(seg),
m_binutility(new Trk::BinUtility(*seg.m_binutility))
{}

Trk::ReadoutSegmentation1D::~ReadoutSegmentation1D()
{
    delete m_binutility;
}

Trk::ReadoutSegmentation1D* Trk::ReadoutSegmentation1D::clone() const
{
    return(new Trk::ReadoutSegmentation1D(*this));
}

Trk::ReadoutSegmentation1D& Trk::ReadoutSegmentation1D::operator=(const Trk::ReadoutSegmentation1D& seg)
{
    if (this!=&seg) {
        Trk::ReadoutSegmentation::operator=(seg);
        delete m_binutility;
        m_binutility = new Trk::BinUtility(*seg.m_binutility);
    }
    return (*this);
}

size_t Trk::ReadoutSegmentation1D::bins() const
{
    return (m_binutility->bins());
}

const unsigned long Trk::ReadoutSegmentation1D::bin(const Alg::Point2D& locpos) const
{
    return (m_binutility->bin(locpos,0));
}

const std::vector<unsigned long> Trk::ReadoutSegmentation1D::compatibleBins(const Alg::Point2D& locpos) const
{
    std::vector<unsigned long> bins;
    unsigned long bin = this->bin(locpos);
    for (int i=bin-1; i<this->bins(); i++) {
        if (i>=0) {
            unsigned long value=i;
            bins.push_back(value);
        }
    }
    return (bins);
}

Alg::Point2D Trk::ReadoutSegmentation1D::binToLocpos(unsigned long bin) const
{
    double c1 = m_binutility->bincenter(bin,0);
    
    return (Alg::Point2D(c1,0.));
}

float Trk::ReadoutSegmentation1D::binwidth(const Alg::Point2D& locpos, size_t ba) const
{
    return (m_binutility->binwidth(m_binutility->bin(locpos,0), 0));
}


