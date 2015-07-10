//
//  SensitivePlaneSurface.h
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#ifndef RECO_SENSITIVEPLANESURFACE_H
#define RECO_SENSITIVEPLANESURFACE_H

#include "RecoGeometry/PlaneSurface.h"
#include "RecoGeometry/SensitiveSurface.h"

#include "TrkGeometryUtils/BinUtility.h"

namespace Reco {
    
    class SensitivePlaneSurface : public PlaneSurface, public SensitiveSurface {
        
    public:
        //constructors from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //constructor with TGeoNode - transform is set automatically
        //binsX and binsY are the number of bins in X and Y for the readout at this surface
        SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //constructor to set transform manually
        SensitivePlaneSurface(TGeoBBox* box, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //constructor with materialmap
        SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, MaterialMap* materialmap, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        SensitivePlaneSurface(TGeoBBox* box, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //manuel constructor, to set transform and dimensions manuel
        SensitivePlaneSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX, double halfY, double HalfThickness, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //copy constructor
        SensitivePlaneSurface(const SensitivePlaneSurface& planesurface);
        //destructor
        virtual ~SensitivePlaneSurface();
        //clone method
        virtual SensitivePlaneSurface* clone() const override;
        //assignemnt operator
        SensitivePlaneSurface& operator=(const SensitivePlaneSurface& sensplane);
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const override;
        //returns the surface representation of the sensitive surface
        virtual const PlaneSurface* surfaceRepresentation() const override;
    };
}

#endif //RECO_SENSITIVEPLANESURFACE_H
