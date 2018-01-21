//
//  DetCylinderLayer.h
//
//
//  Created by Julia Hrdinka on 12/12/14.
//
//

#ifndef DET_DETCYLINDERLAYER_H
#define DET_DETCYLINDERLAYER_H

#include "DetExtensions/IDetExtension.h"

namespace dd4hep {
namespace Geometry {
class DetElement;
}
}

namespace Det {

class DetCylinderLayer : public IDetExtension {

public:
  DetCylinderLayer() {}

  DetCylinderLayer(const DetCylinderLayer&, const dd4hep::DetElement&) {}

  virtual ~DetCylinderLayer() {}

  virtual ExtensionType type() { return ExtensionType::CylinderLayer; }
};
}

#endif  // DET_DETCYLINDERLAYER_H
