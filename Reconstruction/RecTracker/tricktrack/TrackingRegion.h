#ifndef TRACKSEEDTOOLS_TRACKINGREGION_H
#define TRACKSEEDTOOLS_TRACKINGREGION_H


namespace tricktrack {

/** @class TrackingRegion
 *
 * gathers detector geometry information on luminous region for geometric constraints during track seeding.
 */
class TrackingRegion {

public:
  TrackingRegion(double ptMin, double region_origin_x, double region_origin_y, double region_origin_radius) : m_ptMin(ptMin), m_region_origin_x(region_origin_x), m_region_origin_y(region_origin_y), m_region_origin_radius(region_origin_radius) {};

  double ptMin() const { return m_ptMin;}
  double originX() const {return m_region_origin_x;}
  double originY() const {return m_region_origin_y;}
  double originRBound() const {return m_region_origin_radius;}


private:

  double m_ptMin;
  double m_region_origin_x;
  double m_region_origin_y;
  double m_region_origin_radius;

};

}

#endif
