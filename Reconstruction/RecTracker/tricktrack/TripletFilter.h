#ifndef TRICKTRACK_TRIPLETFILTER_H
#define TRICKTRACK_TRIPLETFILTER_H

#include <array>
#include <memory>




namespace tricktrack {
  template <typename Hit>
  class CMCell;

inline  bool areCompatibleTime(float ti, float tm, float to, float timeCut) {
    if ((tm < ti) || (to < tm)) {
      return false;
    }
    return( ((tm - ti) < timeCut) && ((to - tm) < timeCut));

  }



inline  int areAlignedRZ(float ri, float zi, float r1, float z1, float ro, float zo, const float ptmin, const float thetaCut)
  {
    float radius_diff = std::abs(r1 - ro);
    float distance_13_squared = radius_diff*radius_diff + (z1 - zo)*(z1 - zo);
    
    float pMin = ptmin*std::sqrt(distance_13_squared); //this needs to be divided by radius_diff later
    
    float tan_12_13_half_mul_distance_13_squared = fabs(z1 * (ri - ro) + zi * (ro - r1) + zo * (r1 - ri)) ;
    return tan_12_13_half_mul_distance_13_squared * pMin <= thetaCut * distance_13_squared * radius_diff;
  }

  /// check two cells for compatibility using the curvature in x-y plane
inline  bool haveSimilarCurvature(float x1, float y1, float x2, float y2, float x3, float y3, 
			    const float region_origin_x, const float region_origin_y, const float region_origin_radius, const float phiCut, const float hardPtCut, const float ptmin)
  {
    
     

    
    float distance_13_squared = (x1 - x3)*(x1 - x3) + (y1 - y3)*(y1 - y3);
    float tan_12_13_half_mul_distance_13_squared = std::abs(y1 * (x2 - x3) + y2 * (x3 - x1) + y3 * (x1 - x2)) ;
    // high pt : just straight
    if(tan_12_13_half_mul_distance_13_squared * ptmin <= 1.0e-4f*distance_13_squared)
      {
	
	float distance_3_beamspot_squared = (x3-region_origin_x) * (x3-region_origin_x) + (y3-region_origin_y) * (y3-region_origin_y);
	
	float dot_bs3_13 = ((x1 - x3)*( region_origin_x - x3) + (y1 - y3) * (region_origin_y-y3));
	float proj_bs3_on_13_squared = dot_bs3_13*dot_bs3_13/distance_13_squared;
	
	float distance_13_beamspot_squared  = distance_3_beamspot_squared -  proj_bs3_on_13_squared;
	
	    return distance_13_beamspot_squared < (region_origin_radius+phiCut)*(region_origin_radius+phiCut);
      }
    
    //87 cm/GeV = 1/(3.8T * 0.3)
    
    //take less than radius given by the hardPtCut and reject everything below
    float minRadius = hardPtCut*87.f;  // FIXME move out and use real MagField
    
    auto det = (x1 - x2) * (y2 - y3) - (x2 - x3) * (y1 - y2);
    
    
    auto offset = x2 * x2 + y2*y2;
    
    auto bc = (x1 * x1 + y1 * y1 - offset)*0.5f;
    
    auto cd = (offset - x3 * x3 - y3 * y3)*0.5f;
    
    
    
    auto idet = 1.f / det;
    
    auto x_center = (bc * (y2 - y3) - cd * (y1 - y2)) * idet;
    auto y_center = (cd * (x1 - x2) - bc * (x2 - x3)) * idet;
    
    auto radius = std::sqrt((x2 - x_center)*(x2 - x_center) + (y2 - y_center)*(y2 - y_center));
    
    if(radius < minRadius)  return false;  // hard cut on pt
    
    auto centers_distance_squared = (x_center - region_origin_x)*(x_center - region_origin_x) + (y_center - region_origin_y)*(y_center - region_origin_y);
    auto region_origin_radius_plus_tolerance = region_origin_radius + phiCut;
    auto minimumOfIntersectionRange = (radius - region_origin_radius_plus_tolerance)*(radius - region_origin_radius_plus_tolerance);
    
    if (centers_distance_squared >= minimumOfIntersectionRange) {
      auto maximumOfIntersectionRange = (radius + region_origin_radius_plus_tolerance)*(radius + region_origin_radius_plus_tolerance);
      return centers_distance_squared <= maximumOfIntersectionRange;
    } 
    
    return false;
    
  }


template <typename Hit>
  bool defaultGeometricFilter(const CMCell<Hit>& theInnerCell, const CMCell<Hit>& theOuterCell, const float ptmin, const float region_origin_x, const float region_origin_y, const float region_origin_radius, const float phiCut, const float hardPtCut, const float thetaCut) {

    auto x1 = theInnerCell.getInnerX();
    auto y1 = theInnerCell.getInnerY();
    
    auto x2 = theOuterCell.getInnerX();
    auto y2 = theOuterCell.getInnerY();
    
    auto x3 = theOuterCell.getOuterX();
    auto y3 = theOuterCell.getOuterY();

    auto ro = theOuterCell.getOuterR();
    auto zo = theOuterCell.getOuterZ();

    float r1 = theInnerCell.getInnerR();
    float z1 = theInnerCell.getInnerZ();

    float ri = theOuterCell.getInnerR();
    float zi = theOuterCell.getInnerZ();
    
    return areAlignedRZ(ri, zi, r1, z1, ro, zo, ptmin, thetaCut) && haveSimilarCurvature(x1, y1, x2, y2, x3, y3, region_origin_x, region_origin_y,
              region_origin_radius, phiCut, hardPtCut, ptmin);

  }
} // namespace tricktrack


#endif 
