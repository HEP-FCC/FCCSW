#ifndef DIGITIZATION_FCCDIGITIZATIONCELL_H
#define DIGITIZATION_FCCDIGITIZATIONCELL_H

#include "ACTS/Digitization/DigitizationCell.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "datamodel/TrackHit.h"

/** @class FCCDigitizationCell
 *
 *
 *  @author Julia Hrdinka
 *  @date   2018-02
 */

namespace sim {
struct FCCDigitizationCell : Acts::DigitizationCell {
  // connection to FCC track hit
  fcc::TrackHit trackHit;

  // construct them
  FCCDigitizationCell(fcc::TrackHit th, size_t ch0, size_t ch1, float d = 0.)
      : DigitizationCell(ch0, ch1, d), trackHit(th) {}
  // copy them
  FCCDigitizationCell(const FCCDigitizationCell& dc) : DigitizationCell(dc), trackHit(dc.trackHit) {}
};
}
#endif  // DIGITIZATION_FCCDIGITIZATIONCELL_H
