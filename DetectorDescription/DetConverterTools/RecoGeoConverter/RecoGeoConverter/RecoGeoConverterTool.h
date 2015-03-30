//
//  RecoGeoConverterTool.h
//  
//
//  Created by Julia Hrdinka on 10/11/14.
//
//

#ifndef RECO_RECOGEOCONVERTERTOOL_H
#define RECO_RECOGEOCONVERTERTOOL_H

#define NMAX 100 //spaeter in fkt mitgeben lassen
//Interface
#include "IConverter/IGeoConverterTool.h"
//DetDesServices
#include "DetDesServices/DD4HepDetDesSvc.h"
#include "DetDesInterfaces/IRecoGeoSvc.h"
//Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/DataObjectHandle.h"
//Gaudi random generator
#include "GaudiKernel/RndmGenerators.h"
//DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Detector.h"
//Root
#include "TGeoManager.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
//RecoGeometry
#include "RecoGeometry/Surface.h"
#include "RecoGeometry/PlaneSurface.h"
#include "RecoGeometry/CylinderSurface.h"
#include "RecoGeometry/TrapezoidSurface.h"
#include "RecoGeometry/BoundarySurface.h"

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
//std
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <random>

class RecoGeoConverterTool : public AlgTool, virtual public IGeoConverterTool {
public:
    
    typedef std::vector<std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>> LayerVector;
    //constructor
    RecoGeoConverterTool(const std::string& type, const std::string& name, const IInterface* parent);
    //destructor
    virtual ~RecoGeoConverterTool (){
  //l      m_out.close();
    }
   //  virtual StatusCode initialize();
  //  virtual StatusCode finalize ();
    
    //flat random number generator between min and max
    double flatrand(double min, double max);
    //invoked by the Detectors/DetDesServices/DD4HepDetDesSvc
    virtual StatusCode convert();
    //scans trough all detector elements, currently not used in the tool
    StatusCode scanDetector(DD4hep::Geometry::DetElement detelement);
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
    bool isInsideTrapezoid(double locpos[3], double trapX1, double trapXm2, double trapY) const;
    //orders rValues of the DiscLayer
    std::vector<float> orderRValues(std::vector<std::pair<float,float>>& old) const;
    //sorts floats
    static bool sortFloatPairs(std::pair<float,float> ap, std::pair<float,float> bp);
    //sorts the CylinderLayers by their radius
    static bool sortCylinderLayers(const std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>& a,const std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>& b);
    //sorts the DiscLayers by their position beginning
    static bool sortDiscLayers(const std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>& a,const std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D>& b);
    //sorts the volumes by their position in z
    static bool sortZvolumes(const std::pair<std::shared_ptr<const Reco::Volume>, Alg::Point3D>& a,const std::pair<std::shared_ptr<const Reco::Volume>, Alg::Point3D>& b);
    
    
private:
    //output
    MsgStream               m_log;
    //output
//l    std::ofstream           m_out;
    //counter
    int                     m_counter;
    int module_counter;
    //Service for retrieving the DD4Hep Geometry
    IDetDesSvc*             m_DD4HepSvc;
    //Service to hand over the world volume of the reconstruction geometry
    IRecoGeoSvc*           m_recoGeoSvc;

};

#endif //RECO_RECOGEOCONVERTERTOOL_H
