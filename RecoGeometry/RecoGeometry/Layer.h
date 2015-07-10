//
//  Layer.h
//  
//
//  Created by Julia Hrdinka on 26/11/14.
//
//

#ifndef RECO_LAYER_H
#define RECO_LAYER_H

#include "TrkGeometryUtils/BinnedArray.h"
#include "RecoGeometry/Surface.h"

namespace Reco {
    
    typedef std::vector<std::shared_ptr<const Surface>> SurfaceVector;
    typedef Trk::BinnedArray<SurfaceVector> SurfaceArray;
    
    class Layer {
    
    public:
        
        //type of the layer
        //used in materialLayersordered method of Volume
        enum LayerType {
            navigation  = 0,
            cylinder    = 1,
            disc        = 2,
        };
        
        //constructor with a BinnedArray of surfaces which the layer contains
        Layer (SurfaceArray* sf);
        //copy constructor
        Layer (const Layer& layer);
        //destructor
        virtual ~Layer();
        //clone method
        virtual Layer* clone() const = 0;
        //get the specific modules(=surfaces) at the global position glopos - returns empty vector if no surface is at this point
        const SurfaceVector getModules(const Alg::Point3D& glopos) const;
        //get the specific modules(=surfaces) at the local position locpos - returns empty vector if no surface is at this point
        const SurfaceVector getModules(const Alg::Point2D& locpos) const;
        //returns the surfaces at this position plus the eight surrounding surfaces - returns empty vector for a NavogationLayer
        virtual const std::vector<SurfaceVector> compatibleSurfaces(const Alg::Point3D& glopos) const = 0;
        //set the next layer - spherical direction beginning from center of the detector to outside
        virtual void setNextLayer(std::shared_ptr<const Layer> layer) const;
        //set the previous layer - spherical direction beginning from center of the detector to outside
        virtual void setPreviousLayer(std::shared_ptr<const Layer> layer) const;
        //get the next layer - spherical direction beginning from center of the detector to outside
        virtual const Layer* getNextLayer() const;
        //get the previous layer - spherical direction beginning from center of the detector to outside
        virtual const Layer* getPreviousLayer() const;
        //Not finished //get the next layer with respect of a specific direction
        virtual const Layer* getNextLayer(const Alg::Vector3D& dir) const;
        //Not finished //get the next layer with respect of a specific direction
        virtual const Layer* getPreviousLayer(const Alg::Vector3D& dir) const;
        //checks if a global position glopos is on the layer
        virtual bool onLayer(const Alg::Point3D& glopos, double tol=0.00001) const = 0;
        //returns the LayerType see enum
        virtual LayerType type() const = 0;
        //returns the surface representation of the layer
        virtual const Surface* surfaceRepresentation() const = 0;
        
    protected:
        //default constructor private for inherited classes
        Layer ();
        
        //BinnedArray of Surfaces which the layer contains
        const SurfaceArray*                     m_surfaces;
        //shared pointers to the next and previous layers
        mutable std::shared_ptr<const Layer>    m_nextLayer;
        mutable std::shared_ptr<const Layer>    m_previousLayer;
    };
}

#endif //RECO_LAYER_H
