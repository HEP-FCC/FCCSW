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
        SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, size_t binsX, size_t binsY, long long int volumeID);
        //constructor to set transform manually
        SensitivePlaneSurface(TGeoBBox* box, std::shared_ptr<const Alg::Transform3D> transf, size_t binsX, size_t binsY, long long int volumeID);
        //constructor with materialmap
        SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, MaterialMap* materialmap, size_t binsX , size_t binsY, long long int volumeID);
        SensitivePlaneSurface(TGeoBBox* box, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, size_t binsX, size_t binsY, long long int volumeID);
        //manuel constructor, to set transform and dimensions manuel
        SensitivePlaneSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX, double halfY, size_t binsX, size_t binsY, long long int volumeID);
        //copy constructor
        SensitivePlaneSurface(const SensitivePlaneSurface& planesurface);
        //destructor
        virtual ~SensitivePlaneSurface();
        //clone method
        virtual SensitivePlaneSurface* clone() const override;
        //assignemnt operator
        SensitivePlaneSurface& operator=(const SensitivePlaneSurface& sensplane);
        //every position on the surface corresponds to a unique bin of this surface
        //calculates the unique bin in the local 2D grid of the surface corresponding to this local position locpos
        virtual size_t bin(const Alg::Point2D& locpos) const override;
        
    private:
        //local 2D grid
        Trk::BinUtility* m_binutility;
        
    };
}

#endif //RECO_SENSITIVEPLANESURFACE_H
