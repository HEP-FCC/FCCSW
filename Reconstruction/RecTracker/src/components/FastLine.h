#ifndef RECTRACKER_FASTLINE_H
#define RECTRACKER_FASTLINE_H

#include <ctgmath>
#include <tuple>

#include "GlobalPoint.h"

/**
 *  Calculate the Line parameters (n1, n2, c) of a Line in Rho*Phi-Z.
 *
 *  A Line is defined by n1 * x + n2 * y + c = 0. (== n1 * RHOPHI + n2 * Z + c)
 *
 *  If rho is not specified, the line parameters are calculated in R-Z.
 *
 *  Implementation: Matthias Winkler 21.02.2001
 */

std::tuple<double, double, double, bool> FastLineFit(GlobalPoint innerHit, GlobalPoint outerHit, double rho);

std::tuple<double, double, double, bool> FastLineFit(GlobalPoint innerHit, GlobalPoint outerHit, double rho) {
  double rphi0 = 0.;
  double rphi1 = 0.;

  if (rho > 0.) {
    if (fabs(1. - perp2(innerHit) / (2. * rho * rho)) > 1.) {
      rphi0 = perp(innerHit);
    } else {
      rphi0 = rho * acos(1. - perp2(innerHit) / (2. * rho * rho));
    }

    if (fabs(1. - perp2(outerHit) / (2. * rho * rho)) > 1.) {
      rphi1 = perp(outerHit);
    } else {
      rphi1 = rho * acos(1. - perp2(outerHit) / (2. * rho * rho));
    }
  } else {
    rphi0 = perp(innerHit);
    rphi1 = perp(outerHit);
  }

  double n1 = innerHit.z() - outerHit.z();
  double n2 = -(rphi0 - rphi1);
  double norm = sqrt(n1 * n1 + n2 * n2);

  double N1 = n1 / norm;
  double N2 = n2 / norm;
  double C = -(N1 * rphi0 + N2 * innerHit.z());

  bool fittingValid = true;

  return std::make_tuple(N1, N2, C, fittingValid);
}

class FastLine {

public:
  FastLine(const GlobalPoint& outerHit, const GlobalPoint& innerHit)
      : outerHit_(outerHit), innerHit_(innerHit), rho_(0.), n1_(0.), n2_(0.), c_(0.), fittingValid_(true) {
    auto fit = FastLineFit(innerHit, outerHit, rho_);
    n1_ = std::get<0>(fit);
    n2_ = std::get<1>(fit);
    c_ = std::get<2>(fit);
    fittingValid_ = std::get<3>(fit);
  }

  FastLine(const GlobalPoint& outerHit, const GlobalPoint& innerHit, double rho)
      : outerHit_(outerHit), innerHit_(innerHit), rho_(rho), n1_(0.), n2_(0.), c_(0.), fittingValid_(true) {
    auto fit = FastLineFit(innerHit, outerHit, rho_);
    n1_ = std::get<0>(fit);
    n2_ = std::get<1>(fit);
    c_ = std::get<2>(fit);
    fittingValid_ = std::get<3>(fit);
  }

  double n1() const { return n1_; }

  double n2() const { return n2_; }

  double c() const { return c_; }

  bool isValid() const { return fittingValid_; }

private:
  GlobalPoint outerHit_;
  GlobalPoint innerHit_;
  double rho_;

  double n1_;
  double n2_;
  double c_;

  bool fittingValid_;
};

#endif
