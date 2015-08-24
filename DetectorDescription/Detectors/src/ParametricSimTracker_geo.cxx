#include "DD4hep/DetFactoryHelper.h"
#include "DetExtensions/DetCylinderVolume.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)
{
    xml_det_t x_det = e;
    string det_name = x_det.nameStr();
    //Make DetElement
    DetElement simpletracker(det_name, x_det.id());
    std::cout<<"sdjfksdhfsdjhfkjsdhfkjsdhfkjdshfkjdshfkjshfoehfsfkj "<<det_name<<std::endl;
    //get status for the RecoGeometry
    xml_comp_t x_status = x_det.child(_U(status));
    int status     = x_status.id();
    //add Extension to Detlement for the RecoGeometry
    Det::DetCylinderVolume* detvolume = new Det::DetCylinderVolume(status);
    simpletracker.addExtension<Det::IDetExtension>(detvolume);
    //make Volume
    DD4hep::XML::Dimension x_det_dim(x_det.dimensions());
    Tube tube_shape(x_det_dim.rmin(),x_det_dim.rmax(),x_det_dim.z());
    Volume tube_vol(det_name,tube_shape,lcdd.air()); //air at the moment change later
    tube_vol.setVisAttributes(lcdd, x_det_dim.visStr());
    //Place Volume
    Volume mother_vol = lcdd.pickMotherVolume(simpletracker);
    PlacedVolume placedTube = mother_vol.placeVolume(tube_vol);
    placedTube.addPhysVolID("tracker",simpletracker.id());
    simpletracker.setPlacement(placedTube);
    
    return simpletracker;
}

DECLARE_DETELEMENT(ParametricSimTracker, create_element)
















