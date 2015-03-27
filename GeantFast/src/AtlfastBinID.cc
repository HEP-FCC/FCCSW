#include "AtlfastBinID.hh"
#include <assert.h>

namespace Atlfast {

  /** @brief Stores bin limits for a given bin index.
   *
   * Works with any number of dimensions.
   */

/** Constructor for 1D bin*/
BinID::BinID(int intID, double low1, double high1): m_int(intID) {
  m_lowEdge.push_back(low1);
  m_highEdge.push_back(high1);
}
/** Constructor for 2D bin */
BinID::BinID(int intID, double low1, double high1, double low2,double high2):
  m_int(intID) {
  m_lowEdge.push_back(low1);
  m_lowEdge.push_back(low2);
  m_highEdge.push_back(high1);
  m_highEdge.push_back(high2);
}
/** Constructor for any-dimensional bin */
BinID::BinID(int intID, std::vector<double> lowV, std::vector<double> highV):
  m_int(intID), m_lowEdge(lowV), m_highEdge(highV) {}

/** Returns low edge of bin in nth dimension */
double BinID::low(int n) const {return m_lowEdge[n];}
/** Returns high edge of bin in nth dimension */
double BinID::high(int n) const {return m_highEdge[n];}
/** Checks whether value is in between bin limits: 1D */
bool BinID::isInBin(const double& var) const{
  std::vector<double> vec(1, var);
  return this->isInBin(vec);
}
/** Checks whether value is in between bin limits: any-dimensional */
bool BinID::isInBin(const std::vector<double>& var) const{
  std::vector<double>::const_iterator vIter = var.begin();
  std::vector<double>::const_iterator vEnd = var.end();
  std::vector<double>::const_iterator lowIter = m_lowEdge.begin();
  std::vector<double>::const_iterator highIter = m_highEdge.begin();

  assert(var.size() <=m_lowEdge.size() );

  for (;vIter != vEnd; ++vIter) {
    if ( (*vIter) < (*lowIter) || (*vIter) > (*highIter) ) {
      return false;
    }
    ++lowIter;
    ++highIter;
  }
  return true;
}

 /** Is this bin index lower than the input bin's index? */
bool BinID::operator<(const BinID& other) const {
  return (m_int < other.m_int);
}

}
