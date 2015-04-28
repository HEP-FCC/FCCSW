//
//  DiscLayer.h
//  
//
//  Created by Julia Hrdinka on 08/12/14.
//
//

#ifndef RECO_DISCLAYER_H
#define RECO_DISCLAYER_H

#include "RecoGeometry/Layer.h"
#include "RecoGeometry/DiscSurface.h"
#include "TrkGeometryUtils/BinnedArray.h"

namespace Reco  {
    
    class DiscLayer : public DiscSurface, public Layer {
    
    public:
        //constructors from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //constructor with TGeoNode (transformation matrix) and TGeoConeSeg(dimesnions) and a BinnedArray of Surfaces
        DiscLayer(TGeoNode* node, TGeoConeSeg* disc, Trk::BinnedArray<Surface>* sf);
        //constructor with transformation, TGeoConeSeg(dimesnions) and a BinnedArray of Surfaces
        DiscLayer(std::shared_ptr<const Alg::Transform3D> transf, TGeoConeSeg* disc, Trk::BinnedArray<Surface>* sf);
        //constructor with transformation, dimensions and a BinnedArray of Surfaces
        DiscLayer(std::shared_ptr<const Alg::Transform3D> transf, double rmin, double rmax, double dz, double HalfThickness, Trk::BinnedArray<Surface>* sf);
        //destructor
        ~DiscLayer();
        //clone method
        virtual DiscLayer* clone() const override;
        //get the half length in z
        double getHalfZ() const;
        //returns the surface at this position plus the eight surrounding surfaces
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
        //checks if a global position glopos is on the layer
        virtual bool onLayer(const Alg::Point3D& glopos, double tol) const override;
        //returns the LayerType see enum in Layer.h
        virtual LayerType type() const override;
        //returns the surface representation of the layer
        virtual const DiscSurface* surfaceRepresentation() const override;
        
    private:
        //default constructor
        DiscLayer();
        
        //half length in z
        const double m_dz;
    };
}

#endif //RECO_DISCLAYER_H
