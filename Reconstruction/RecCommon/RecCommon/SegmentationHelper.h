#ifndef RECONSTRUCTION_SEGMENTATIONHELPER_H
#define RECONSTRUCTION_SEGMENTATIONHELPER_H

// DD4hep
#include "DDSegmentation/BitField64.h"


namespace rec {
namespace segmentation {

  /**  Get neighbours in many dimensions.
   *   @param[in] aDecoder Handle to the bitfield decoder.
   *   @param[in] aDimensionNames names of the identifiers for which neighbours are found.
   *   @param[in] aCellId ID of cell for whch one wants to look for neighbours.
   *   return vector of neighbours.
   */
std::vector<uint64_t> neighbours(DD4hep::DDSegmentation::BitField64& aDecoder,
  const std::vector<std::string>& aDimensionNames,
  uint64_t aCellId);

  /** Get minimal and maximal values that can be decoded in the fields of bitfield.
   *   @param[in] aDecoder Handle to the bitfield decoder.
   *   return vector of pairs (min,max)
   */
std::vector<std::pair<int,int>> bitfieldExtremes(DD4hep::DDSegmentation::BitField64& aDecoder);
}
}

#endif /* RECONSTRUCTION_SEGMENTATIONHELPER_H */
