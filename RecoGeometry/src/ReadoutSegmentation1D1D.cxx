//
//  ReadoutSegmentation1D1D.cxx
//  
//
//  Created by Julia Hrdinka on 24/04/15.
//
//

#include "RecoGeometry/ReadoutSegmentation1D1D.h"

Reco::ReadoutSegmentation1D1D::ReadoutSegmentation1D1D(Trk::BinUtility* binutil, std::vector<Trk::BinUtility*>* m_binvector) :
Reco::ReadoutSegmentation(),
m_binutility(binutil),
m_binvector(m_binvector)
{
    if (m_binutility->dimension()!=1) throw GaudiException("ReadoutSegmentation1D1D", "Dimension of BinUtility has to be 1 for this Semgentationtype", StatusCode::FAILURE);
}

Reco::ReadoutSegmentation1D1D::ReadoutSegmentation1D1D(const Reco::ReadoutSegmentation1D1D& seg) :
Reco::ReadoutSegmentation(seg),
m_binutility(new Trk::BinUtility(*seg.m_binutility)),
m_binvector(new std::vector<Trk::BinUtility*>(*seg.m_binvector))
{}

Reco::ReadoutSegmentation1D1D::~ReadoutSegmentation1D1D()
{
    delete m_binutility;
    delete m_binvector;
}

Reco::ReadoutSegmentation1D1D* Reco::ReadoutSegmentation1D1D::clone() const
{
    return(new Reco::ReadoutSegmentation1D1D(*this));
}

Reco::ReadoutSegmentation1D1D& Reco::ReadoutSegmentation1D1D::operator=(const Reco::ReadoutSegmentation1D1D& seg)
{
    if (this!=&seg) {
        Reco::ReadoutSegmentation::operator=(seg);
        delete m_binutility;
        m_binutility = new Trk::BinUtility(*seg.m_binutility);
        delete m_binvector;
        m_binvector = new std::vector<Trk::BinUtility*>(*seg.m_binvector);
    }
    return (*this);
}

size_t Reco::ReadoutSegmentation1D1D::bins() const
{
    size_t sum = 0;
    for (size_t j=0; j<m_binvector->size(); j++) {
        sum += m_binvector->at(j)->bins();
    }
    return sum;
}

unsigned long Reco::ReadoutSegmentation1D1D::bin(const Alg::Point2D& locpos) const
{
    unsigned long j = m_binutility->bin(locpos,0);
    unsigned long l = m_binvector->at(j)->bin(locpos,0);
    if (j>0) {
        for (size_t k=0; k<j; k++) {
            l += m_binvector->at(k)->bins(0);
        }
    }

    return(l);
}

const std::vector<unsigned long> Reco::ReadoutSegmentation1D1D::compatibleBins(const Alg::Point2D& locpos) const
{
    std::vector<unsigned long> bins;
    int max2 = m_binutility->bins(0);
    int j = m_binutility->bin(locpos,0);
    unsigned long l = m_binvector->at(j)->bin(locpos,0);
    long lsigned = l;

    for (int k=0; k<j+1; k++) {
        size_t max1 = m_binvector->at(k)->bins(0);
        l += max1;
        if (k==j-1 && k>=0) {
            bins.push_back(l);
            if (lsigned-1>=0) bins.push_back(l-1);
            if (l+1<max1) bins.push_back(l+1);
        }
        if (k==j) {
            bins.push_back(l);
            if (lsigned-1>=0) bins.push_back(l-1);
            if (l+1<max1) bins.push_back(l+1);
        }
        if (k==j+1 && k<max2) {
            bins.push_back(l);
            if (lsigned-1>=0) bins.push_back(l-1);
            if (l+1<max1) bins.push_back(l+1);
        }
    }
    
    return (bins);
}


Alg::Point2D Reco::ReadoutSegmentation1D1D::binToLocpos(unsigned long bin) const
{
    size_t s = 0;
    size_t j = 0;
    size_t mk= 0;
    for (size_t k=0; k<m_binutility->bins(0) && bin<s; k++) {
        mk = m_binvector->at(k)->bins(0);
        s += mk;
        j = k;
    }
    size_t i = (mk-(s-bin));
    
    return (Alg::Point2D(m_binvector->at(j)->bincenter(i,0),m_binutility->bincenter(j,0)));
}

float Reco::ReadoutSegmentation1D1D::binwidth(const Alg::Point2D& locpos, size_t ba) const
{
    size_t bin2 = m_binutility->bin(locpos, 0);
    if (ba == 0) {
        size_t bin1 = m_binvector->at(bin2)->bin(locpos,0);
        return (m_binvector->at(bin2)->binwidth(bin1,0));
    }
   return (m_binutility->binwidth(bin2, 0));
}



