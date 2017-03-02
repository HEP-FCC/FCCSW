#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

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
  double cryo_thickness_front = cryo_dim.rmin2() - cryo_dim.rmin1();
  double cryo_thickness_back = cryo_dim.rmax2() - cryo_dim.rmax1();

  // Retrieve active and passive material data
  DD4hep::XML::DetElement calo = aXmlElement.child("calorimeter");
  DD4hep::XML::Dimension calo_dim (calo.dimensions());

  DD4hep::XML::DetElement active = calo.child("active");
  std::string active_mat = active.materialStr();
  double active_thickness = active.thickness();

  DD4hep::XML::DetElement overlap = active.child("overlap");
  double active_passive_overlap = overlap.offset();
  if(active_passive_overlap < 0 || active_passive_overlap > 0.5) {
    ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "ECalConstruction");
    lLog << MSG::ERROR << "Overlap between active and passive cannot be more than half of passive plane!" << endmsg;
    incidentSvc->fireIncident(Incident("ECalConstruction","GeometryFailure"));
  }
  DD4hep::XML::DetElement cell = calo.child("cell");
  double cell_height = cell.dimensions().height();

  DD4hep::XML::DetElement readout = calo.child("readout");
  std::string readout_mat = readout.materialStr();
  double readout_thickness = readout.thickness();

  DD4hep::XML::DetElement passive = calo.child("passive");
  DD4hep::XML::DetElement passive_inner = passive.child("inner");
  DD4hep::XML::DetElement passive_outer = passive.child("outer");
  DD4hep::XML::DetElement passive_glue = passive.child("glue");
  std::string passive_inner_mat = passive_inner.materialStr();
  std::string passive_outer_mat = passive_outer.materialStr();
  std::string passive_glue_mat = passive_glue.materialStr();
  double passive_inner_thickness = passive_inner.thickness();
  double passive_outer_thickness = passive_outer.thickness();
  double passive_glue_thickness = passive_glue.thickness();
  double passive_thickness = passive_inner_thickness + passive_outer_thickness + passive_glue_thickness;
  double angle = passive.rotation().angle();

  // 1. Create the tubes for the outer shapes of the volumes
  double bath_rmin, bath_rmax;
  if (cryo_thickness_front > 0) {
    bath_rmin = cryo_dim.rmin2();
    bath_rmax = cryo_dim.rmax1();
  } else {
    bath_rmin = calo_dim.rmin();
    bath_rmax = calo_dim.rmax();
  }
  DD4hep::Geometry::Tube bath_outer_shape(bath_rmin, bath_rmax, calo_dim.dz());
  if (cryo_thickness_front > 0) {
    DD4hep::Geometry::Tube cryo_outer_shape(cryo_dim.rmin1(), cryo_dim.rmax2(), cryo_dim.dz());
    // Subtract volumes to get the actual shape of cryo
    DD4hep::Geometry::SubtractionSolid cryo_shape(cryo_outer_shape, bath_outer_shape);

    // 1. Create cryostat
    lLog << MSG::DEBUG << "ECAL cryostat: rmin (cm) = " << cryo_dim.rmin1() << " rmax (cm) = "
         << cryo_dim.rmax2() << " thickness in front of ECal (cm) = " << cryo_thickness_front
         << " thickness behind ECal (cm) = " << cryo_thickness_back << endmsg;
    DD4hep::Geometry::Volume cryo_vol(cryostat.nameStr(), cryo_shape, aLcdd.material(cryostat.materialStr()));
    cryo_vol.setSensitiveDetector(aSensDet);
    DD4hep::Geometry::PlacedVolume cryo_physvol = envelope_vol.placeVolume(cryo_vol);
    cryo_physvol.addPhysVolID("part",1);
  }
  // 2. Create bath that is inside the cryostat and surrounds the calorimeter
  //    Bath is filled with active material -> but not sensitive
  DD4hep::Geometry::Volume bath_vol(active_mat + "_bath", bath_outer_shape, aLcdd.material(active_mat));
  lLog << MSG::DEBUG << "ECAL bath: material = " << active_mat << " rmin (cm) =  "
       << bath_rmin << " rmax (cm) = " << bath_rmax
       << " thickness in front of ECal (cm) = " << calo_dim.rmin() - cryo_dim.rmin2()
       << " thickness behind ECal (cm) = " << cryo_dim.rmax1() - calo_dim.rmax()  << endmsg;

  // 3. Create the calorimeter by placing the passive material, trapezoid active cells, readout and again trapezoid active cells in the bath.
  // sensitive detector for the cells
  DD4hep::Geometry::SensitiveDetector sd = aSensDet;
  DD4hep::XML::Dimension sd_typ = xml_det.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());
  lLog << MSG::DEBUG << "ECAL sensitive volume rmin (cm) =  "
       << calo_dim.rmin() << " rmax (cm) = " << calo_dim.rmax() << endmsg;

  // 3.a. Create the passive planes, readout in between of 2 passive planes and the remaining space fill with active material
  //////////////////////////////
  // PASSIVE PLANES
  //////////////////////////////
  lLog << MSG::DEBUG << "passive inner material = " << passive_inner_mat << " and outer material = "
       << passive_outer_mat << " thickness of inner part (cm) =  "
       << passive_inner_thickness << " thickness of outer part (cm) =  "
       << passive_outer_thickness << " thickness of total (cm) =  "
       << passive_thickness << " rotation angle = " << angle << endmsg;
  uint numPlanes = M_PI / asin( (passive_thickness + active_thickness + readout_thickness) / (2. * calo_dim.rmin() * cos(angle) )) ;
  double dPhi = 2. * M_PI / numPlanes;
  lLog << MSG::DEBUG << "number of passive plates = " << numPlanes << " azim. angle difference =  " << dPhi << endmsg;
  lLog << MSG::DEBUG << " distance at inner radius (cm) = " << 2. * M_PI * calo_dim.rmin() / numPlanes
       << " distance at outer radius (cm) = " << 2. * M_PI * calo_dim.rmax() / numPlanes << endmsg;
  // Readout is in the middle between two passive planes
  double offsetPassivePhi = calo_dim.offset() + dPhi / 2.;
  double offsetReadoutPhi = calo_dim.offset() + 0;
  lLog << MSG::DEBUG << "readout material = " << readout_mat << " thickness of readout planes (cm) =  "
       << readout_thickness << " readout cell height = " << cell_height << endmsg;
  double Rmin = calo_dim.rmin();
  double Rmax = calo_dim.rmax();
  double dR = Rmax - Rmin;
  double plane_length = - Rmin * cos(angle) + sqrt( pow(Rmax,2) - pow( Rmin * sin(angle), 2) );
  lLog << MSG::DEBUG << "thickness of calorimeter (cm) = " << dR << " length of passive or readout planes (cm) =  "
       << plane_length << endmsg;
  DD4hep::Geometry::Box passive_shape(passive_thickness / 2., calo_dim.dz(), plane_length / 2.);
  DD4hep::Geometry::Volume passive_vol("passive", passive_shape, aLcdd.material(passive_inner_mat));

  //////////////////////////////
  // READOUT PLANES
  //////////////////////////////
  DD4hep::Geometry::Box readout_shape(readout_thickness / 2., calo_dim.dz(), plane_length / 2.);
  DD4hep::Geometry::Volume readout_vol(readout_mat, readout_shape , aLcdd.material(readout_mat));

  //////////////////////////////
  // ACTIVE
  //////////////////////////////
  // thickness of active cells at inner radius and outer ( = distance between passive plane and readout plane)
  // at inner radius: distance projected at plane perpendicular to readout plane
  double active_in_thck = Rmin * sin(dPhi / 2.) * cos(angle);
  // // decreased by the thickness of passive and readout planes
  // active_in_thck -= passive_thickness / 2. + readout_thickness / 2.;
  active_in_thck -= passive_thickness * (0.5 - active_passive_overlap);
  // at outer radius: distance projected at plane perpendicular to readout plane
  double active_out_thck = (Rmin + plane_length) * sin(dPhi / 2.) * cos(angle);
  // make correction for outer readius caused by inclination angle
  // first calculate intersection of readout plane and plane parallel to shifted passive plane
  double xIntersect = (Rmin * (tan(angle) - cos(dPhi / 2.) * tan(angle + dPhi / 2.)) - plane_length * sin(dPhi / 2.)) /
    (tan(angle) - tan(angle + dPhi / 2.));
  double yIntersect = tan(angle) * xIntersect + Rmin * (sin(dPhi / 2.) - tan(angle)) + plane_length * sin(dPhi / 2.);
  // distance from inner radius to intersection
  double correction = plane_length - sqrt(pow(xIntersect - Rmin * cos(dPhi/2), 2) +
    pow(yIntersect - Rmin * sin(dPhi/2), 2));
  // correction to the active thickness
  active_out_thck += 2. * correction * sin(dPhi / 4.);
  // // decreased by the thickness of passive and readout planes
  // active_out_thck -= passive_thickness / 2. + readout_thickness / 2.;
  active_out_thck -= passive_thickness * (0.5 - active_passive_overlap);
  lLog << MSG::DEBUG << "active material = " << active_mat << " active cells thickness at inner radius (cm) = "
       << active_in_thck << " thickness at outer radious (cm) = " << active_out_thck << " making "
       << (active_out_thck - active_in_thck) * 100 / active_in_thck << " % increase." << endmsg;

  // creating shape for rows of cells (active material between two passive planes, with readout in the middle)
  // first define area between two passive planes, area can reach up to the symmetry axis of passive plane
  DD4hep::Geometry::Trapezoid active_outer_shape(active_in_thck, active_out_thck,
    calo_dim.dz(), calo_dim.dz(), plane_length / 2.);
  // subtract readout shape from the middle
  // DD4hep::Geometry::Trapezoid active_shape_noReadout(active_outer_shape);
  DD4hep::Geometry::SubtractionSolid active_shape_noReadout(active_outer_shape, readout_shape);

  // make calculation for active plane that is inclined with 0 deg (= offset + angle)
  double Cx = Rmin * cos(-angle) + plane_length / 2.;
  double Cy = Rmin * sin(-angle);
  double Ax = Rmin * cos(-angle + dPhi / 2.) + plane_length / 2. * cos(dPhi / 2.);
  double Ay =  Rmin * sin(-angle + dPhi / 2.) + plane_length / 2. * sin(dPhi / 2.);
  double CAx = fabs(Ax - Cx);
  double CAy = fabs(Ay - Cy);
  double zprim, xprim;
  zprim = CAx;
  xprim = CAy;

  double Bx =  Rmin * cos( -angle - dPhi / 2.) + plane_length / 2. * cos( - dPhi / 2. );
  double By =  Rmin * sin( -angle - dPhi / 2.) + plane_length / 2. * sin( - dPhi / 2. );
  double CBx = fabs(Bx - Cx);
  double CBy = fabs(By - Cy);
  double zprimB, xprimB;
  zprimB = CBx;
  xprimB = CBy;

  // subtract passive volume above
  DD4hep::Geometry::SubtractionSolid active_shape_noPassiveAbove(active_shape_noReadout, passive_shape,
    DD4hep::Geometry::Transform3D(
      DD4hep::Geometry::RotationY( - dPhi / 2.),
      DD4hep::Geometry::Position( - fabs(xprim), 0, fabs(zprim) ) ));
  // subtract passive volume below
  DD4hep::Geometry::SubtractionSolid active_shape(active_shape_noPassiveAbove, passive_shape,
    DD4hep::Geometry::Transform3D( DD4hep::Geometry::RotationY( dPhi / 2.),
      DD4hep::Geometry::Position(  fabs(xprimB), 0, -fabs(zprimB))));
  DD4hep::Geometry::Volume active_vol("active", active_shape, aLcdd.material(active_mat) );
  active_vol.setSensitiveDetector(aSensDet);

  bath_vol.setSensitiveDetector(aSensDet);
  passive_vol.setSensitiveDetector(aSensDet);
  readout_vol.setSensitiveDetector(aSensDet);
  // Next place elements: passive planes, readout planes and rows of cells
  for(uint iPlane = 0; iPlane < numPlanes; iPlane ++) {
    // first calculate positions of passive and readout planes
    // PASSIVE
    // calculate centre position of the plane without plane rotation
    double phi = offsetPassivePhi + iPlane * dPhi;
    double xRadial = (Rmin + plane_length / 2.) * cos(phi);
    double yRadial = (Rmin + plane_length / 2.) * sin(phi);
    // calculate position of the beginning of plane
    double xRmin = Rmin * cos(phi);
    double yRmin = Rmin * sin(phi);
    // rotate centre by angle wrt beginning of plane
    double xRotated = xRmin + (xRadial - xRmin) * cos(angle) - (yRadial - yRmin) * sin(angle);
    double yRotated = yRmin + (xRadial - xRmin) * sin(angle) + (yRadial - yRmin) * cos(angle);
    DD4hep::Geometry::Transform3D transform( DD4hep::Geometry::RotationX(- M_PI / 2.) // to get in XY plane
      * DD4hep::Geometry::RotationY(M_PI / 2. // to get pointed towards centre
        - phi - angle),
      DD4hep::Geometry::Position( xRotated, yRotated, 0) );
    DD4hep::Geometry::PlacedVolume passive_physvol = bath_vol.placeVolume(passive_vol, transform);
    passive_physvol.addPhysVolID("passive", iPlane+1);

    // READOUT
    // calculate centre position of the plane without plane rotation
    double phiRead = offsetReadoutPhi + iPlane * dPhi;
    double xRadialRead = (Rmin + plane_length / 2.) * cos(phiRead);
    double yRadialRead = (Rmin + plane_length / 2.) * sin(phiRead);
    // calculate position of the beginning of plane
    double xRminRead = Rmin * cos(phiRead);
    double yRminRead = Rmin * sin(phiRead);
    // rotate centre by angle wrt beginning of plane
    double xRotatedRead = xRminRead + (xRadialRead - xRminRead) * cos(angle) - (yRadialRead - yRminRead) * sin(angle);
    double yRotatedRead = yRminRead + (xRadialRead - xRminRead) * sin(angle) + (yRadialRead - yRminRead) * cos(angle);
    DD4hep::Geometry::Transform3D transformRead ( DD4hep::Geometry::RotationX(- M_PI / 2.) // to get in XY plane
      * DD4hep::Geometry::RotationY(M_PI / 2. // to get pointed towards centre
        - phiRead - angle),
      DD4hep::Geometry::Position(xRotatedRead, yRotatedRead, 0) );
    DD4hep::Geometry::PlacedVolume readout_physvol = bath_vol.placeVolume(readout_vol, transformRead);
    readout_physvol.addPhysVolID("readout", iPlane+1);

    // ACTIVE
    DD4hep::Geometry::Rotation3D rotationActive (
      DD4hep::Geometry::RotationX(- M_PI/2) *
      DD4hep::Geometry::RotationY(M_PI/2 - phiRead - angle));
    DD4hep::Geometry::PlacedVolume active_physvol = bath_vol.placeVolume(active_vol,
      DD4hep::Geometry::Transform3D(rotationActive,
        DD4hep::Geometry::Position(xRotatedRead,yRotatedRead, 0)
        ));
    active_physvol.addPhysVolID("row", iPlane + 1);

  }
  DD4hep::Geometry::PlacedVolume bath_physvol = envelope_vol.placeVolume(bath_vol);

  // Place the envelope
  DD4hep::Geometry::Volume mother_vol = aLcdd.pickMotherVolume(det_element);
  DD4hep::Geometry::PlacedVolume ECalBarrel_physvol = mother_vol.placeVolume(envelope_vol);
  ECalBarrel_physvol.addPhysVolID("system", xml_det.id());
  det_element.setPlacement(ECalBarrel_physvol);
  return det_element;

}
} // namespace det

DECLARE_DETELEMENT(EmCaloBarrelInclined, det::createECalBarrelInclined)

