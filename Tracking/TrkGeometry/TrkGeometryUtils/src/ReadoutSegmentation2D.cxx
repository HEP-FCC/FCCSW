//
//  ReadoutSegmentation2D.cxx
//  
//
//  Created by Julia Hrdinka on 24/04/15.
//
//

#include "TrkGeometryUtils/ReadoutSegmentation2D.h"

Trk::ReadoutSegmentation2D::ReadoutSegmentation2D(Trk::BinUtility* binutil) :
Trk::ReadoutSegmentation(),
m_binutility(binutil)
{}

Trk::ReadoutSegmentation2D::ReadoutSegmentation2D(const Trk::ReadoutSegmentation2D& seg) :
Trk::ReadoutSegmentation(seg),
m_binutility(new Trk::BinUtility(*seg.m_binutility))
{

}

Trk::ReadoutSegmentation2D::~ReadoutSegmentation2D()
{
    delete m_binutility;
}

Trk::ReadoutSegmentation2D* Trk::ReadoutSegmentation2D::clone() const
{
    return(new Trk::ReadoutSegmentation2D(*this));
}

Trk::ReadoutSegmentation2D& Trk::ReadoutSegmentation2D::operator=(const Trk::ReadoutSegmentation2D& seg)
{
    if (this!=&seg) {
        Trk::ReadoutSegmentation::operator=(seg);
        delete m_binutility;
        m_binutility = new Trk::BinUtility(*seg.m_binutility);
    }
    
    return (*this);
}

size_t Trk::ReadoutSegmentation2D::bins() const
{
    return ((m_binutility->bins(0))*(m_binutility->bins(1)));
}

const unsigned long Trk::ReadoutSegmentation2D::bin(const Alg::Point2D& locpos) const
{
    unsigned long i = m_binutility->bin(locpos,0);
    unsigned long j = m_binutility->bin(locpos,1);
    unsigned long m = m_binutility->bins(0);
    return(i+j*m);
}

const std::vector<unsigned long> Trk::ReadoutSegmentation2D::compatibleBins(const Alg::Point2D& locpos) const
{
    std::vector<unsigned long> bins;
    int max1 = m_binutility->bins(0);
    int max2 = m_binutility->bins(1);
    int m = m_binutility->bin(locpos,0);
    int n = m_binutility->bin(locpos,1);
    for (int i=m-1; i<=m+1 ; i++) {
        if (i>=0 && i<max1) {
            for (int j=n-1; j<=n+1; j++) {
                if (j>=0 && j<max2) {
                    unsigned long value = j+i+j*(max1-1);
                    bins.push_back(value);
                }
            }
        }
    }
    return (bins);
}

Alg::Point2D Trk::ReadoutSegmentation2D::binToLocpos(unsigned long bin) const
{
    
    size_t m = m_binutility->bins(0);
    size_t f = bin/m;
    float c1 = m_binutility->bincenter(bin-f*m,0);
    float c2 = m_binutility->bincenter(bin,1);
    
    return (Alg::Point2D(c1,c2));
}

float Trk::ReadoutSegmentation2D::binwidth(const Alg::Point2D& locpos, size_t ba) const
{
   return (m_binutility->binwidth(m_binutility->bin(locpos,ba), ba));
}







