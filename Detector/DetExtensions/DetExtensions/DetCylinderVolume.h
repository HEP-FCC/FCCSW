//
//  DetCylinderVolume.h
//
//
//  Created by Julia Hrdinka on 19/01/15.
//
//

#ifndef DET_DETCYLINDERVOLUME_H
#define DET_DETCYLINDERVOLUME_H

#include "DetExtensions/IDetExtension.h"

namespace DD4hep {
namespace Geometry {
class DetElement;
}
}

namespace Det {

class DetCylinderVolume : public IDetExtension {

public:
  explicit DetCylinderVolume(int status) : m_status(status) {}
  DetCylinderVolume(const DetCylinderVolume& volume, const DD4hep::Geometry::DetElement&) {
    m_status = volume.m_status;
  }
  virtual ~DetCylinderVolume() {}
  int status() { return m_status; }
  virtual ExtensionType type() { return ExtensionType::CylinderVolume; }

private:
  int m_status;
};
}

#endif  // DET_DETCYLINDERVOLUME_H
