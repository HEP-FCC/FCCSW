//
//  Surface.h
//  
//
//  Created by Julia Hrdinka on 25/11/14.
//
//

#ifndef RECO_SURFACE_H
#define RECO_SURFACE_H

#include "Algebra/AlgPrimitives.h"
#include "TGeoNode.h"

#include "TrkParametersBase/ParametersBase.h"
#include "TrkParametersBase/Charged.h"
#include "TrkParametersBase/Neutral.h"

#include "TrkGeometryUtils/Intersection.h"

#include "RecoGeometry/Material.h"
#include "RecoGeometry/MaterialMap.h"

namespace Reco {
    
    class Surface {
        
    public:
        //constructors from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //constructor with TGeoNode - transform is set automaticall
        //constructor only with TGeoNode, creates a Surface without material, is used only for layers
        Surface (TGeoNode* node);
        //standard constructor with TGeoNode and MateriaMap
        Surface (TGeoNode* node, MaterialMap* materialmap);
        //constructor to set transform manually
        Surface (MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf);
        Surface (std::shared_ptr<const Alg::Transform3D> transf);
        //copy constructor
        Surface (const Surface& surface);
        //destructor
        virtual ~Surface();
        //clone method
        virtual Surface* clone() const = 0;
        //assignment operator
        Surface& operator=(const Surface& surface);
        //get transform matrix - returns identity transformation if not set
        const Alg::Transform3D& transform() const;
        //get the center of the surface - returns origin if not set
        const Alg::Point3D& center() const;
        //get the normal vector of the surface
        virtual const Alg::Vector3D& normal() const;
        //normal vector to local position
        virtual const Alg::Vector3D* normal(const Alg::Point2D& locpos) const;
        //get the MaterialMap of the Surface
        const MaterialMap* materialmap() const;
        //get a pointer to the material on a local position of the surface
        virtual const Material* material(Alg::Point2D& locpos) const = 0;
        //checks if local position locpos is Inside bounds, with the two tolerances
        virtual bool isInside(const Alg::Point2D& locpos, double tol1, double tol2) const = 0;
        //converts local position in global position
        virtual void localToGlobal(const Alg::Point2D& locpos, const Alg::Vector3D& mom, Alg::Point3D& glopos) const = 0;
        //converts global position in local position and checks whether the result is on the surface or not
        virtual bool globalToLocal(const Alg::Point3D& glopos, const Alg::Vector3D& mom, Alg::Point2D& locpos) const = 0;
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const = 0;
        /** Use the Surface as a ParametersBase constructor, from local parameters - charged */
        virtual const Trk::ParametersBase<5, Trk::Charged>* createTrackParameters(double, double, double, double, double, Alg::AmgSymMatrix<5>* cov = 0) const = 0;
        
        /** Use the Surface as a ParametersBase constructor, from global parameters - charged*/
        virtual const Trk::ParametersBase<5, Trk::Charged>* createTrackParameters(const Alg::Point3D&, const Alg::Vector3D&, double, Alg::AmgSymMatrix<5>* cov = 0) const = 0;
        
        /** Use the Surface as a ParametersBase constructor, from local parameters - neutral */
        virtual const Trk::ParametersBase<5, Trk::Neutral>* createNeutralParameters(double, double, double, double, double, Alg::AmgSymMatrix<5>* cov = 0) const = 0;
        
        /** Use the Surface as a ParametersBase constructor, from global parameters - neutral */
        virtual const Trk::ParametersBase<5, Trk::Neutral>* createNeutralParameters(const Alg::Point3D&, const Alg::Vector3D&, double charge=0., Alg::AmgSymMatrix<5>* cov = 0) const = 0;
        
        /** fast straight line intersection schema - standard: provides closest intersection and (signed) path length
         forceFwd is to provide the closest forward solution
         */
        virtual Trk::Intersection straightLineIntersection(const Alg::Point3D& pos,
         const Alg::Vector3D& dir,
         bool forceDir = false) const = 0;
        
                
    protected:
        //default constructor for inherited classes
        Surface ();
        //center position of the surface
        mutable Alg::Point3D*                   m_center;
        //normal vector to the surface
        mutable Alg::Vector3D*                  m_normal;
        //transformation matrix of the surface
        std::shared_ptr<const Alg::Transform3D> m_transform;
        //TGeoNode for the conversion of the DD4Hep geometry
        TGeoNode*                               m_node;
        //2D grid of Material (set to zero if surface has none)
        MaterialMap*                            m_materialmap;
        
       //constants
        //tolerance for a point being on the surface
        static double                           s_onSurfaceTolerance;
        //limit, when a variable is seen as zero
        static double                           s_zeroLimit;
        //origin
        static Alg::Point3D                     s_origin;
        //identity transformation
        static Alg::Transform3D                 s_idTransform;
        
    };
}

#endif //RECO_SURFACE_H)