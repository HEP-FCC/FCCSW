//
//  ReadoutSegmentation2D.cxx
//  
//
//  Created by Julia Hrdinka on 24/04/15.
//
//

#include "RecoGeometry/ReadoutSegmentation2D.h"

Reco::ReadoutSegmentation2D::ReadoutSegmentation2D(Trk::BinUtility* binutil) :
Reco::ReadoutSegmentation(),
m_binutility(binutil)
{
    if (m_binutility->dimension()!=2) throw GaudiException("ReadoutSegmentation2D", "Dimension has to be 2 for this Semgentationtype", StatusCode::FAILURE);
}

Reco::ReadoutSegmentation2D::ReadoutSegmentation2D(const Reco::ReadoutSegmentation2D& seg) :
Reco::ReadoutSegmentation(seg),
m_binutility(new Trk::BinUtility(*seg.m_binutility))
{

}

Reco::ReadoutSegmentation2D::~ReadoutSegmentation2D()
{
    delete m_binutility;
}

Reco::ReadoutSegmentation2D* Reco::ReadoutSegmentation2D::clone() const
{
    return(new Reco::ReadoutSegmentation2D(*this));
}

Reco::ReadoutSegmentation2D& Reco::ReadoutSegmentation2D::operator=(const Reco::ReadoutSegmentation2D& seg)
{
    if (this!=&seg) {
        Reco::ReadoutSegmentation::operator=(seg);
        delete m_binutility;
        m_binutility = new Trk::BinUtility(*seg.m_binutility);
    }
    
    return (*this);
}

size_t Reco::ReadoutSegmentation2D::bins() const
{
    return ((m_binutility->bins(0))*(m_binutility->bins(1)));
}

const unsigned long Reco::ReadoutSegmentation2D::bin(const Alg::Point2D& locpos) const
{
    unsigned long i = m_binutility->bin(locpos,0);
    unsigned long j = m_binutility->bin(locpos,1);
    unsigned long m = m_binutility->bins(0);
    return(i+j*m);
}

const std::vector<unsigned long> Reco::ReadoutSegmentation2D::compatibleBins(const Alg::Point2D& locpos) const
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
                    unsigned long value = i+j*max1;
                    bins.push_back(value);
                }
            }
        }
    }
    return (bins);
}

Alg::Point2D Reco::ReadoutSegmentation2D::binToLocpos(unsigned long bin) const
{
    
    size_t m = m_binutility->bins(0);
    size_t f = bin/m;
    float c1 = m_binutility->bincenter(bin-f*m,0);
    float c2 = m_binutility->bincenter(bin,1);
    
    return (Alg::Point2D(c1,c2));
}

float Reco::ReadoutSegmentation2D::binwidth(const Alg::Point2D& locpos, size_t ba) const
{
   return (m_binutility->binwidth(m_binutility->bin(locpos,ba), ba));
}







