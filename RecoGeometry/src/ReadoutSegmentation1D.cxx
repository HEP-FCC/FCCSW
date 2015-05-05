//
//  ReadoutSegmentation1D.cxx
//  
//
//  Created by Julia Hrdinka on 24/04/15.
//
//

#include "RecoGeometry/ReadoutSegmentation1D.h"

Reco::ReadoutSegmentation1D::ReadoutSegmentation1D(Trk::BinUtility* binutil) :
Reco::ReadoutSegmentation(),
m_binutility(binutil)
{
    if (m_binutility->dimension()!=1) throw GaudiException("ReadoutSegmentation1D", "Dimension has to be 1 for this Semgentationtype", StatusCode::FAILURE);
}

Reco::ReadoutSegmentation1D::ReadoutSegmentation1D(const Reco::ReadoutSegmentation1D& seg) :
Reco::ReadoutSegmentation(seg),
m_binutility(new Trk::BinUtility(*seg.m_binutility))
{}

Reco::ReadoutSegmentation1D::~ReadoutSegmentation1D()
{
    delete m_binutility;
}

Reco::ReadoutSegmentation1D* Reco::ReadoutSegmentation1D::clone() const
{
    return(new Reco::ReadoutSegmentation1D(*this));
}

Reco::ReadoutSegmentation1D& Reco::ReadoutSegmentation1D::operator=(const Reco::ReadoutSegmentation1D& seg)
{
    if (this!=&seg) {
        Reco::ReadoutSegmentation::operator=(seg);
        delete m_binutility;
        m_binutility = new Trk::BinUtility(*seg.m_binutility);
    }
    return (*this);
}

size_t Reco::ReadoutSegmentation1D::bins() const
{
    return (m_binutility->bins());
}

const unsigned long Reco::ReadoutSegmentation1D::bin(const Alg::Point2D& locpos) const
{
    return (m_binutility->bin(locpos,0));
}

const std::vector<unsigned long> Reco::ReadoutSegmentation1D::compatibleBins(const Alg::Point2D& locpos) const
{
    std::vector<unsigned long> bins;
    unsigned long bin = this->bin(locpos);
    if ((bin-1) >= 0)           bins.push_back(bin-1);
                                bins.push_back(bin);
    if ((bin+1) < this->bins()) bins.push_back(bin+1);
    
    return (bins);
}

Alg::Point2D Reco::ReadoutSegmentation1D::binToLocpos(unsigned long bin) const
{
    double c1 = m_binutility->bincenter(bin,0);
    
    return (Alg::Point2D(c1,0.));
}

float Reco::ReadoutSegmentation1D::binwidth(const Alg::Point2D& locpos, size_t ba) const
{
    return (m_binutility->binwidth(m_binutility->bin(locpos,0), 0));
}


