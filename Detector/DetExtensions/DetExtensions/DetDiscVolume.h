//
//  DetDiscVolume.h
//
//
//  Created by Julia Hrdinka on 19/01/15.
//
//

#ifndef DET_DETDISCVOLUME_H
#define DET_DETDISCVOLUME_H

#include "DetExtensions/IDetExtension.h"

namespace DD4hep {
namespace Geometry {
class DetElement;
}
}

namespace Det {

class DetDiscVolume : public IDetExtension {

public:
  explicit DetDiscVolume(int status) : m_status(status) {}
  DetDiscVolume(const DetDiscVolume& volume, const DD4hep::Geometry::DetElement&) { m_status = volume.m_status; }
  virtual ~DetDiscVolume() {}
  int status() { return m_status; }
  virtual ExtensionType type() { return ExtensionType::DiscVolume; }

private:
  int m_status;
};
}

#endif  // DET_DETDISCVOLUME_H
