#ifndef RECTRACKER_FASTHELIX_H
#define RECTRACKER_FASTHELIX_H

#include "GlobalPoint.h"
#include "FastCircle.h"
#include "FastLine.h"
#include <iostream>

/**
 *  Adapted from CMSSW via https://github.com/JuantAldea/FastFitters
 *  Generation of track parameters at a vertex using two hits and a vertex.
 *  It is used e.g. by a seed generator.
 *
 *  24.01.2012: introduced Maxpt cut. changed algo of "FastLine" to use vertex
 *  21.02.2001: Old FastHelix is now called FastHelixFit. Replace FastLineFit
 *              by FastLine (z0, dz/drphi calculated without vertex and errors)
 *  14.02.2001: Replace general Circle by FastCircle.
 *  13.02.2001: LinearFitErrorsInTwoCoordinates replaced by FastLineFit
 *  29.11.2000: (Pascal Vanlaer) Modification of calculation of sign of px,py
 *              and change in calculation of pz, z0.
 *  29.11.2000: (Matthias Winkler) Split stateAtVertex() in two parts (Circle
 *              is valid or not): helixStateAtVertex() and
 *                            straightLineStateAtVertex()
*/

class FastHelix {
public:
  FastHelix(const GlobalPoint& oHit, const GlobalPoint& mHit, const GlobalPoint& aVertex, double nomField)
      : m_theCircle(oHit, mHit, aVertex) {
    m_tesla0 = 0.1 * nomField;
    m_maxRho = maxPt / (0.01 * 0.3 * m_tesla0);
    m_useBasisVertex = false;
    compute();
  }

  bool isValid() const { return m_theCircle.isValid(); }

  const FastCircle& circle() const { return m_theCircle; }

  const GlobalPoint& getPt() const { return m_pt; }

private:
  GlobalPoint const& outerHit() const { return m_theCircle.outerPoint(); }
  GlobalPoint const& middleHit() const { return m_theCircle.innerPoint(); }
  GlobalPoint const& vertex() const { return m_theCircle.vertexPoint(); }

  void compute();
  void helixStateAtVertex();
  void straightLineStateAtVertex();

  static constexpr float maxPt = 10000;  // 10Tev

  GlobalPoint m_pt;

  GlobalPoint m_basisVertex;
  FastCircle m_theCircle;
  float m_tesla0;
  float m_maxRho;
  bool m_useBasisVertex;
  typedef int t_trackCharge;
};

void FastHelix::compute() {
  if (isValid() && (std::abs(m_tesla0) > 1e-3) && m_theCircle.rho() < m_maxRho) {
    helixStateAtVertex();
  } else {
    straightLineStateAtVertex();
  }
}

void FastHelix::helixStateAtVertex() {
  // given the above rho>0.
  double rho = m_theCircle.rho();
  // remember (radius rho in cm):
  // rho =
  // 100. * pt *
  //(10./(3.*MagneticField::inTesla(GlobalPoint(0., 0., 0.)).z()));
  // pt = 0.01 * rho * (0.3*MagneticField::inTesla(GlobalPoint(0.,0.,0.)).z());

  double cm2GeV = 0.01 * 0.3 * m_tesla0;
  double pt = cm2GeV * rho;

  // verify that rho is not too large
  // first calculate cos phi (where phi is angle between outer and middle hit)
  // by taking scalar product and
  double dcphi = ((outerHit().x() - m_theCircle.x0()) * (middleHit().x() - m_theCircle.x0()) +
                  (outerHit().y() - m_theCircle.y0()) * (middleHit().y() - m_theCircle.y0())) /
      (rho * rho);

  // if the scalar product is one, the points do not lie on a circle
  if (std::abs(dcphi) >= 1.f) {
    straightLineStateAtVertex();
    return;
  }

  // tangent in v (or the opposite...)
  double px = -cm2GeV * (vertex().y() - m_theCircle.y0());
  double py = cm2GeV * (vertex().x() - m_theCircle.x0());
  // check sign with scalar product
  if (px * (middleHit().x() - vertex().x()) + py * (middleHit().y() - vertex().y()) < 0.) {
    px = -px;
    py = -py;
  }

  // calculate z0, pz
  //(z, R*phi) linear relation in a helix
  // with R, phi defined as radius and angle w.r.t. centre of circle
  // in transverse plane
  // pz = pT*(dz/d(R*phi)))

  double dzdrphi = (outerHit().z() - middleHit().z()) / (rho * acos(dcphi));
  double pz = pt * dzdrphi;

  m_pt[0] = px;
  m_pt[1] = py;
  m_pt[2] = pz;
}

void FastHelix::straightLineStateAtVertex() {
  // calculate GlobalTrajectoryParameters assuming straight line...
  double dydx = 0.;
  double pt = 0., px = 0., py = 0.;

  if (fabs(m_theCircle.n1()) > 0. || fabs(m_theCircle.n2()) > 0.) {
    pt = maxPt;  // 10 TeV //else no pt
  }
  if (fabs(m_theCircle.n2()) > 0.) {
    dydx = -m_theCircle.n1() / m_theCircle.n2();  // else px = 0
  }
  px = pt / sqrt(1. + dydx * dydx);
  py = px * dydx;

  // check sign with scalar product
  if (px * (middleHit().x() - vertex().x()) + py * (middleHit().y() - vertex().y()) < 0.) {
    px *= -1.;
    py *= -1.;
  }

  // calculate z_0 and pz at vertex using weighted mean
  // z = z(r) = z0 + (dz/dr)*r
  // tan(theta) = dr/dz = (dz/dr)^-1
  // theta = atan(1./dzdr)
  // p = pt/sin(theta)
  // pz = p*cos(theta) = pt/tan(theta)

  FastLine flfit(outerHit(), middleHit());
  double dzdr = -flfit.n1() / flfit.n2();
  double pz = pt * dzdr;

  m_pt[0] = px;
  m_pt[1] = py;
  m_pt[2] = pz;
}

#endif
