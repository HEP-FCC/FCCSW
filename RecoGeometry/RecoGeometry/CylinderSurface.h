//
//  CylinderSurface.h
//  
//
//  Created by Julia Hrdinka on 25/11/14.
//
//

#ifndef RECO_CYLINDERSURFACE_H
#define RECO_CYLINDERSURFACE_H

#include "RecoGeometry/Surface.h"
#include "TGeoCone.h"

#include "TrkParametersBase/ParametersT.h"

namespace Reco {
    
    class CylinderSurface : public Surface {
    
    public:
        //constructor from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //material is set atomatically (node->Material)
        CylinderSurface(TGeoNode* node, TGeoConeSeg* tube);
        CylinderSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf);
        //constructor to set Material extra
        CylinderSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap);
        CylinderSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf);
        //constructor with transform matrix and dimensions of the cylinder
        CylinderSurface(std::shared_ptr<const Alg::Transform3D> transf, double radius, double halfZ);
        //constructor with rmin and rmax
        CylinderSurface(double radius, double halfZ);
        //copy constructor
        CylinderSurface(const CylinderSurface& cylindersurface);
        //destructor
        virtual ~CylinderSurface();
        //clone method
        virtual CylinderSurface* clone() const override;
        //assignment operator
        CylinderSurface& operator=(const CylinderSurface& cylindersurface);
        //get dimensions
        double getR() const;
        double getHalfZ() const;
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
        
        
        /** Use the Surface as a ParametersBase constructor, from local parameters - charged */
        virtual const Trk::ParametersT<5, Trk::Charged, CylinderSurface>* createTrackParameters(double l1,
                                                                                                double l2,
                                                                                                double phi,
                                                                                                double theta,
                                                                                                double qop,
                                                                                                Alg::AmgSymMatrix<5>* cov = 0) const override
        { return new Trk::ParametersT<5, Trk::Charged, CylinderSurface>(l1, l2, phi, theta, qop, *this, cov); }
        
        
        /** Use the Surface as a ParametersBase constructor, from global parameters - charged*/
        virtual const Trk::ParametersT<5, Trk::Charged, CylinderSurface>* createTrackParameters(const Alg::Point3D& position,
                                                                                                const Alg::Vector3D& momentum,
                                                                                                double charge,
                                                                                                Alg::AmgSymMatrix<5>* cov = 0) const override
        { return new Trk::ParametersT<5, Trk::Charged, CylinderSurface>(position, momentum, charge, *this, cov); }
        
        /** Use the Surface as a ParametersBase constructor, from local parameters - neutral */
        virtual const Trk::ParametersT<5, Trk::Neutral, CylinderSurface>* createNeutralParameters(double l1,
                                                                                                  double l2,
                                                                                                  double phi,
                                                                                                  double theta,
                                                                                                  double qop,
                                                                                                  Alg::AmgSymMatrix<5>* cov = 0) const
        { return new Trk::ParametersT<5, Trk::Neutral, CylinderSurface>(l1, l2, phi, theta, qop, *this, cov); }
        
        /** Use the Surface as a ParametersBase constructor, from global parameters - neutral */
        virtual const Trk::ParametersT<5, Trk::Neutral, CylinderSurface>* createNeutralParameters(const Alg::Point3D& position,
                                                                                                  const Alg::Vector3D& momentum,
                                                                                                  double charge,
                                                                                                  Alg::AmgSymMatrix<5>* cov = 0) const override
        { return new Trk::ParametersT<5, Trk::Neutral, CylinderSurface>(position, momentum, charge, *this, cov); }
        
        
        /** Use the Surface as a ParametersBase constructor, from local parameters */
        template <int DIM, class T> const Trk::ParametersT<DIM, T, CylinderSurface>* createParameters(double l1,
                                                                                                      double l2,
                                                                                                      double phi,
                                                                                                      double theta,
                                                                                                      double qop,
                                                                                                      Alg::AmgSymMatrix<DIM>* cov = 0) const override
        { return new Trk::ParametersT<DIM, T, CylinderSurface>(l1, l2, phi, theta, qop, *this, cov); }
        
        /** Use the Surface as a ParametersBase constructor, from global parameters */
        template <int DIM, class T> const Trk::ParametersT<DIM, T, CylinderSurface>* createParameters(const Alg::Point3D& position,
                                                                                                      const Alg::Vector3D& momentum,
                                                                                                      double charge,
                                                                                                      Alg::AmgSymMatrix<DIM>* cov = 0) const override
        { return new Trk::ParametersT<DIM, T, CylinderSurface>(position, momentum, charge, *this, cov); }
        
        /** fast straight line intersection schema - provides closest intersection and (signed) path length
         
         <b>mathematical motivation:</b>
         
         The calculation will be done in the 3-dim frame of the cylinder,
         i.e. the symmetry axis of the cylinder is the z-axis, x- and y-axis are perpenticular
         to the the z-axis. In this frame the cylinder is centered around the origin.
         Therefore the two points describing the line have to be first recalculated into the new frame.
         Suppose, this is done, the intersection is straight forward:<br>
         may @f$p_{1}=(p_{1x}, p_{1y}, p_{1z}), p_{2}=(p_{2x}, p_{2y}, p_{2z}) @f$the two points describing the 3D-line,
         then the line in the \f$x-y@f$plane can be written as
         @f$y=kx+d\f$, where @f$k =\frac{p_{2y}-p_{1y}}{p_{2x}-p_{1x}}@f$such as @f$d=\frac{p_{2x}p_{1y}-p_{1x}p_{2y}}{p_{2x}-p_{1x}},\f$<br>
         and intersects with the corresponding circle @f$x^{2}+y^{2} = R^{2}. @f$<br>
         The solutions can then be found by a simple quadratic equation and reinsertion into the line equation.
         */
         virtual Trk::Intersection straightLineIntersection(const Alg::Point3D& pos,
         const Alg::Vector3D& dir,
         bool forceDir = false) const override;
        
    protected:
        
        CylinderSurface();
        
        double m_R;
        double m_halfZ;
    };
}


#endif //RECO_CYLINDERSURFACE_H
