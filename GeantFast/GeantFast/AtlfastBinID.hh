#ifndef ATLFAST_BINID_H
#define ATLFAST_BINID_H

#ifndef STD_VECTOR_H
#include <vector>
#define STD_VECTOR_H
#endif

namespace Atlfast{

  /** @brief Stores bin limits for a given bin index. 
   *
   * Works with any number of dimensions.
   */

  class BinID{
    
  public:

    /** Default constructor */
    BinID(): m_int(0),m_lowEdge(1, 0.),m_highEdge(1, 0.){}
    /** Constructor for 1D bin*/
    BinID(int intID, double low1, double high1);    
    /** Constructor for 2D bin */
    BinID(int intID, double low1, double high1, double low2,double high2);
    /** Constructor for any-dimensional bin */
    BinID(int intID, std::vector<double> lowV, std::vector<double> highV);
 
    /** Returns low edge of bin in nth dimension */
    double  low(int n=0) const;
    /** Returns high edge of bin in nth dimension */
    double high(int n=0) const;

    /** Checks whether value is in between bin limits: 1D */
    bool isInBin(const double& var) const;
    /** Checks whether value is in between bin limits: any-dimensional */
    bool isInBin(const std::vector<double>& var) const;

    /** Is this bin index lower than the input bin's index? */
    bool operator<(const BinID& other) const; 
  private:
    int m_int;
    std::vector<double> m_lowEdge;
    std::vector<double> m_highEdge;
  };
}//namepace
#endif
  





