#ifndef ClicFIELD_H
#define ClicFIELD_H

#include "papas/detectors/Field.h"

namespace papas {

// Forward declaration
class VolumeCylinder;
/// Clic specific implementation of Detector Field element
///
class ClicField : public Field {
public:
  /** Constructor
   *
   * @param[in] volume The field cyclinders
   * @param[in] magnitude field strength
   */
  ClicField(const VolumeCylinder&& volume, double magnitude);
  /** Constructor
   *
   * @param[in] volume the field cyclinders
   * @param[in] magnitude field strength
   */ ClicField(const VolumeCylinder& volume, double magnitude);

private:
};

}  // end namespace papas
#endif
