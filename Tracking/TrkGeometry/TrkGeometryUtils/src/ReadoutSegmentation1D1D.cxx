//
//  ReadoutSegmentation1D1D.cxx
//  
//
//  Created by Julia Hrdinka on 24/04/15.
//
//

#include "TrkGeometryUtils/ReadoutSegmentation1D1D.h"

Trk::ReadoutSegmentation1D1D::ReadoutSegmentation1D1D(Trk::BinUtility* binutil, std::vector<Trk::BinUtility*>* m_binvector) :
Trk::ReadoutSegmentation(),
m_binutility(binutil),
m_binvector(m_binvector)
{}

Trk::ReadoutSegmentation1D1D::ReadoutSegmentation1D1D(const Trk::ReadoutSegmentation1D1D& seg) :
Trk::ReadoutSegmentation(seg),
m_binutility(new Trk::BinUtility(*seg.m_binutility)),
m_binvector(new std::vector<Trk::BinUtility*>(*seg.m_binvector))
{}

Trk::ReadoutSegmentation1D1D::~ReadoutSegmentation1D1D()
{
    delete m_binutility;
    delete m_binvector;
}

Trk::ReadoutSegmentation1D1D* Trk::ReadoutSegmentation1D1D::clone() const
{
    return(new Trk::ReadoutSegmentation1D1D(*this));
}

Trk::ReadoutSegmentation1D1D& Trk::ReadoutSegmentation1D1D::operator=(const Trk::ReadoutSegmentation1D1D& seg)
{
    if (this!=&seg) {
        Trk::ReadoutSegmentation::operator=(seg);
        delete m_binutility;
        m_binutility = new Trk::BinUtility(*seg.m_binutility);
        delete m_binvector;
        m_binvector = new std::vector<Trk::BinUtility*>(*seg.m_binvector);
    }
    return (*this);
}

size_t Trk::ReadoutSegmentation1D1D::bins() const
{
    size_t sum = 0;
    for (int j=0; j<m_binvector->size(); j++) {
        sum += m_binvector->at(j)->bins();
    }
    return sum;
}

const unsigned long Trk::ReadoutSegmentation1D1D::bin(const Alg::Point2D& locpos) const
{
    unsigned long j = m_binutility->bin(locpos,0);
    unsigned long i = m_binvector->at(j)->bin(locpos,0);
    unsigned long l = i;
    for (int k=0; k<j; k++) {
        l += k*m_binvector->at(k)->bins(0);
    }
    return(l);
}

const std::vector<unsigned long> Trk::ReadoutSegmentation1D1D::compatibleBins(const Alg::Point2D& locpos) const
{
    std::vector<unsigned long> bins;
    int max2 = m_binutility->bins(0);
    int n = m_binutility->bin(locpos,0);
    
    for (int j=n-1; j<=n+1 ; j++) {
        if (j>=0 && j<max2) {
            int max1 = m_binvector->at(j)->bins(0);
            int m    = m_binvector->at(j)->bin(locpos,0);
            for (int i=m-1; i<=m+1; i++) {
                if (i>=0 && i<max1) {
                    unsigned long value = j+i+j*(max1-1);
                    bins.push_back(value);
                }
            }
        }
    }
    return (bins);
}


Alg::Point2D Trk::ReadoutSegmentation1D1D::binToLocpos(unsigned long bin) const
{
    size_t s = 0;
    size_t j = 0;
    size_t i = 0;
    for (size_t k=0; k<m_binutility->bins(0); k++) {
        size_t mk = m_binvector->at(k)->bins(0);
        s += k*mk;
        if (bin<=s) {
            j = k;
            i = mk-(s-bin);
        }
    }
    
    double c2 = m_binutility->bincenter(j,1);
    double c1 = m_binvector->at(j)->bincenter(i,0);
    
    return (Alg::Point2D(c1,c2));
}

float Trk::ReadoutSegmentation1D1D::binwidth(const Alg::Point2D& locpos, size_t ba) const
{
    size_t bin2 = m_binutility->bin(locpos, 0);
    if (ba == 0) {
        size_t bin1 = m_binvector->at(bin2)->bin(locpos,0);
        return (m_binvector->at(bin2)->binwidth(bin1,0));
    }
   return (m_binutility->binwidth(bin2, 0));
}



