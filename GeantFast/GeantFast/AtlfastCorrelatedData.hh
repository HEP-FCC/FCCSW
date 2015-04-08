#ifndef ATLFAST_CORRELATEDDATA_H
#define ATLFAST_CORRELATEDDATA_H

#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Matrix/Matrix.h"
#include <vector>
#include <utility>

namespace Atlfast{
  using std::vector;
  using std::pair;
  using ::CLHEP::HepMatrix;

  /** @brief Class to smear parameters that are correlated. */
  class CorrelatedData{
  public:

    /** Constructor accepts random seed */
    CorrelatedData(int);
    /** Destructor */
    ~CorrelatedData() {delete m_randomEngine;}

    /** Generates smeared parameters from input matrix */
    vector<double> generate(const CLHEP::HepMatrix& matrix) const;
    /** Generates smeared parameter from input number */
    double generate(double) const;

    /** Returns square root matrix of original matrix */
    CLHEP::HepMatrix root(const CLHEP::HepMatrix& matrix)const;


    //private:
    pair<double,double> m_ellipse;
    pair<double,double> m_stFactors;
    pair<double,double> m_abFactors;
    CLHEP::HepRandomEngine*    m_randomEngine;

    vector<double> normal(int nDev)const;
    bool makeDeviate(pair<double,double> randoms,
		     double& deviate)const;

  };

}//end of namespace

#endif


