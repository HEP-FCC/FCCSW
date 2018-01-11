//
//  DetSensComponent.h
//
//
//  Created by Julia Hrdinka on 15/06/15.
//
//

#ifndef DET_DETSENSCOMPONENT_H
#define DET_DETSENSCOMPONENT_H

#include "DetExtensions/IDetExtension.h"
#include <memory>

namespace dd4hep {
namespace Geometry {
class DetElement;
class Segmentation;
}
}

namespace Det {

class DetSensComponent : public IDetExtension {

public:
  explicit DetSensComponent(const dd4hep::Segmentation segmentation) : m_segmentation(segmentation) {}
  DetSensComponent(const DetSensComponent&, const dd4hep::DetElement&) {}
  virtual ~DetSensComponent() {}
  const dd4hep::Segmentation segmentation() { return (m_segmentation); }
  virtual ExtensionType type() { return ExtensionType::SensComponent; }

private:
  const dd4hep::Segmentation m_segmentation;
};
}

#endif  // DET_DETSENSCOMPONENT_H
