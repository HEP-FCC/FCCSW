//
//  Volume.h
//  
//
//  Created by Julia Hrdinka on 09/01/15.
//
//

#ifndef RECO_VOLUME_H
#define RECO_VOLUME_H

#include "Algebra/AlgPrimitives.h"
#include "TrkGeometryUtils/BinnedArray.h"
#include "RecoGeometry/BoundarySurface.h"
#include "RecoGeometry/Surface.h"
#include "RecoGeometry/Layer.h"
#include "TGeoNode.h"

namespace Reco {
    
    class Volume {
        
    public:
        
        enum VolumeType {
            None                    = 0,
            CylinderVolume          = 1,
            ContainerVolume         = 2,
            ContainerCylinderVolume = 3
        };
        
        //volume type needed for the translation
        enum TranslationType {
            nEndCap   = 0,
            barrel    = 1,
            pEndCap   = 2,
            container = 3,
            none      = 4
        };
        
        typedef Trk::BinnedArray<Layer> LayerArray;
        typedef std::vector<std::shared_ptr<const BoundarySurface>> SurfaceVector;
        
        //constructor with transformation matrix for volume without layers
        Volume(std::shared_ptr<const Alg::Transform3D> transf);
        //constructor for volume with layers
        Volume(LayerArray* layers);
        //constructor for volume with layers and transformation matrix
        Volume(std::shared_ptr<const Alg::Transform3D> transf, LayerArray* layers);
        //constructors from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //constructor with TGeoNode - transform is set automatically - for a volume with layers
        Volume(LayerArray* layers, TGeoNode* node);
        //constructor from TGeoNode for volume without layers
        Volume(TGeoNode* node);
        //copyconstructor
        Volume(const Volume& volume);
        //destructor
        virtual ~Volume();
        //clone Method
        virtual Volume* clone() const = 0;
        //assignment operator
        Volume& operator=(const Volume& volume);
        //get the center of the volume - returns origin if not set
        const Alg::Point3D& center() const;
        //get transform matrix - returns idendity transformation if not se
        const Alg::Transform3D& transform() const;
        //checks if global position glopos is inside the bounds of the volume with a tolerance tol
        virtual bool isInside(const Alg::Point3D& glopos, double tol=0.00001) const = 0;
        //returns only the layers with a substructure (no layers which are only for navigation purposes) ordered from the global starting point glopos with irdering direction mom
        std::vector<const Reco::Layer*> materialLayersOrdered(const Alg::Point3D& glopos,const Alg::Vector3D& mom, double charge) const;
        //returns the number of boundary surfaces
        int NumberOfSurfaces() const;
        //get all boundarySurfaces in a vector
        std::vector<std::weak_ptr<const BoundarySurface>> boundarySurfaces() const;
        //get all the BoundarySurfaces in a vector
//        SurfaceVector& boundarySurfaces();
        //get pointer to boundary surface at n - for definition of n see specific classes (example: CylinderVolume)
        const BoundarySurface* getBoundarySurface (size_t n) const;
        //get boundary surface at n - for definition of n see specific classes (example: CylinderVolume)
//        const BoundarySurface& boundarySurface (size_t n) const;
        //set pointer to boundary surface at n - for definition of n see enum BoundarySurfaceType in inherited classes (example: CylinderVolume.h)
        virtual bool setBoundarySurface(size_t n, std::shared_ptr<const BoundarySurface> boundarysurface) const;
        //get the type of the volume - see enum VolumeType
        VolumeType type() const;
        //set the type of the volume for the translation in the RecoGeoConverterTool(needed for Translation from DD4hep in RecoGeometry) - see enum VolumeType
        void setTranslationType(TranslationType volumeType) const;
        //set the type of the volume for the translation in the RecoGeoConverterTool(needed for Translation from DD4hep in RecoGeometry) - see enum TranslationType
        TranslationType getTranslationType() const;
        //get the coordinates of the volume (see enum Coordinates of inherited classes)
        double getCoordinate(size_t n) const;
        
    
    protected:
        //default constructor for inherited classes
        Volume();
        //center position of the volume
        mutable Alg::Point3D*                           m_center;
        //transformation matrix of the volume
        std::shared_ptr<const Alg::Transform3D>         m_transform;
        //contains all layers (navigation and material) of the volume - one atached to the other (no free spaces)
        LayerArray*                                     m_layers;
        //boundary surfaces
        mutable SurfaceVector                           m_boundarySurfaces;
        //type of the volume (see inherited classes)
        VolumeType                                      m_volumeType;
        //type of the volume for the translation in the RecoGeoConverterTool(needed for Translation from DD4hep in RecoGeometry) - see enum VolumeType
        mutable TranslationType                         m_translationType;
        //coordinates of the volume (see enum Coordinates of inherited classes)
        std::vector<double>                             m_coordinates;
        //origin
        static Alg::Point3D                             s_origin;
        //idendity transformation
        static Alg::Transform3D                         s_idTransform;
        
       

    };
}


#endif //RECO_VOLUME_H
