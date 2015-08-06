//
//  RecoGeoTest.h
//  
//
//  Created by Julia Hrdinka on 26/02/15.
//
//

#ifndef RECOGEOTEST_H
#define RECOGEOTEST_H

#define N_REPEAT 100000

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "DetDesInterfaces/IRecoGeoSvc.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph2D.h"
#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TProfile.h"

#include "FWCore/DataHandle.h"
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

#include "datamodel/Particle.h"
#include "datamodel/ParticleCollection.h"
//tools
#include "GaudiKernel/IToolSvc.h"
#include "TestInterfaces/IPrintHits.h"



class RecoGeoTest: public GaudiAlgorithm {
    friend class AlgFactory<RecoGeoTest> ;
    
public:
    /// Constructor.
    RecoGeoTest(const std::string& name, ISvcLocator* svcLoc);
    /// Initialize.
    virtual StatusCode initialize();
    //flat random number generator between min and max
    double flatrand(double min, double max) const;
    //random direction
    const Alg::Vector3D randomdir() const;
    //intersect
    StatusCode intersect(const Reco::Volume* vol, Alg::Point3D& glopos, const Alg::Vector3D& dir) const;
    //scan trough volumes to get the volume at this point, which is not a container volume
    const Reco::Volume* scanVolumes(const Reco::Volume* volume, const Alg::Point3D& glopos) const;
    //intersect with the material layers of a volume
    bool intersectSurface(const Reco::Surface& surface, Alg::Point3D& glopos, const Alg::Vector3D& dir) const;
    //get the next volume, if you are in a volume without material layers - intersects the boundarysurface and if it hits the surface changes glopos to the position of intersection
    const Reco::Volume* nextVolume(std::weak_ptr<const Reco::BoundarySurface> surf, Alg::Point3D& glopos, const Alg::Vector3D& dir, bool& success) const;
    /// Execute.
    virtual StatusCode execute();
    /// Finalize.
    virtual StatusCode finalize();
    
private:
    
    IRecoGeoSvc* m_recoGeoSvc; //Detector Description Service
    std::shared_ptr<const Reco::ContainerVolume>    m_worldVolume;
    mutable std::ofstream                           m_out;
//    mutable std::ofstream                           m_barrels;
//    mutable std::ofstream                           m_endCaps;
    mutable TGraph2D*                               m_graph;
    mutable int                                     m_counter;
//    ITHistSvc*                                      m_ths;
/*    TH2F*                                           m_layer1;
    TH2F*                                           m_layer2;
    TH2F*                                           m_layer3;
    TH2F*                                           m_layer4;
    TH2F*                                           m_layer5;
    TH2F*                                           m_layer6;
    TH2F*                                           m_layer6_1;
    TH2F*                                           m_dirXY;*/
    mutable std::ofstream                           m_layers;
    mutable std::ofstream                           m_boundaries;
    mutable std::ofstream                           m_modules;
    mutable std::ofstream                           m_sens;
//    TrackHitCollection*                             m_hitcoll;
    DataHandle<ParticleCollection>                  m_particles;
    ParticleCollection*                             m_particlecoll;
    IToolSvc*                                       m_toolsvc;
    IPrintHits*                                     m_printhits;
    mutable double                                  m_tInX0;
    mutable double                                  m_pathlength;
    ITHistSvc*                                      m_ths;
    TProfile*                                       m_profile;
    
//    mutable std::vector<std::tuple<const Reco::Surface*, const Alg::Point3D, const Alg::Vector3D>> m_hits;
    mutable double m_sumh;

};


#endif //RECOGEOTEST_H
