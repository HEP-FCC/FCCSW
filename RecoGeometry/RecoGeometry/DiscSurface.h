//
//  DiscSurface.h
//  
//
//  Created by Julia Hrdinka on 02/12/14.
//
//

#ifndef RECO_DISCSURFACE_H
#define RECO_DISCSURFACE_H

#include "RecoGeometry/Surface.h"
#include "TGeoCone.h"

#include "TrkParametersBase/ParametersT.h"

namespace Reco {
    
    class DiscSurface : public Surface {
        
    public:
        //constructor from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //material is set atomatically (node->Material)
        DiscSurface(TGeoNode* node, TGeoConeSeg* tube);
        DiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf);
        //constructor to set Material extra
        DiscSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap);
        DiscSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf);
        //manuel constructor with transform matrix and dimensions of the Disc
        DiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, double HalfThickness = 0.);
        //copy constructor
        DiscSurface(const DiscSurface& discsurface);
        //copy constructor
        virtual ~DiscSurface();
        //clone method
        virtual DiscSurface* clone() const override;
        //assignment operator
        DiscSurface& operator=(const DiscSurface& discsurface);
        //get dimensions
        double getRmin() const;
        double getRmax() const;
        virtual double halfThickness() const override;
        //get the normal vector of the surface
        virtual const Alg::Vector3D& normal() const override;
        // normal vector to local position
        virtual const Alg::Vector3D* normal(const Alg::Point2D& locpos) const override;
        //get Material on a local position of the surface
        virtual const Material* material(Alg::Point2D& locpos) const override;
        //checks if local position locpos is inside the bounds of the surface
        virtual bool isInside(const Alg::Point2D& locpos, double tol1, double tol2) const override;
        //checks if local position locpos is Inside bounds, with the two tolerances
        virtual void localToGlobal(const Alg::Point2D& locpos, const Alg::Vector3D& mom, Alg::Point3D& glopos) const override;
        //converts local position in global position
        virtual bool globalToLocal(const Alg::Point3D& glopos, const Alg::Vector3D& mom, Alg::Point2D& locpos) const override;
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const override;
        //returns the pathlength
        virtual double pathlength(const Alg::Point3D& pos, const Alg::Vector3D& dir) const override;
        
        
        /** Use the Surface as a ParametersBase constructor, from local parameters - charged */
        virtual const Trk::ParametersT<5, Trk::Charged, DiscSurface>* createTrackParameters(double l1,
                                                                                                double l2,
                                                                                                double phi,
                                                                                                double theta,
                                                                                                double qop,
                                                                                                Alg::AmgSymMatrix<5>* cov = 0) const override
        { return new Trk::ParametersT<5, Trk::Charged, DiscSurface>(l1, l2, phi, theta, qop, *this, cov); }
        
        
        /** Use the Surface as a ParametersBase constructor, from global parameters - charged*/
        virtual const Trk::ParametersT<5, Trk::Charged, DiscSurface>* createTrackParameters(const Alg::Point3D& position,
                                                                                                const Alg::Vector3D& momentum,
                                                                                                double charge,
                                                                                                Alg::AmgSymMatrix<5>* cov = 0) const override
        { return new Trk::ParametersT<5, Trk::Charged, DiscSurface>(position, momentum, charge, *this, cov); }
        
        /** Use the Surface as a ParametersBase constructor, from local parameters - neutral */
        virtual const Trk::ParametersT<5, Trk::Neutral, DiscSurface>* createNeutralParameters(double l1,
                                                                                                  double l2,
                                                                                                  double phi,
                                                                                                  double theta,
                                                                                                  double qop,
                                                                                                  Alg::AmgSymMatrix<5>* cov = 0) const
        { return new Trk::ParametersT<5, Trk::Neutral, DiscSurface>(l1, l2, phi, theta, qop, *this, cov); }
        
        /** Use the Surface as a ParametersBase constructor, from global parameters - neutral */
        virtual const Trk::ParametersT<5, Trk::Neutral, DiscSurface>* createNeutralParameters(const Alg::Point3D& position,
                                                                                                  const Alg::Vector3D& momentum,
                                                                                                  double charge,
                                                                                                  Alg::AmgSymMatrix<5>* cov = 0) const override
        { return new Trk::ParametersT<5, Trk::Neutral, DiscSurface>(position, momentum, charge, *this, cov); }
        
        
        /** Use the Surface as a ParametersBase constructor, from local parameters */
        template <int DIM, class T> const Trk::ParametersT<DIM, T, DiscSurface>* createParameters(double l1,
                                                                                                      double l2,
                                                                                                      double phi,
                                                                                                      double theta,
                                                                                                      double qop,
                                                                                                      Alg::AmgSymMatrix<DIM>* cov = 0) const override
        { return new Trk::ParametersT<DIM, T, DiscSurface>(l1, l2, phi, theta, qop, *this, cov); }
        
        /** Use the Surface as a ParametersBase constructor, from global parameters */
        template <int DIM, class T> const Trk::ParametersT<DIM, T, DiscSurface>* createParameters(const Alg::Point3D& position,
                                                                                                      const Alg::Vector3D& momentum,
                                                                                                      double charge,
                                                                                                      Alg::AmgSymMatrix<DIM>* cov = 0) const override
        { return new Trk::ParametersT<DIM, T, DiscSurface>(position, momentum, charge, *this, cov); }
        
        /** fast straight line intersection schema - standard: provides closest intersection and (signed) path length
         forceDir is to provide the closest forward solution
         
         <b>mathematical motivation:</b>
         
         the equation of the plane is given by: <br>
         @f$ \vec n \cdot \vec x = \vec n \cdot \vec p,@f$ <br>
         where @f$ \vec n = (n_{x}, n_{y}, n_{z})@f$ denotes the normal vector of the plane,
         @f$ \vec p = (p_{x}, p_{y}, p_{z})@f$ one specific point on the plane and @f$ \vec x = (x,y,z) @f$ all possible points
         on the plane.<br>
         Given a line with:<br>
         @f$ \vec l(u) = \vec l_{1} + u \cdot \vec v @f$, <br>
         the solution for @f$ u @f$ can be written:
         @f$ u = \frac{\vec n (\vec p - \vec l_{1})}{\vec n \vec v}@f$ <br>
         If the denominator is 0 then the line lies:
         - either in the plane
         - perpenticular to the normal of the plane
         */
        
        virtual Trk::Intersection straightLineIntersection(const Alg::Point3D& pos,
                                                           const Alg::Vector3D& dir,
                                                           bool forceDir = false) const override;
        
    protected:
        
        DiscSurface();
        
        double m_Rmin;
        double m_Rmax;
        //halfthickness
        double m_halfThickness;
    };
    
    inline Trk::Intersection DiscSurface::straightLineIntersection(const Alg::Point3D& pos,
                                                                    const Alg::Vector3D& dir,
                                                                    bool forceDir) const
    {
        double denom = dir.Dot(normal());
        if (denom){
            double u = (normal().Dot((center()-pos)))/(denom);
            Alg::Point3D intersectPoint(pos + u * dir);
            // evaluate the intersection in terms of direction
            bool isValid = forceDir ?  ( u > 0.) : true;
            //check, if on surface
            Alg::Point2D loc;
            if (isValid) isValid = (globalToLocal(intersectPoint,dir,loc));
            // return the result
            return Trk::Intersection(intersectPoint,u,isValid);
        }
        return Trk::Intersection(pos,0.,false);
    }
    
}



#endif //RECO_DISCSURFACE_H
