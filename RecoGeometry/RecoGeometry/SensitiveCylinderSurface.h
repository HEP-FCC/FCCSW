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
        SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, size_t binsRPhi, size_t binsZ, long long int volumeID);
        SensitiveCylinderSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, size_t binsRPhi, size_t binsZ, long long int volumeID);
        //constructor to set Material extra
        SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, size_t binsRPhi, size_t binsZ, long long int volumeID);
        SensitiveCylinderSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, size_t binsRPhi, size_t binsZ, long long int volumeID);
        //constructor with transform matrix and dimensions of the cylinder
        SensitiveCylinderSurface(std::shared_ptr<const Alg::Transform3D> transf, double radius, double halfZ, size_t binsRPhi, size_t binsZ, long long int volumeID);
        //constructor with rmin and rmax
        SensitiveCylinderSurface(double radius, double halfZ, size_t binsRPhi, size_t binsZ, long long int volumeID);
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
        virtual size_t bin(const Alg::Point2D& locpos) const override;

    private:
        //BinUtility 2D grid for every surface
        Trk::BinUtility*    m_binutility;
        
    };
}


#endif //RECO_SENSITIVECYLINDERSURFACE_H
