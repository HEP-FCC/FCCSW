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

#include "TrkGeometryUtils/BinUtility.h"

namespace Reco {
    
    class SensitiveCylinderSurface : public CylinderSurface, public SensitiveSurface {
        
    public:
        //constructor from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //material is set atomatically (node->Material)
        //binsRPhi and binsZ are the number of bins in RPhi and Z for the readout at this surface (at the moment set randomly 1000 as default - maybe later hand over size of cell)
        SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        
        SensitiveCylinderSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf,long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //constructor to set Material extra
        SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        SensitiveCylinderSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //constructor with transform matrix and dimensions of the cylinder
        SensitiveCylinderSurface(std::shared_ptr<const Alg::Transform3D> transf, double radius, double halfZ, double HalfThickness, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //constructor with rmin and rmax
        SensitiveCylinderSurface(double radius, double halfZ, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //copy constructor
        SensitiveCylinderSurface(const SensitiveCylinderSurface& senscyl);
        //destructor
        virtual ~SensitiveCylinderSurface();
        //clone method
        virtual SensitiveCylinderSurface* clone() const override;
        //assignment operator
        SensitiveCylinderSurface& operator=(const SensitiveCylinderSurface& senscyl);
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const override;
        //returns the surface representation of the sensitive surface
        virtual const CylinderSurface* surfaceRepresentation() const override;
        
    };
}


#endif //RECO_SENSITIVECYLINDERSURFACE_H
