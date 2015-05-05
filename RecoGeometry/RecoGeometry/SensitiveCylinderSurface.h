//
//  SensitiveCylinderSurface.h
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#ifndef RECO_SENSITIVECYLINDERSURFACE_H
#define RECO_SENSITIVECYLINDERSURFACE_H

#include "RecoGeometry/CylinderSurface.h"
#include "RecoGeometry/SensitiveSurface.h"
#include "RecoGeometry/ReadoutSegmentation.h"

#include "TrkGeometryUtils/BinUtility.h"

namespace Reco {
    
    class SensitiveCylinderSurface : public CylinderSurface, public SensitiveSurface {
        
    public:
        //constructor from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //material is set atomatically (node->Material)
        //binsRPhi and binsZ are the number of bins in RPhi and Z for the readout at this surface (at the moment set randomly 1000 as default - maybe later hand over size of cell)
        SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, long long int volumeID, Reco::ReadoutSegmentation* segmentation);
        
        SensitiveCylinderSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf,long long int volumeID, Reco::ReadoutSegmentation* segmentation);
        //constructor to set Material extra
        SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, long long int volumeID, Reco::ReadoutSegmentation* segmentation);
        SensitiveCylinderSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Reco::ReadoutSegmentation* segmentation);
        //constructor with transform matrix and dimensions of the cylinder
        SensitiveCylinderSurface(std::shared_ptr<const Alg::Transform3D> transf, double radius, double halfZ, double HalfThickness, long long int volumeID, Reco::ReadoutSegmentation* segmentation);
        //constructor with rmin and rmax
        SensitiveCylinderSurface(double radius, double halfZ, long long int volumeID, Reco::ReadoutSegmentation* segmentation);
        //copy constructor
        SensitiveCylinderSurface(const SensitiveCylinderSurface& senscyl);
        //destructor
        virtual ~SensitiveCylinderSurface();
        //clone method
        virtual SensitiveCylinderSurface* clone() const override;
        //assignment operator
        SensitiveCylinderSurface& operator=(const SensitiveCylinderSurface& senscyl);
        //every position on the surface corresponds to a unique bin of this surface
        //calculates the unique bin in the local 2D grid of the surface corresponding to this local position locpos
        virtual unsigned long bin(const Alg::Point2D& locpos) const override;
        //returns the corresponding local position to this local bin
        virtual Alg::Point2D binToLocpos(unsigned long bin) const override;
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const override;
        //returns the pitch width, ba represents the coordinate
        virtual float binWidth(const Alg::Point2D& locpos, size_t ba=0) const override;
        //returns bin at this position plus the surrounding bins
        virtual const std::vector<unsigned long> compatibleBins(const Alg::Point2D& locpos) const override;
        
    private:
    
        Reco::ReadoutSegmentation* m_segmentation;
        
    };
}


#endif //RECO_SENSITIVECYLINDERSURFACE_H
