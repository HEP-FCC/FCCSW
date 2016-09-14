
#include "DetCommon/DetUtils.h"

#include "DD4hep/DetFactoryHelper.h"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {

static DD4hep::Geometry::Ref_t createGenericTrackerBarrel(DD4hep::Geometry::LCDD& lcdd,
                                                          DD4hep::XML::Handle_t xmlElement,
                                                          DD4hep::Geometry::SensitiveDetector sensDet) {
  // shorthands
  DD4hep::XML::DetElement xmlDet = static_cast<DD4hep::XML::DetElement>(xmlElement);
  Dimension dimensions(xmlDet.dimensions());
  // get sensitive detector type from xml
  DD4hep::XML::Dimension sdTyp = xmlElement.child("sensitive");
  if (xmlDet.isSensitive()) {
    // sensitive detector used for all sensitive parts of this detector
    sensDet.setType(sdTyp.typeStr());
  }
  // definition of top volume
  // has min/max dimensions of tracker for visualization etc.
  std::string detectorName = xmlDet.nameStr();
  DetElement topDetElement(detectorName, xmlDet.id());
  DD4hep::Geometry::Tube topVolumeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume topVolume(detectorName, topVolumeShape, lcdd.air());
  topVolume.setVisAttributes(lcdd.invisible());

  // counts all layers - incremented in the inner loop over repeat - tags
  unsigned int layerCounter = 0;
  // loop over 'layer' nodes in xml
  for (DD4hep::XML::Collection_t xLayerColl(xmlElement, _U(layers)); nullptr != xLayerColl; ++xLayerColl) {
    DD4hep::XML::Component xLayer = static_cast<DD4hep::XML::Component>(xLayerColl);

    DD4hep::XML::Component xModuleComponents = xmlElement.child("module_components");
    DD4hep::XML::Component xModule =
        utils::getNodeByStrAttr(xmlElement, "module", "name", xLayer.attr<std::string>("module"));

    // optional parameters
    double stereo_offset = utils::getAttrValueWithFallback(xLayer, "stereo_offset", 0.0);
    double module_twist_angle = utils::getAttrValueWithFallback(xLayer, "module_twist_angle", 0.1 * M_PI);
    double stereo_module_overlap = utils::getAttrValueWithFallback(xLayer, "stereo_module_overlap", 0.0);

    // get total thickness of module
    unsigned int idxSubMod = 0;
    double totalModuleComponentThickness = 0;
    for (DD4hep::XML::Collection_t xCompColl(xModuleComponents, _U(module_component)); nullptr != xCompColl;
         ++xCompColl, ++idxSubMod) {
      DD4hep::XML::Component xComp = static_cast<DD4hep::XML::Component>(xCompColl);
      totalModuleComponentThickness += xComp.thickness();
    }
    // now that thickness is known: define module components volumes
    idxSubMod = 0;
    double integratedModuleComponentThickness = 0;
    std::vector<Volume> moduleComponentVector;
    for (DD4hep::XML::Collection_t xCompColl(xModuleComponents, _U(module_component)); nullptr != xCompColl;
         ++xCompColl, ++idxSubMod) {
      DD4hep::XML::Component xComp = static_cast<DD4hep::XML::Component>(xCompColl);
      std::string moduleComponentName = "layer" + std::to_string(layerCounter) + "_rod_module_component" +
          std::to_string(idxSubMod) + "_" + xComp.materialStr();
      Volume moduleComponentVolume(moduleComponentName,
                                   DD4hep::Geometry::Box(xModule.width(), xComp.thickness(), xModule.length()),
                                   lcdd.material(xComp.materialStr()));
      moduleComponentVolume.setVisAttributes(lcdd, xComp.visStr());
      if (xComp.isSensitive()) {
        moduleComponentVolume.setSensitiveDetector(sensDet);
      }
      moduleComponentVector.push_back(moduleComponentVolume);
    }

    // definition of module volume (smallest independent subdetector)
    // define the module whose name was given in the "layer" xml Element
    Volume moduleVolume("module", DD4hep::Geometry::Box(xModule.width(), xModule.thickness(), xModule.length()),
                        lcdd.material("Air"));
    moduleVolume.setVisAttributes(lcdd, xModule.visStr());

    // definition of rod volume (longitudinal arrangement of modules)
    Volume rodVolume("GenericTrackerBarrel_layer" + std::to_string(layerCounter) + "_rod",
                     DD4hep::Geometry::Box(xModule.width(), xModule.thickness(), xLayer.dz()),
                     lcdd.material("Air"));
    rodVolume.setVisAttributes(lcdd.invisible());

    /// @todo: allow for more than one type of module components
    // analogous to module
    // place module substructure in module
    std::string moduleComponentName = "moduleComponent";
    idxSubMod = 0;
    for (DD4hep::XML::Collection_t xCompColl(xModuleComponents, _U(module_component)); nullptr != xCompColl;
         ++xCompColl, ++idxSubMod) {
      DD4hep::XML::Component xComp = static_cast<DD4hep::XML::Component>(xCompColl);
      DD4hep::Geometry::Position offset(0, -0.5 * totalModuleComponentThickness + integratedModuleComponentThickness,
                                        0);
      integratedModuleComponentThickness += xComp.thickness();
      PlacedVolume placedModuleComponentVolume = moduleVolume.placeVolume(moduleComponentVector[idxSubMod], offset);
      placedModuleComponentVolume.addPhysVolID("module_component", idxSubMod);
    }

    // handle repeat attribute in xml
    // "repeat" layers  equidistant between rmin and rmax
    double numRepeat = xLayer.repeat();
    double layerThickness = (xLayer.rmax() - xLayer.rmin()) / numRepeat;
    double layer_rmin = xLayer.rmin();
    unsigned int nPhi = 0;
    double r = 0;
    double phi = 0;
    // loop over repeated layers defined by one layer tag
    for (unsigned int repeatIndex = 0; repeatIndex < numRepeat; ++repeatIndex) {
      ++layerCounter;
      // let r be the middle between two equidistant layer boundaries
      r = layer_rmin + (0.5 + repeatIndex) * layerThickness;
      // definition of layer volumes
      DD4hep::Geometry::Tube layerShape(r - 0.5*layerThickness, r + 0.5*layerThickness, xLayer.dz());
      std::string layerName = "layer" + std::to_string(layerCounter);
      Volume layerVolume(layerName, layerShape, lcdd.material("Silicon"));
      layerVolume.setVisAttributes(lcdd.invisible());
      PlacedVolume placedLayerVolume = topVolume.placeVolume(layerVolume);
      placedLayerVolume.addPhysVolID("layer", layerCounter);
      // approximation of tklayout values
      double phiOverlapFactor = utils::getAttrValueWithFallback(xLayer, "phi_overlap_factor", 1.15);
      nPhi = static_cast<unsigned int>( phiOverlapFactor * 2 * M_PI * r / (2 * xModule.width()));
      for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
        phi = 2 * M_PI * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
        DD4hep::Geometry::Translation3D lTranslation(r * cos(phi), r * sin(phi), 0);
        DD4hep::Geometry::RotationZ lRotation(phi + module_twist_angle + 0.5 * M_PI);
        PlacedVolume placedRodVolume = layerVolume.placeVolume(rodVolume, lTranslation * lRotation);
        placedRodVolume.addPhysVolID("rod", phiIndex);
      }
    }
    // placement of modules within rods
    unsigned int zRepeat = static_cast<int>(xLayer.dz() / (xModule.length() - stereo_module_overlap));
    // stereo overlap
    for (unsigned int zIndex = 0; zIndex < zRepeat; ++zIndex) {
      stereo_offset *= -1.;
      DD4hep::Geometry::Position moduleOffset(0, stereo_offset,
                                              zIndex * 2 * (xModule.length() - stereo_module_overlap) - xLayer.dz() +
                                                  xModule.length() - stereo_module_overlap);
      PlacedVolume placedModuleVolume = rodVolume.placeVolume(moduleVolume, moduleOffset);
      placedModuleVolume.addPhysVolID("module", zIndex);
    }
  }
  Volume motherVol = lcdd.pickMotherVolume(topDetElement);
  PlacedVolume placedGenericTrackerBarrel = motherVol.placeVolume(topVolume);
  placedGenericTrackerBarrel.addPhysVolID("system", topDetElement.id());
  topDetElement.setPlacement(placedGenericTrackerBarrel);
  return topDetElement;
}
}  // namespace det

DECLARE_DETELEMENT(GenericTrackerBarrel, det::createGenericTrackerBarrel)
