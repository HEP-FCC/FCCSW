//
//  NavigationLayer.h
//  
//
//  Created by Julia Hrdinka on 14/01/15.
//
//

#ifndef RECO_NAVIGATIONLAYER_H
#define RECO_NAVIGATIONLAYER_H

#include "RecoGeometry/Layer.h"

namespace Reco {
    
    class NavigationLayer : public Layer {
    
    public:
        
        //default constructor
        NavigationLayer();
        //setting next and previous layer already while construction
        NavigationLayer(std::shared_ptr<const Layer> next,std::shared_ptr<const Layer> previous);
        //destructor
        ~NavigationLayer();
        //clone method
        virtual NavigationLayer* clone() const override;
        //return nullptr
        virtual const Surface* getModule(const Alg::Point3D& glopos) const;
        //return nullptr
        virtual const Surface* getModule(const Alg::Point2D& locpos) const;
        //returns the surface at this position plus the eight surrounding surfaces - returns empty vector
        virtual const std::vector<const Surface*> compatibleSurfaces(const Alg::Point3D& glopos) const override;
        //set the next layer - spherical direction beginning from center of the detector to outside
        virtual void setNextLayer(std::shared_ptr<const Layer> layer) const override;
        //set the previous layer - spherical direction beginning from center of the detector to outside
        virtual void setPreviousLayer(std::shared_ptr<const Layer> layer) const override;
        //get the next layer - spherical direction beginning from center of the detector to outside
        virtual const Layer* getNextLayer() const override;
        //get the previous layer - spherical direction beginning from center of the detector to outside
        virtual const Layer* getPreviousLayer() const override;
        //Not finished //get the next layer with respect of a specific direction
        virtual const Layer* getNextLayer(const Alg::Vector3D& dir) const override;
        //Not finished //get the next layer with respect of a specific direction
        virtual const Layer* getPreviousLayer(const Alg::Vector3D& dir) const override;
        //returns the LayerType see enum in Layer.h
        virtual LayerType type() const override;
        //returns false
        virtual bool onLayer(const Alg::Point3D& glopos, double tol) const override;
        //returns nullptr
        virtual const Surface* surfaceRepresentation() const override;
    };
}

#endif //RECO_NAVIGATIONLAYER_H
