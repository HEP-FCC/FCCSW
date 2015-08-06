//
//  SurfaceTest.cpp
//  
//
//  Created by Julia Hrdinka on 17/09/14.
//
//

#include "TestAlgorithms/SurfaceTest.h"

#include "Math/GenVector/RotationX.h"
#include "Math/GenVector/RotationY.h"
#include "Math/GenVector/RotationZ.h"

#include "TFile.h"
#include "TSystem.h"
#include "TGeoManager.h"
//#include "TGeoMaterial.h"
//#include "TGeoMedium.h"
//#include "TGeoVolume.h"
#include "TGeoArb8.h"
#include "TGeoMatrix.h"
#include "Math/GenVector/RotationZYX.h"

//#include "DDe4hep/Segmentations.h"

#include <iostream>


//#include "DD4hep/DetFactoryHelper.h"

/*using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;*/

DECLARE_COMPONENT(SurfaceTest)

SurfaceTest::SurfaceTest(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc)
{}

StatusCode SurfaceTest::initialize() {
    return GaudiAlgorithm::initialize();
}

StatusCode SurfaceTest::execute() {
    
    //LCDD& lcdd;
    //Trk::PlaneSurface defaultsurface;
    //std::cout << defaultsurface << std::endl;
    
    //SurfaceTest
    /*Alg::Transform3D *rtrans = new Alg::Transform3D;
    std::cout << "#### rtrans" << *rtrans << std::endl;
    Trk::PlaneSurface testsurface(rtrans, 0.5, 0.6);
    Trk::PlaneSurface copysurface(testsurface);
    std::cout << testsurface << std::endl;
    std::cout << copysurface << std::endl;
    
    //Rectangle
    Trk::RectangleBounds *rbounds = new Trk::RectangleBounds(2.,3.);
    Alg::Transform3D *trans = new Alg::Transform3D;
    Trk::PlaneSurface plane(trans, rbounds);
    std::cout << *rbounds << std::endl;
    std::cout << plane << std::endl;
    
    if (testsurface==plane) std::cout << "testsurface = plane" << "type " << plane.type() << std::endl;
    
    //Triangle
    Alg::Transform3D *ttrans = new Alg::Transform3D;
    Alg::Vector2D _p1(1.,0.);
    Alg::Vector2D _p2(1.,2.);
    Alg::Vector2D _p3(2.,1.);
    Trk::TriangleBounds *tribounds = new Trk::TriangleBounds(_p1, _p2, _p3);
    Trk::PlaneSurface triangle (ttrans, tribounds);
    std::cout << *tribounds << std::endl;
    std::cout << triangle << std::endl;
    
    
    //Diamond
    ROOT::Math::RotationX rx;
    rx.SetComponents(2.5);
    Alg::Transform3D *xtrans = new Alg::Transform3D(rx,transvec);
    Trk::DiamondBounds *diabounds = new Trk::DiamondBounds(3.,1.,3.,2.,2.);
    Trk::PlaneSurface diamond (xtrans, diabounds);
    std::cout << *diabounds << std::endl;
    std::cout << diamond << std::endl;
    
    //Cylinder
    Alg::Transform3D *ctrans = new Alg::Transform3D;
    Trk::CylinderSurface cylindersur(ctrans, 3., 5.);
    //Trk::CylinderSurface cylindersur1(ctrans, 3., 1.5, 5.);
    
    ROOT::Math::RotationZ rz;
    rz.SetComponents(1.5);
    Alg::Transform3D* ztrans = new Alg::Transform3D(rz);
    Trk::CylinderBounds *cylbounds = new Trk::CylinderBounds (5., 2.6, 12.);
    Trk:: CylinderSurface cylindersur2 (ztrans, cylbounds);
    std::cout << *cylbounds << std::endl;
    std::cout << cylindersur2 << std::endl;
    
    //Perigee
    Alg::Vector3D perv(1.,2.,1.);
    Trk::PerigeeSurface perigee(perv);
    Trk::PerigeeSurface* perigeeclone = perigee.clone();
    Trk::NoBounds perigeebounds = perigee.bounds();
    std::cout << perigeebounds << std::endl;
    std::cout << perigee << std::endl;
    std::cout << *perigeeclone << std::endl;

    //Trk::Volume testvolume;
    
    //Alg::AmgSymMatrix <3> testmatrix3;
    const int DIM = 5;
    Alg::AmgSymMatrix <DIM> testmatrix;
    for (int i = 0; i < DIM*DIM; i++)
    {
        testmatrix[i] = i;
        std::cout << "Testmatrix: " << testmatrix[i] << std::endl;
    }
    Alg::AmgVector <DIM> testvector;
    for (int i = 0; i < DIM; i++)
    {
        testvector[i] = i;
        std::cout << "TestVector: " << testvector[i] << std::endl;
    }
    
    Alg::AmgSymMatrix <5>* matrix = new Alg::AmgSymMatrix <5>;
    cylindersur2.Trk::CylinderSurface::createTrackParameters(2., 3., 1.5, 1.8, 1.2, matrix);*/
    
    //Trapezoid
  /*  Alg::RotationMatrix3D unity;
    Alg::Vector3D transvec(0.,0.,10.);
    Alg::Transform3D *trtrans = new Alg::Transform3D(unity,transvec);
    Trk::TrapezoidBounds *trabounds = new Trk::TrapezoidBounds(10.,20.,5.);
    Trk::PlaneSurface trapezoid (trtrans, trabounds);
    std::cout << *trabounds << std::endl;
    std::cout << trapezoid << std::endl;
    
    
    //Disc
    ROOT::Math::RotationY ry;
    ry.SetComponents(2.5);
    Alg::Vector3D _TY(0.,-12.,0.);
    Alg::Transform3D* ytrans = new Alg::Transform3D(ry,_TY);
    Trk::DiscBounds *discbounds = new Trk::DiscBounds (1., 3., 2.5);
    Trk:: DiscSurface discsur (ytrans, discbounds);
    std::cout << *discbounds << std::endl;
    std::cout << discsur << std::endl;
    
    
    //AbstractVolume
    std::cout << "########## full Cylinder ##########" << std::endl;
    Trk::CylinderVolumeBounds* fullbounds = new Trk::CylinderVolumeBounds(1.,5.);
    std::cout << *fullbounds << std::endl;
    Alg::Transform3D *voltrans = new Alg::Transform3D;
    std::cout << *voltrans << std::endl;
    Trk::AbstractVolume fullcylinder (voltrans, fullbounds);
    std::cout << fullcylinder << std::endl;
    
    std::cout << "########## Cylinder with hole ##########" << std::endl;
    Trk::CylinderVolumeBounds* holebounds = new Trk::CylinderVolumeBounds(2.,3.,5.);
    std::cout << *holebounds << std::endl;
    ROOT::Math::RotationY RY(0.5*M_PI);
    //ROOT::Math::RotationX RX(0.5*M_PI);
    //Alg::RotationMatrix3D RYX = RY*RX;
    //RX.SetComponents(0.5*M_PI);
    Alg::Vector3D TY(0.,12.,0.);
    Alg::Transform3D* ZTrans = new Alg::Transform3D(RY,TY);
    //Alg::Transform3D* ZTrans = new Alg::Transform3D;
    std::cout << *ZTrans << std::endl;
    Trk::AbstractVolume holecylinder (ZTrans, holebounds);
    std::cout << holecylinder << std::endl;
    
    
    //TFile
    //TFile *CylinderOut = new TFile("FullCylinder.root","RECREATE");
    //if ( CylinderOut->IsOpen() ) printf("File opened successfully\n");
    
    // gStyle->SetCanvasPreferGL(true);
    gSystem->Load("libGeom");
    TGeoManager *geom = new TGeoManager("simple1", "Simple geometry");
    //--- define some materials
    TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
    TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,2.7);
    //--- define some media
    TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
    TGeoMedium *Al = new TGeoMedium("Root Material",2, matAl);
    
    //Transformation for FullCylinder
    Alg::Vector3D a (voltrans->Translation().Vect());
   // TGeoTranslation *translation = new TGeoTranslation (a.X(),a.Y(),a.Z());
    //Get Rotation
    ROOT::Math::RotationZYX rotationZXY(voltrans->Rotation());
    //TGeoRotation *rotation = new TGeoRotation("georotation",rotationZXY.Phi(),rotationZXY.Theta(),rotationZXY.Psi());
    TGeoCombiTrans *transform = new TGeoCombiTrans(a.X(),a.Y(),a.Z(),new TGeoRotation("georotation",rotationZXY.Phi(),rotationZXY.Theta(),rotationZXY.Psi()));
    
    //Transformation for Cylinder with Hole
    a = ZTrans->Translation().Vect();
    rotationZXY = ZTrans->Rotation();
   // std::cout << "Rotationzyx: " << rotationZXY << std::endl;
   // std::cout << rotationZXY.Phi() << " "<< (rotationZXY.Theta()) << " " << rotationZXY.Psi() << std::endl;
    TGeoCombiTrans *htransform = new TGeoCombiTrans(a.X(),a.Y(),a.Z(),new TGeoRotation("georotation",rotationZXY.Phi()*180./M_PI,rotationZXY.Theta()*180./M_PI,rotationZXY.Psi()*180./M_PI));
    //TGeoCombiTrans *htransform = new TGeoCombiTrans(a.X(),a.Y(),a.Z(),new TGeoRotation("rot2",0,90,0));

    //Transformation for the Disc
    a = ytrans->Translation().Vect();
    rotationZXY = ytrans->Rotation();
    TGeoCombiTrans *dtransform = new TGeoCombiTrans(a.X(),a.Y(),a.Z(),new TGeoRotation("georotation",rotationZXY.Phi()*180./M_PI,rotationZXY.Theta()*180./M_PI,rotationZXY.Psi()*180./M_PI));
    
    //Transformation for the Trapezoid
    a = trtrans ->Translation().Vect();
    rotationZXY = trtrans->Rotation();
    TGeoCombiTrans *trtransform = new TGeoCombiTrans(a.X(),a.Y(),a.Z(),new TGeoRotation("georotation",rotationZXY.Phi()*180./M_PI,rotationZXY.Theta()*180./M_PI,rotationZXY.Psi()*180./M_PI));
    
    
    //make TOP-Volume
    TGeoVolume *top = geom->MakeBox("TOP", Vacuum, 100., 100., 100.);
    geom->SetTopVolume(top);

    //Add First Cylinder
    TGeoVolume *fcylindervol = geom->MakeTube("fcylindervol", Al, fullbounds->innerRadius(), fullbounds->outerRadius(), fullbounds->halflengthZ());
    fcylindervol->SetLineColor(kRed);
    top->AddNode(fcylindervol, 1, transform);
    
    //Add Second Cylinder
    TGeoVolume *hcylindervol = geom->MakeTube("hcylindervol", Al, holebounds->innerRadius(), holebounds->outerRadius(), holebounds->halflengthZ());
    fcylindervol->SetLineColor(kBlue);
    top->AddNode(hcylindervol, 2, htransform);
    
    //Add Disc Surface
    TGeoVolume *discsurface = geom->MakeTubs("discsurface", Al, discbounds->rMin(), discbounds->rMax(), 0.,discbounds->averagePhi(),discbounds->averagePhi());
    fcylindervol->SetLineColor(kGreen);
    top->AddNode(discsurface, 3, dtransform);
    
    TGeoVolume *trapezoidsur = geom->MakeTrd1("trapezoid", Al, trabounds->minHalflengthX(), trabounds->maxHalflengthX(), trabounds->halflengthY(),0.);
    fcylindervol->SetLineColor(kGreen);
    top->AddNode(trapezoidsur, 4, trtransform);
    
    
   geom->CloseGeometry();
    
    //Draw on ROOT_File
    top->Draw("ogle");
    geom->Export("FullCylinder.root");
    geom->SetVisLevel(0);*/
    
    
    
    /*std::cout << "########## Cylinder Segment ##########" << std::endl;
    Trk::CylinderVolumeBounds* segbounds = new Trk::CylinderVolumeBounds(1.,3.,0.25*M_PI,6.);
    std::cout << *segbounds << std::endl;
    Alg::Vector3D Transvec(1.,2.,-1.);
    ROOT::Math::RotationX RX;
    RX.SetComponents(0.5*M_PI);
    Alg::Transform3D* XTrans = new Alg::Transform3D(RX, Transvec);
    std::cout << *XTrans << std::endl;
    Trk::AbstractVolume segcylinder (XTrans, segbounds);
    std::cout << segcylinder << std::endl;*/

    
 /*   std::cout << "##### SURFACETEST GEHT #####" << std::endl;
    
    
    gSystem->Load("libGeom");
    TGeoManager *geom = new TGeoManager("simple1", "Simple geometry");
    
    TGeoVolume *top = geom->MakeBox("TOP", Vacuum, 100., 100., 100.);
    geom->SetTopVolume(top);
    
    TGeoVolume *box = geom->MakeBox("box", Al, 2.,2.,4.);
    TGeoTranslation* translation = new TGeoTranslation(0.,0.,30);
    
    top->AddNode(box, 1, translation);
    geom->CloseGeometry();
    */
    

    return StatusCode::SUCCESS;
}

StatusCode SurfaceTest::finalize() {
    
    return GaudiAlgorithm::finalize();
}
