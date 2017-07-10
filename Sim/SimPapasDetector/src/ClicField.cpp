/**
 * @file Clic.cc
 * @brief Implementation of the Clic detector
 */
//#include "papas/detectors/ClicField.h"
#include "ClicField.h"


namespace papas {

ClicField::ClicField(const VolumeCylinder& volume, double magnitude) : Field(volume, Material("void", 0, 0), magnitude) {}

ClicField::ClicField(const VolumeCylinder&& volume, double magnitude)
    : Field(volume, Material("void", 0, 0), magnitude) {}

}  // end namespace papas
