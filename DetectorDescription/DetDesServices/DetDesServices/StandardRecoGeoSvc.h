//
//  StandardRecoGeoSvc.h
//  
//
//  Created by Julia Hrdinka on 31/03/15.
//
//

#ifndef STANDARDRECOGEOSVC_H
#define STANDARDRECOGEOSVC_H

//Interfaces
#include "DetDesInterfaces/IRecoGeoSvc.h"
#include "DetDesInterfaces/IGeoSvc.h"
//Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
//Root
#include "TGeoManager.h"
//DD4hep
#include "DD4hep/Detector.h"

//RecoGeometry
#include "RecoGeometry/Surface.h"
#include "RecoGeometry/PlaneSurface.h"
#include "RecoGeometry/CylinderSurface.h"
#include "RecoGeometry/TrapezoidSurface.h"
#include "RecoGeometry/BoundarySurface.h"

#include "RecoGeometry/SensitivePlaneSurface.h"
#include "RecoGeometry/SensitiveTrapezoidSurface.h"

#include "RecoGeometry/Layer.h"
#include "RecoGeometry/CylinderLayer.h"
#include "RecoGeometry/DiscLayer.h"
#include "RecoGeometry/NavigationLayer.h"

#include "RecoGeometry/Volume.h"
#include "RecoGeometry/CylinderVolume.h"
#include "RecoGeometry/ContainerVolume.h"
#include "RecoGeometry/ContainerCylinderVolume.h"
//DetExtensions
#include "DetExtensions/IExtension.h"
#include "DetExtensions/Extension.h"
#include "DetExtensions/DetCylinderVolume.h"
#include "DetExtensions/DetDiscVolume.h"
#include "DetExtensions/DetCylinderLayer.h"
#include "DetExtensions/DetDiscLayer.h"
#include "DetExtensions/DetModule.h"
//TrkGeometryUtils
#include "TrkGeometryUtils/BinUtility.h"
#include "TrkGeometryUtils/BinnedArray1D.h"
#include "TrkGeometryUtils/BinnedArray2D.h"
#include "TrkGeometryUtils/ReadoutSegmentation1D1D.h"
#include "TrkGeometryUtils/ReadoutSegmentation2D.h"
//std
#include <iostream>
#include <fstream>
#include <stdlib.h>

class StandardRecoGeoSvc: public extends1<Service, IRecoGeoSvc> {

public:
    
    typedef std::vector<std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>> LayerVector;
    
    StandardRecoGeoSvc(const std::string& name, ISvcLocator* svc);
    
    virtual ~StandardRecoGeoSvc();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode buildGeometry() override;
    virtual const Reco::ContainerVolume* getRecoGeo() override;

    //scans trough all detector elements
    StatusCode scanDetector(DD4hep::Geometry::DetElement detelement); //not used
    //translates the whole detector - called by convert
    StatusCode translateDetector(DD4hep::Geometry::DetElement det);
    //translates the volumes - called by translateDetector
    StatusCode translateVolume(DD4hep::Geometry::DetElement det, std::multimap<int, std::shared_ptr<const Reco::Volume>>& volumes);
    //translates the layers - called by translateVolume
    StatusCode translateLayer(DD4hep::Geometry::DetElement det, LayerVector& layers, std::shared_ptr<const Alg::Transform3D> transform);
    //translates the modules - called by translateLayer
    StatusCode translateModule(DD4hep::Geometry::DetElement det, std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>>& surfaces, std::shared_ptr<const Alg::Transform3D> transform);
    //takes the BinnedArray of CylinderLayers layers and creates a new BinnedArray fullayers with navigationlayers added between the layers and a vector bValues with the values for the binning
    //no center point needed for Cylinder, Rmax is the outer radius of the surrounding volume
    StatusCode binCylinderLayers(LayerVector& layers, LayerVector& fulllayers, std::vector<float>& bValues, Alg::Point3D center, double Rmax) const;
    //takes the BinnedArray of CylinderLayers layers and creates a new BinnedArray fullayers with navigationlayers added between the layers and a vector bValues with the values for the binning
    //center is the center position of the surrounding volume, halfZ is the outer radius of the containing volume
    StatusCode binDiscLayers(LayerVector& layers, LayerVector& fulllayers, std::vector<float>& bValues, Alg::Point3D center, double halfZ) const;
    //checks if the local position locpos is inside a trapezoid with the dimensions trapX1, trapX2, trapY
    bool isInsideTrapezoid(double locpos[3], double trapX1, double trapXm2, double trapY) const; //outsource?
    //orders rValues of the DiscLayer
    std::vector<float> orderRValues(std::vector<std::pair<float,float>>& old) const; //outsource?
    //sorts floats
    static bool sortFloatPairs(std::pair<float,float> ap, std::pair<float,float> bp); //outsource?
    //sorts the CylinderLayers by their radius
    static bool sortCylinderLayers(const std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>& a,const std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>& b);
    //sorts the DiscLayers by their position beginning
    static bool sortDiscLayers(const std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>& a,const std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>& b);
    //sorts the volumes by their position in z
    static bool sortZvolumes(const std::pair<std::shared_ptr<const Reco::Volume>, Alg::Point3D>& a,const std::pair<std::shared_ptr<const Reco::Volume>, Alg::Point3D>& b);

    
   // StatusCode buildGeometry();
    
private:
    std::shared_ptr<const Reco::ContainerVolume> m_worldVolume;
    IGeoSvc*                                     m_DD4HepSvc;
    MsgStream                                    m_log;
    mutable std::ofstream                        m_file;
};

#endif //STANDARDRECOGEOSVC_H
