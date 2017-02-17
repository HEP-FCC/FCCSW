#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace det {
static DD4hep::Geometry::Ref_t createECalBarrelInclined (DD4hep::Geometry::LCDD& aLcdd,
  DD4hep::XML::Handle_t aXmlElement,
  DD4hep::Geometry::SensitiveDetector aSensDet) {
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "ECalConstruction");
  MsgStream lLog(&(*msgSvc), "ECalConstruction");

  DD4hep::XML::DetElement xml_det = aXmlElement;
  std::string name_det = xml_det.nameStr();
  DD4hep::XML::Dimension dim_det (xml_det.dimensions());
  DD4hep::Geometry::DetElement det_element (name_det,xml_det.id());

  // Create air envelope for the whole barrel
  DD4hep::Geometry::Volume envelope_vol(name_det+"_vol",
    DD4hep::Geometry::Tube(dim_det.rmin(),dim_det.rmax(),dim_det.dz()),
    aLcdd.material("Air"));
  envelope_vol.setVisAttributes(aLcdd, dim_det.visStr());

  // Retrieve cryostat data
  DD4hep::XML::DetElement cryostat = aXmlElement.child("cryostat");
  DD4hep::XML::Dimension cryo_dim (cryostat.dimensions());
  double cryo_thickness = cryo_dim.thickness();

  // Retrieve active and passive material data
  DD4hep::XML::DetElement calo = aXmlElement.child("calorimeter");
  DD4hep::XML::Dimension calo_dim (calo.dimensions());

  DD4hep::XML::DetElement active = calo.child("active");
  std::string active_mat = active.materialStr();
  double active_thickness = active.thickness();

  DD4hep::XML::DetElement passive = calo.child("passive");
  std::string passive_mat = passive.materialStr();
  double passive_thickness = passive.thickness();
  double angle = passive.rotation().angle();

  // 1. Create the tubes for the outer shapes of the volumes
  DD4hep::Geometry::Tube calo_outer_shape(calo_dim.rmin(), calo_dim.rmax(), calo_dim.dz());
  DD4hep::Geometry::Tube bath_outer_shape(cryo_dim.rmin() + cryo_thickness,
    cryo_dim.rmax() - cryo_thickness, cryo_dim.dz() - cryo_thickness);
  DD4hep::Geometry::Tube cryo_outer_shape(cryo_dim.rmin(), cryo_dim.rmax(), cryo_dim.dz());
  // Subtract volumes to get the actual shapes of bath and cryo
  DD4hep::Geometry::SubtractionSolid bath_shape(bath_outer_shape, calo_outer_shape);
  DD4hep::Geometry::SubtractionSolid cryo_shape(cryo_outer_shape, bath_outer_shape);

  // 1. Create cryostat
  lLog << MSG::DEBUG << "ECAL cryostat: rmin (cm) = " << cryo_dim.rmin() << " rmax (cm) = "
       << cryo_dim.rmax() << " thickness (cm) = " << cryo_thickness << endmsg;
  DD4hep::Geometry::Volume cryoVol(cryostat.nameStr(), cryo_shape, aLcdd.material(cryostat.materialStr()));
  envelope_vol.placeVolume(cryoVol);

  // 2. Create bath that is inside the cryostat and surrounds the calorimeter
  //    Bath is filled with active material -> but not sensitive
  DD4hep::Geometry::Volume bath_vol(active_mat + "_bath", bath_shape, aLcdd.material(active_mat));
  lLog << MSG::DEBUG << "ECAL bath: material = " << active_mat << " rmin (cm) =  "
       << cryo_dim.rmin() + cryo_thickness << " rmax (cm) = " << cryo_dim.rmax() - cryo_thickness << endmsg;
  envelope_vol.placeVolume(bath_vol);

  // 3. Create the calorimeter by placing the passive material.
  //    Subtracting the passive material from calo tube forms the sensitive, active region
  DD4hep::Geometry::Volume calo_vol(active_mat + "_sensitive", calo_outer_shape, aLcdd.material(active_mat));
  // Mark active material as sensitive
  DD4hep::Geometry::SensitiveDetector sd = aSensDet;
  DD4hep::XML::Dimension sd_typ = xml_det.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());
  calo_vol.setSensitiveDetector(aSensDet);
  lLog << MSG::DEBUG << "ECAL sensitive volume: material = " << active_mat << " rmin (cm) =  "
       << calo_dim.rmin() << " rmax (cm) = " << calo_dim.rmax() << endmsg;

  // 3.a. Create the passive planes
  lLog << MSG::DEBUG << "passive material = " << passive_mat << " thickness of planes (cm) =  "
       << passive_thickness << " rotation angle = " << angle << endmsg;
  uint numPlanes = 2. * M_PI * calo_dim.rmin() / (passive_thickness + active_thickness);
  double dPhi = 2. * M_PI / numPlanes;
  lLog << MSG::DEBUG << "number of passive plates = " << numPlanes << " azim. angle difference =  " << dPhi << endmsg;
  lLog << MSG::DEBUG << " distance at inner radius (cm) = " << 2. * M_PI * calo_dim.rmin() / numPlanes
       << " distance at outer radius (cm) = " << 2. * M_PI * calo_dim.rmax() / numPlanes << endmsg;
  double Rmin = calo_dim.rmin();
  double Rmax = calo_dim.rmax();
  double dR = Rmax - Rmin;
  // TODO: adjust length of passive plane (because of angle)
  double plane_length = dR;
  DD4hep::Geometry::Box plane_shape(plane_length / 2., passive_thickness / 2., calo_dim.dz());
  DD4hep::Geometry::Volume plane_vol(passive_mat, plane_shape , aLcdd.material(passive_mat));
  double offsetPhi = 0;
  for(uint iPlane = 0; iPlane < numPlanes; iPlane ++) {
    // calculate centre position of the plane without plane rotation
    double phi = offsetPhi + iPlane * dPhi;
    double xRadial = (Rmin + dR / 2.) * cos(phi);
    double yRadial = (Rmin + dR / 2.) * sin(phi);
    // calculate position of the beginning of plane
    double xRmin = Rmin * cos(phi);
    double yRmin = Rmin * sin(phi);
    // rotate centre by angle wrt beginning of plane
    double xRotated = xRmin + (xRadial - xRmin) * cos(angle) - (yRadial - yRmin) * sin(angle);
    double yRotated = yRmin + (xRadial - xRmin) * sin(angle) + (yRadial - yRmin) * cos(angle);
    DD4hep::Geometry::Transform3D transform( DD4hep::Geometry::RotationZ(phi + angle),
        DD4hep::Geometry::Position(xRotated, yRotated, 0) );
    DD4hep::Geometry::PlacedVolume plane_physvol = calo_vol.placeVolume(plane_vol, transform);
    lLog << MSG::DEBUG << " Centre of the unrotated plane " << iPlane <<" at angle "<< phi <<" is ( " << xRadial << " , " << yRadial << " ) " << endmsg;
    lLog << MSG::DEBUG << " Rotating plane by angle "<< angle <<" => ( " << xRotated << " , " << yRotated << " ) " << endmsg;
  }
  envelope_vol.placeVolume(calo_vol);

  // Place the envelope
  DD4hep::Geometry::Volume mother_vol = aLcdd.pickMotherVolume(det_element);
  DD4hep::Geometry::PlacedVolume ECalBarrel_physvol = mother_vol.placeVolume(envelope_vol);
  ECalBarrel_physvol.addPhysVolID("system", xml_det.id());
  det_element.setPlacement(ECalBarrel_physvol);
  return det_element;

}
} // namespace det

DECLARE_DETELEMENT(EmCaloBarrelInclined, det::createECalBarrelInclined)

