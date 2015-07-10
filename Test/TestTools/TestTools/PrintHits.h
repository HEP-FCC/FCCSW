//
//  PlotHits.h
//  
//
//  Created by Julia Hrdinka on 29/04/15.
//
//

#ifndef PLOTHITS_H
#define PLOTHITS_H

//Interface
#include "TestInterfaces/IPrintHits.h"
//Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/RndmGenerators.h"
//ROOT
#include "TProfile.h"
#include "TFile.h"

#include "RecoGeometry/Surface.h"
//#include "Algebra/AlgPrimitives.h"
//RecoGeometry
#include "RecoGeometry/Volume.h"
#include "RecoGeometry/ContainerVolume.h"
#include "RecoGeometry/BoundarySurface.h"
#include "RecoGeometry/SensitiveSurface.h"
#include "RecoGeometry/Layer.h"

//std
#include <iostream>
#include <fstream>
#include <stdlib.h>

class PrintHits : public AlgTool, virtual public IPrintHits {

public:
    
    PrintHits(const std::string& type, const std::string& name, const IInterface* parent);
    ~PrintHits() {}
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    //flat random number generator between min and max
    double flatrand(double min, double max) const;
    //random direction
    const Alg::Vector3D randomdir();
    //scan trough volumes to get the volume at this point, which is not a container volume
    const Reco::Volume* scanVolumes(const Reco::Volume* volume, const Alg::Point3D& glopos) const;
    //intersect with the material layers of a volume
    StatusCode intersectSurface(const Reco::Surface& surface, Alg::Point3D& glopos, const Alg::Vector3D& dir) const;
    //get the next volume, if you are in a volume without material layers - intersects the boundarysurface and if it hits the surface changes glopos to the position of intersection
    const Reco::Volume* nextVolume(std::weak_ptr<const Reco::BoundarySurface> surf, Alg::Point3D& glopos, const Alg::Vector3D& dir, bool& success) const;
    //intersect
    StatusCode intersect(const Reco::Volume* vol, Alg::Point3D& glopos, const Alg::Vector3D& dir) const;
    
    virtual StatusCode printMaterial(std::shared_ptr<const Reco::ContainerVolume> worldVolume, const Alg::Point3D& start, size_t Nevents) override;
    virtual StatusCode printMaterial(std::shared_ptr<const Reco::ContainerVolume> worldVolume, const Alg::Point3D& start, std::vector<Alg::Vector3D> directions) override;
    virtual StatusCode printHits(std::vector<std::pair<double, const Alg::Vector3D>>& hits) override;
   /// StatusCode plotHits()
    
private:
    //OutPuts
    MsgStream       m_log;
    mutable std::ofstream m_out;
    TFile* file;
    //Random numbers
    IRndmGenSvc*    m_RGS;
    Rndm::Numbers   m_flat;
    //Histograms
    ITHistSvc*      m_ths;
    TProfile*       m_tInX0Prof;
    TProfile*       m_pathProf;
    TProfile*       m_tInX0Prof1;
    TProfile*       m_pathProf1;
    TProfile*       m_hitsProf;
    
    mutable double  m_tInX0;
    mutable double  m_pathlength;
    mutable double  m_pathlength1;
    mutable int     m_counter;
    mutable int     m_hits;
    
};

#endif //PLOTHITS_H


