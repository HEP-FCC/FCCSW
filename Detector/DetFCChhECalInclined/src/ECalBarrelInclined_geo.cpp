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

  DD4hep::XML::DetElement cell = calo.child("cell");
  double cell_height = cell.dimensions().height();

  DD4hep::XML::DetElement readout = calo.child("readout");
  std::string readout_mat = readout.materialStr();
  double readout_thickness = readout.thickness();

  DD4hep::XML::DetElement passive = calo.child("passive");
  std::string passive_mat = passive.materialStr();
  double passive_thickness = passive.thickness();
  double angle = passive.rotation().angle();

  // 1. Create the tubes for the outer shapes of the volumes
  DD4hep::Geometry::Tube bath_outer_shape(cryo_dim.rmin() + cryo_thickness,
    cryo_dim.rmax() - cryo_thickness, cryo_dim.dz() - cryo_thickness);
  DD4hep::Geometry::Tube cryo_outer_shape(cryo_dim.rmin(), cryo_dim.rmax(), cryo_dim.dz());
  // Subtract volumes to get the actual shape of cryo
  DD4hep::Geometry::SubtractionSolid cryo_shape(cryo_outer_shape, bath_outer_shape);

  // 1. Create cryostat
  lLog << MSG::DEBUG << "ECAL cryostat: rmin (cm) = " << cryo_dim.rmin() << " rmax (cm) = "
       << cryo_dim.rmax() << " thickness (cm) = " << cryo_thickness << endmsg;
  DD4hep::Geometry::Volume cryoVol(cryostat.nameStr(), cryo_shape, aLcdd.material(cryostat.materialStr()));
  envelope_vol.placeVolume(cryoVol);

  // 2. Create bath that is inside the cryostat and surrounds the calorimeter
  //    Bath is filled with active material -> but not sensitive
  DD4hep::Geometry::Volume bath_vol(active_mat + "_bath", bath_outer_shape, aLcdd.material(active_mat));
  lLog << MSG::DEBUG << "ECAL bath: material = " << active_mat << " rmin (cm) =  "
       << cryo_dim.rmin() + cryo_thickness << " rmax (cm) = " << cryo_dim.rmax() - cryo_thickness << endmsg;

  // 3. Create the calorimeter by placing the passive material, trapezoid active cells, readout and again trapezoid active cells in the bath.
  // sensitive detector for the cells
  DD4hep::Geometry::SensitiveDetector sd = aSensDet;
  DD4hep::XML::Dimension sd_typ = xml_det.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());
  lLog << MSG::DEBUG << "ECAL sensitive volume: material = " << active_mat << " rmin (cm) =  "
       << calo_dim.rmin() << " rmax (cm) = " << calo_dim.rmax() << endmsg;

  // 3.a. Create the passive planes, readout in between of 2 passive planes and the remaining space fill with active material
  lLog << MSG::DEBUG << "passive material = " << passive_mat << " thickness of planes (cm) =  "
       << passive_thickness << " rotation angle = " << angle << endmsg;
  uint numPlanes = M_PI / asin( (passive_thickness + active_thickness + readout_thickness) / (2. * calo_dim.rmin() * cos(angle) )) ;
  double dPhi = 2. * M_PI / numPlanes;
  lLog << MSG::DEBUG << "number of passive plates = " << numPlanes << " azim. angle difference =  " << dPhi << endmsg;
  lLog << MSG::DEBUG << " distance at inner radius (cm) = " << 2. * M_PI * calo_dim.rmin() / numPlanes
       << " distance at outer radius (cm) = " << 2. * M_PI * calo_dim.rmax() / numPlanes << endmsg;
  // Readout is in the middle between two passive planes
  double offsetPassivePhi = 0;
  double offsetReadoutPhi = dPhi / 2.;
  lLog << MSG::DEBUG << "readout material = " << readout_mat << " thickness of readout planes (cm) =  "
       << readout_thickness << " readout cell height = " << cell_height << endmsg;
  double Rmin = calo_dim.rmin();
  double Rmax = calo_dim.rmax();
  double dR = Rmax - Rmin;
  double plane_length = - Rmin * cos(angle) + sqrt( pow(Rmax,2) - pow( Rmin * sin(angle), 2) );
  lLog << MSG::DEBUG << "thickness of calorimeter (cm) = " << dR << " length of passive or readout planes (cm) =  "
       << plane_length << endmsg;
  DD4hep::Geometry::Box passive_shape(plane_length / 2., passive_thickness / 2., calo_dim.dz());
  DD4hep::Geometry::Volume passive_vol(passive_mat, passive_shape , aLcdd.material(passive_mat));
  DD4hep::Geometry::Box readout_shape(plane_length / 2., readout_thickness / 2., calo_dim.dz());
  DD4hep::Geometry::Volume readout_vol(readout_mat, readout_shape , aLcdd.material(readout_mat));
  // thickness of active cells at inner radius and outer ( = distance between passive plane and readout plane)
  // at inner radius: distance projected at plane perpendicular to readout plane
  double active_in_thck = Rmin * sin(dPhi / 2.) * cos(angle);
  // decreased by the thickness of passive and readout planes
  active_in_thck -= passive_thickness / 2. + readout_thickness / 2.;
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
  // decreased by the thickness of passive and readout planes
  active_out_thck -= passive_thickness / 2. + readout_thickness / 2.;
  lLog << MSG::DEBUG << " active cells thickness at inner radius (cm) = " << active_in_thck
       << " thickness at outer radious (cm) = " << active_out_thck << " making "
       << (active_out_thck - active_in_thck) * 100 / active_in_thck << " % increase." << endmsg;

  // creating shapes for rows of cells (reflected along readout -> assumption of small phi angle)
  DD4hep::Geometry::Trap active_shape_below(
    plane_length / 2., // (after rotation) radial length
    atan((active_out_thck / 2. - active_in_thck / 2.)/(2.* (plane_length / 2.))) * 180 / M_PI, // right angle at one side
    0., calo_dim.dz(), // length along beam axis
    active_in_thck / 2. , active_in_thck / 2., // side at inner radius at low and high z
    0., calo_dim.dz(), // length along beam axis
    active_out_thck / 2. , active_out_thck / 2., // side at outer radius at low and high z
    0.);
  DD4hep::Geometry::Trap active_shape_above(
    plane_length / 2.,
    - atan((active_out_thck / 2. - active_in_thck / 2.)/(2.* (plane_length / 2.))) * 180 / M_PI, // right angle
    0., calo_dim.dz(),
    active_in_thck / 2. , active_in_thck / 2.,
    0., calo_dim.dz(),
    active_out_thck / 2. , active_out_thck / 2., 0.);
  DD4hep::Geometry::Volume active_vol_below("activeBelow", active_shape_below, aLcdd.material(active_mat) );
  DD4hep::Geometry::Volume active_vol_above("activeAbove", active_shape_above, aLcdd.material(active_mat) );

  // First place cells in rows
  uint numCells = plane_length / cell_height;
  cell_height = plane_length / numCells;
  double cell_in_thck = active_in_thck;
  float dThickness = (active_out_thck - active_in_thck) / numCells;
  std::cout <<" THICKNESS: " << dThickness << std::endl;
  float cell_out_thck = active_in_thck + dThickness;
  float offsetCellRadial = - plane_length / 2. + cell_height / 2.;
  float offsetCellX = - dThickness / 2. * (numCells / 2. - 0.5);
  lLog << MSG::DEBUG << " Number of cells for each readout plane = " << numCells
       << " Total number of cells  = " << numCells * numPlanes
       << " OFFSET = " << offsetCellX
       << " Corrected height of one cell (cm) = " << cell_height << endmsg;
  for(uint iCell = 0; iCell < numCells; iCell ++) {
    // Trapezoid shape for current radial distance
    DD4hep::Geometry::Trap cell_shape_below(
      cell_height / 2., // (after rotation) radial length
      atan((cell_out_thck / 2. - cell_in_thck / 2.)/(2.* (cell_height / 2.))) * 180 / M_PI, // right angle at one side
      0., calo_dim.dz(), // length along beam axis
      cell_in_thck / 2. , cell_in_thck / 2., // side at inner radius at low and high z
      0., calo_dim.dz(), // length along beam axis
      cell_out_thck / 2. , cell_out_thck / 2., // side at outer radius at low and high z
      0.);
    DD4hep::Geometry::Trap cell_shape_above(
      cell_height / 2.,
      - atan((cell_out_thck / 2. - cell_in_thck / 2.)/(2.* (cell_height / 2.))) * 180 / M_PI, // right angle at other side
      0., calo_dim.dz(),
      cell_in_thck / 2. , cell_in_thck / 2.,
      0., calo_dim.dz(),
      cell_out_thck / 2. , cell_out_thck / 2.,
      0.);

    DD4hep::Geometry::Volume cell_vol_below("cell_vol_below", cell_shape_below, aLcdd.material(active_mat) );
    DD4hep::Geometry::Volume cell_vol_above("cell_vol_above", cell_shape_above, aLcdd.material(active_mat) );
    cell_vol_below.setSensitiveDetector(aSensDet);
    cell_vol_below.setSensitiveDetector(aSensDet);
    DD4hep::Geometry::PlacedVolume cell_physvol_below = active_vol_below.placeVolume(cell_vol_below,
      DD4hep::Geometry::Position(offsetCellX + iCell * dThickness / 2.,
        0,
        offsetCellRadial + iCell * cell_height));
    DD4hep::Geometry::PlacedVolume cell_physvol_above = active_vol_above.placeVolume(cell_vol_above,
      DD4hep::Geometry::Position( - offsetCellX - iCell * dThickness / 2.,
        0,
        offsetCellRadial + iCell * cell_height));
    cell_in_thck = cell_out_thck;
    cell_out_thck += dThickness;
  }

  // Next place elements: passive planes, readout planes and rows of cells
  for(uint iPlane = 0; iPlane < numPlanes; iPlane ++) {
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
    DD4hep::Geometry::Transform3D transform( DD4hep::Geometry::RotationZ(phi + angle),
      DD4hep::Geometry::Position(xRotated, yRotated, 0) );
    DD4hep::Geometry::PlacedVolume passive_physvol = bath_vol.placeVolume(passive_vol, transform);

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
    DD4hep::Geometry::Transform3D transformRead (DD4hep::Geometry::RotationZ(phiRead + angle),
      DD4hep::Geometry::Position(xRotatedRead, yRotatedRead, 0) );
    DD4hep::Geometry::PlacedVolume readout_physvol = bath_vol.placeVolume(readout_vol, transformRead);

    // ACTIVE
    double dCentre = readout_thickness / 2. + (active_in_thck + active_out_thck) / 4.;
    double xRotatedActBelow = xRotatedRead + dCentre * sin(phiRead + angle);
    double yRotatedActBelow = yRotatedRead - dCentre * cos(phiRead + angle);
    double xRotatedActAbove = xRotatedRead - dCentre * sin(phiRead + angle);
    double yRotatedActAbove = yRotatedRead + dCentre * cos(phiRead + angle);

    DD4hep::Geometry::Rotation3D rotationActive (
      DD4hep::Geometry::RotationX(- M_PI / 2.) // to get trapezoid in XY plane
      * DD4hep::Geometry::RotationY(M_PI / 2. // to get trapezoid pointed towards centre
        - phiRead - angle)); // rotation of individual module
    DD4hep::Geometry::PlacedVolume active_physvol_below = bath_vol.placeVolume(active_vol_below,
      DD4hep::Geometry::Transform3D(rotationActive, DD4hep::Geometry::Position(xRotatedActBelow, yRotatedActBelow, 0) ));
    DD4hep::Geometry::PlacedVolume active_physvol_above = bath_vol.placeVolume(active_vol_above,
      DD4hep::Geometry::Transform3D(rotationActive, DD4hep::Geometry::Position(xRotatedActAbove, yRotatedActAbove, 0) ));
  }
  envelope_vol.placeVolume(bath_vol);

  // Place the envelope
  DD4hep::Geometry::Volume mother_vol = aLcdd.pickMotherVolume(det_element);
  DD4hep::Geometry::PlacedVolume ECalBarrel_physvol = mother_vol.placeVolume(envelope_vol);
  ECalBarrel_physvol.addPhysVolID("system", xml_det.id());
  det_element.setPlacement(ECalBarrel_physvol);
  return det_element;

}
} // namespace det

DECLARE_DETELEMENT(EmCaloBarrelInclined, det::createECalBarrelInclined)

