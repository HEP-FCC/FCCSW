// DD4hep includes
#include "DD4hep/DetFactoryHelper.h"
// FCCSW includes
// #include "DetExtensions/DetCylinderVolume.h"


using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {

static DD4hep::Geometry::Ref_t create_hcal (
  DD4hep::Geometry::LCDD& lcdd,
  xml_h xmlElement,
  DD4hep::Geometry::SensitiveDetector sensDet
  ) {
  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  //Make DetElement
  DetElement dummyCalo(detName, xmlDet.id());
  // Get status for the RecoGeometry what is status?
  // xml_comp_t xmlStatus = xmlDet.child(_U(status));
  // int status = xmlStatus.id();
  // add Extension to Detlement for the RecoGeometry
  // Let's skip this for now...
  // Det::DetCylinderVolume* detVolume = new Det::DetCylinderVolume(status);
  // dummyCalo.addExtension<Det::IDetExtension>(detVolume);

  // Make Volume
  Dimension dimensions(xmlDet.dimensions());
  DD4hep::Geometry::Tube tubeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());

  Volume tubeVolume(detName, tubeShape, lcdd.air());
  // Invisibility seems to be broken, have to hardcode that
  // tubeVolume.setVisAttributes(lcdd, dimensions.visStr());
  tubeVolume.setVisAttributes(lcdd.invisible());

  sensDet.setType("Geant4Calorimeter"); // no idea yet, what this does...

  // structural support inside of HCal
  xml_comp_t xFacePlate = xmlElement.child("face_plate");
  double dRhoFacePlate = xFacePlate.thickness();
  DetElement facePlate("facePlate", 0);
  DD4hep::Geometry::Tube facePlateShape(dimensions.rmin() - dRhoFacePlate, dimensions.rmin(), dimensions.dz());
  Volume facePlateVol("facePlate", facePlateShape, lcdd.material(xFacePlate.materialStr()));
  facePlateVol.setVisAttributes(lcdd, xFacePlate.visStr());
  PlacedVolume placedFacePlate = tubeVolume.placeVolume(facePlateVol);
  placedFacePlate.addPhysVolID("facePlate", facePlate.id());
  facePlate.setPlacement(placedFacePlate);


  // structural support at both ends of HCal
  xml_comp_t xEndPlate = xmlElement.child("end_plate");
  double dZEndPlate = xEndPlate.thickness();

  DD4hep::Geometry::Tube endPlateShape(dimensions.rmin(), dimensions.rmax(), dZEndPlate);
  Volume endPlateVol("endPlate", endPlateShape, lcdd.material(xEndPlate.materialStr()));
  endPlateVol.setVisAttributes(lcdd, xEndPlate.visStr());

  DetElement endPlatePos("endPlate", 0);
  DD4hep::Geometry::Position posOffset(0, 0, dimensions.dz());
  // PlacedVolume placedEndPlatePos = tubeVolume.placeVolume(endPlateVol, posOffset);
  // placedEndPlatePos.addPhysVolID("endPlatePos", endPlatePos.id());
  // endPlatePos.setPlacement(placedEndPlatePos);

  DetElement endPlateNeg("endPlate", 1);
  DD4hep::Geometry::Position negOffset(0, 0, -dimensions.dz());
  PlacedVolume placedEndPlateNeg = tubeVolume.placeVolume(endPlateVol, negOffset);
  placedEndPlateNeg.addPhysVolID("endPlateNeg", endPlateNeg.id());
  endPlateNeg.setPlacement(placedEndPlateNeg);


  // Get both sequences
  xml_comp_t xSequenceA = xmlElement.child("sequence_a");
  xml_comp_t xSequenceB = xmlElement.child("sequence_b");
  // NOTE: This assumes that both have the same dimensions!
  Dimension moduleDimensions(xSequenceA.dimensions());
  double dzModule = moduleDimensions.dz();

  // Calculate size in X according to number of modules in phi:
  unsigned int numModulesPhi = moduleDimensions.phiBins();
  unsigned int numModulesZ = static_cast<unsigned>(dimensions.dz() / dzModule) * 2;
  unsigned int numModulesR = static_cast<unsigned>((dimensions.rmax() - dimensions.rmin()) / moduleDimensions.dr());

  double dzDetector = (numModulesZ / 2) * dzModule + dZEndPlate;
  std::cout << "correction of dz:" << dimensions.dz() - dzDetector << std::endl;
  std::cout << dimensions.dz() << " " << dzModule << " numRows = " << numModulesZ << std::endl;
  double dphi = 2 * dd4hep::pi / static_cast<double>(numModulesPhi);
  double tn = tan(dphi / 2.);
  double spacing = moduleDimensions.x();
  double dy1 = moduleDimensions.dz() / 2.;
  double dy2 = moduleDimensions.dz() / 2.;
  double dz0 = moduleDimensions.dr() / 2.;

  int moduleNumber = 0;
  for (unsigned int idxLayer = 0; idxLayer < numModulesR; ++idxLayer) {
    auto xModule = xSequenceB;
    if (idxLayer % 2 == 0) {
      xModule = xSequenceA;
    }
     // will be running index of loop over numModulesR
    double rminLayer = dimensions.rmin() + dRhoFacePlate + idxLayer * moduleDimensions.dr();
    double rmaxLayer = dimensions.rmin() + dRhoFacePlate + (idxLayer + 1) * moduleDimensions.dr();
    double dx1 = tn * rminLayer - spacing;
    double dx2 = tn * rmaxLayer - spacing;

    auto layerName = DD4hep::XML::_toString(idxLayer, "layer%d");
    // volume and DetElement for one layer in R
    Volume layerVol(layerName, DD4hep::Geometry::Tube(rminLayer, rmaxLayer, dzDetector), lcdd.air());
    // layerVol.setVisAttributes(lcdd.invisible());
    layerVol.setVisAttributes(lcdd, xModule.visStr());
    DetElement layerDet(dummyCalo, layerName, idxLayer);
    double rMiddle = rminLayer + 0.5 * moduleDimensions.dr();

    for (unsigned int idxZ = 0; idxZ < numModulesZ; ++idxZ) {
      auto zRowName = layerName + DD4hep::XML::_toString(idxZ, "zRow%d");

      double zOffset = -dzDetector + dZEndPlate * 2 + (idxZ + 0.5) * dzModule;
      // Iterate over tags <module_components...>
      for (unsigned int idxMod = 0; idxMod < numModulesPhi; ++idxMod, ++moduleNumber) {
        auto modName = zRowName + DD4hep::XML::_toString(idxMod, "mod%d");
        DetElement moduleDet(layerDet, modName, moduleNumber);
        Volume moduleVolume("module", DD4hep::Geometry::Trapezoid(
              dx1, dx2, dy1, dy2, dz0
            ), lcdd.material(xModule.materialStr())
        );

        // moduleVolume.setVisAttributes(lcdd, xModule.visStr());
        moduleVolume.setVisAttributes(lcdd.invisible());
        double phi = 0.5 * dphi - idxMod * dphi;
        double yPosModule = rMiddle * cos(phi);
        double xPosModule = rMiddle * sin(phi);
        DD4hep::Geometry::Position moduleOffset(xPosModule, yPosModule, zOffset);
        DD4hep::Geometry::Transform3D trans(
          DD4hep::Geometry::RotationX(-0.5*dd4hep::pi)*
          DD4hep::Geometry::RotationY(phi),
          // DD4hep::Geometry::RotationZ(-0.5*dd4hep::pi)
          moduleOffset
        );

        double previousYPos = -moduleDimensions.dz() / 2.;
        unsigned int idxModComp = 0;
        for (xml_coll_t xCompColl(xModule, _U(module_component)); xCompColl; ++xCompColl, ++idxModComp) {
          xml_comp_t xComp = xCompColl;
          std::string subModuleName = modName + DD4hep::XML::_toString(idxModComp, "module_component%d");
          double dyComp = xComp.thickness() / 2.;
          Volume modCompVol("module_component", DD4hep::Geometry::Trapezoid(
              dx1, dx2, dyComp, dyComp, dz0
            ), lcdd.material(xComp.materialStr())
          );
          // modCompVol.setVisAttributes(lcdd, xComp.visStr());
          modCompVol.setVisAttributes(lcdd.invisible());
          DetElement modCompDet(moduleDet,subModuleName, idxModComp + idxZ*numModulesZ);
          DD4hep::Geometry::Position offset(0, previousYPos + xComp.thickness() / 2., 0);
          PlacedVolume placedModCompVol = moduleVolume.placeVolume(modCompVol, offset);
          placedModCompVol.addPhysVolID("sub_module", idxModComp);
          previousYPos += xComp.thickness();
        }

        PlacedVolume placedModVolume = layerVol.placeVolume(moduleVolume, trans);
        placedModVolume.addPhysVolID("module", idxMod);
        moduleDet.setPlacement(placedModVolume);
      }
    }
    PlacedVolume placedLayerVolume = tubeVolume.placeVolume(layerVol);
    placedLayerVolume.addPhysVolID("layer", 0);
    layerDet.setPlacement(placedLayerVolume);
  }
  std::cout << "created nModules = " << moduleNumber << std::endl;
  //Place Volume
  Volume motherVol = lcdd.pickMotherVolume(dummyCalo);
  PlacedVolume placedTube = motherVol.placeVolume(tubeVolume);
  placedTube.addPhysVolID("tube", dummyCalo.id());
  dummyCalo.setPlacement(placedTube);
  return dummyCalo;

}
} // namespace hcal

DECLARE_DETELEMENT(CaloBarrel, det::create_hcal)

