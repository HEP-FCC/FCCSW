#ifndef RECTRACKER_FASTCIRCLE_H
#define RECTRACKER_FASTCIRCLE_H

#include <tuple>
#include <ctgmath>

#include "GlobalPoint.h"

/**
 *  Adapted from CMSSW via  https://github.com/JuantAldea/FastFitters
 *  Calculate circle parameters (x0, y0, rho) for a circle:
 *  (x-x0)^2 + (y-y0)^2 = rho^2
 *  in Global Cartesian Coordinates in the (x,y) plane for a given set of
 *  GlobalPoints. It is done by mapping the points onto the Riemann Sphere
 *  and fit a plane to the transformed coordinates of the points.
 *  The method is described in:
 *
 *  A.Strandlie, J.Wroldsen, R.Fruehwirth, B.Lillekjendlie:
 *  Particle tracks fitted on the Riemann sphere
 *  Computer Physics Communications 131 (2000) 95-108, 18 January 2000
 *
 *  Implementation: Matthias Winkler, 14 February 2001
 *
 *  This implementation is a specialized version of the general Circle class
 *  for three points.
 *
 *  Update 14.02.2001:
 *       For 3 Points (2 RecHits + Vertex) the plain parameters
 *   n1*x + n2*y + n3*z + c = 0
 *   are analytically calculable.
 *
 *  Update 14.02.2001:
 *       In the case that a circle fit is not possible (points
 *  are along a straight line) the parameters of the
 *  straight line can be used: c + n1*x + n2*y = 0
*/

std::tuple<double, double, double, double, double, double, bool>
fastRiemannCircleFit(GlobalPoint outer, GlobalPoint inner, GlobalPoint vertex, float norm);

std::tuple<double, double, double, double, double, double, bool>
fastRiemannCircleFit(GlobalPoint outer, GlobalPoint inner, GlobalPoint vertex, float norm) {
  // GlobalPoint x, y, z;
  GlobalPoint p;

  float R2, fact;

  p[0] = outer.x() / norm;
  p[1] = outer.y() / norm;
  p[2] = outer.z() / norm;

  R2 = perp2(p);
  fact = 1.f / (1.f + R2);
  GlobalPoint x(fact * p.x(), fact * p.y(), fact * R2);

  p[0] = inner.x() / norm;
  p[1] = inner.y() / norm;
  p[2] = inner.z() / norm;

  R2 = perp2(p);
  fact = 1.f / (1.f + R2);
  GlobalPoint y(fact * p.x(), fact * p.y(), fact * R2);

  p[0] = vertex.x() / norm;
  p[1] = vertex.y() / norm;
  p[2] = vertex.z() / norm;

  R2 = perp2(p);
  fact = 1.f / (1.f + R2);
  GlobalPoint z(fact * p.x(), fact * p.y(), fact * R2);

  GlobalPoint n(x[1] * (y[2] - z[2]) + y[1] * (z[2] - x[2]) + z[1] * (x[2] - y[2]),
                -(x[0] * (y[2] - z[2]) + y[0] * (z[2] - x[2]) + z[0] * (x[2] - y[2])),
                x[0] * (y[1] - z[1]) + y[0] * (z[1] - x[1]) + z[0] * (x[1] - y[1]));

  double mag2 = length2(n);

  // one sqrt here
  n = normalize(n);

  double c = -dot(n, x);

  double x0 = -n[0] / (2. * (c + n[2]));
  double y0 = -n[1] / (2. * (c + n[2]));

  // another sqrt here
  double rho = sqrt((n[0] * n[0] + n[1] * n[1] - 4. * c * (c + n[2]))) / fabs(2. * (c + n[2]));

  bool fittingValid = (mag2 >= 1.e-20) * (fabs(c + n[2]) >= 1.e-5);
  double n1 = n[0];
  double n2 = n[1];

  double X0 = norm * x0;
  double Y0 = norm * y0;
  double Rho = norm * rho;

  return std::make_tuple(X0, Y0, Rho, n1, n2, c, fittingValid);
}

class FastCircle {

public:
  FastCircle(const GlobalPoint& outerHit, const GlobalPoint& middleHit, const GlobalPoint& aVertex, double norm = 128.)
      : outerPoint_(outerHit),
        innerPoint_(middleHit),
        vertexPoint_(aVertex),
        norm_(norm),
        x0_(0.),
        y0_(0.),
        rho_(0.),
        n1_(0.),
        n2_(0.),
        c_(0.),
        fittingValid_(true) {
    auto parameters = fastRiemannCircleFit(outerHit, middleHit, aVertex, float(norm_));
    x0_ = std::get<0>(parameters);
    y0_ = std::get<1>(parameters);
    rho_ = std::get<2>(parameters);
    n1_ = std::get<3>(parameters);
    n2_ = std::get<4>(parameters);
    c_ = std::get<5>(parameters);
    fittingValid_ = std::get<6>(parameters);
  }

  // all returned values have dimensions of cm
  // parameters of the circle (circle is valid)
  double x0() const { return x0_; }

  double y0() const { return y0_; }

  double rho() const { return rho_; }

  bool isValid() const { return fittingValid_; }

  // parameters of the straight line
  // (if circle is invalid only these are available)
  double n1() const { return n1_; }

  double n2() const { return n2_; }

  double c() const { return c_; }

  GlobalPoint const& outerPoint() const { return outerPoint_; }
  GlobalPoint const& innerPoint() const { return innerPoint_; }
  GlobalPoint const& vertexPoint() const { return vertexPoint_; }

private:
  GlobalPoint outerPoint_;
  GlobalPoint innerPoint_;
  GlobalPoint vertexPoint_;

  double norm_;

  double x0_;
  double y0_;
  double rho_;

  double n1_;
  double n2_;
  double c_;

  bool fittingValid_;
};

#endif
