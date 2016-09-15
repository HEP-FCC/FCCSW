#include "RecCommon/SegmentationHelper.h"

namespace rec {
namespace segmentation {

std::vector<uint64_t> neighbours(DD4hep::DDSegmentation::BitField64& aDecoder,
    const std::vector<std::string>& aDimensionNames,
    uint64_t aCellId) {
  std::vector<uint64_t> neighbours;
  aDecoder.setValue(aCellId);
  for(const auto& dim: aDimensionNames) {
    int id = aDecoder[dim];
    // istead of catching exceptions - check max num of cell
    try {
      neighbours.emplace_back(aDecoder.getValue());
      aDecoder[dim] = id+1;
      neighbours.emplace_back(aDecoder.getValue());
    } catch (std::runtime_error e) {}
    try {
      aDecoder[dim] = id-1;
      neighbours.emplace_back(aDecoder.getValue());
    } catch (std::runtime_error e) {}
    aDecoder[dim] = id;
  }
  return neighbours;
}

std::vector<std::pair<int,int>> bitfieldExtremes(DD4hep::DDSegmentation::BitField64& aDecoder) {
  std::vector<std::pair<int,int>> extremes;
  int width = 0;
  for(uint it_fields=0;it_fields<aDecoder.size();it_fields++) {
    width = aDecoder[it_fields].width();
    // compute extreme values
    if( aDecoder[it_fields].isSigned() ){
      extremes.emplace_back(std::make_pair( ( 1LL << ( width - 1 ) ) - ( 1LL << width ),
          ( 1LL << ( width - 1 ) ) - 1));
    } else {
      extremes.emplace_back(std::make_pair( 0,
         (0x0001 << width) - 1 ));
    }
  }
  return extremes;
}
}
}
