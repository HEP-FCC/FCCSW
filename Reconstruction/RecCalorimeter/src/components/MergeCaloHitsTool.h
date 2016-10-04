#ifndef RECCALORIMETER_MERGECALOHITSTOOL_H
#define RECCALORIMETER_MERGECALOHITSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/IMergeCaloHitsTool.h"


/** @class MergeCaloHitsTool
 *
 *  Tool for merging calo hits with same cellID. 
 *  mergeOneCollection - only one input collection.
 *  mergeTwoVectors - two vectors of hits to be merged. The second vector is expected to have unique cellIDs (no merging done inside this vector)!
 *  
 *  TODO
 *    - how do we want to calculate time? Average?
 *
 *  @author Jana Faltova
 *  @date   2016-09
 *
 */


class MergeCaloHitsTool : public GaudiTool, virtual public IMergeCaloHitsTool  {
public:
  MergeCaloHitsTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~MergeCaloHitsTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /** @brief Merge Geant4 energy deposits (aHits) with same cellID
  * Currently only energy of the hits are merged. Time not considered at the moment.
*/
  virtual std::vector<fcc::CaloHit*> mergeOneCollection(const fcc::CaloHitCollection& aHits) final;
  /** @brief Merge Geant4 energy deposits (aHits1 and aHits2) with the same cellID
   * two vectors of hits to be merged. The second vector is expected to have unique cellIDs (no merging done inside this vector)! 
   * Currently only energy of the hits are merged. Time not considered at the moment.
   */
  virtual std::vector<fcc::CaloHit*> mergeTwoVectors( const std::vector<fcc::CaloHit*>& aHits1, const std::vector<fcc::CaloHit*>& aHits2) final;

private:
  /// Merge CaloHits in mergedHits vector, prepare a map with cellID,pointer to a CaloHit
  void mergeHits(const std::vector<fcc::CaloHit*>& aHits, std::unordered_map<uint64_t, fcc::CaloHit*>& cellIdMap, std::vector<fcc::CaloHit*>& mergedHitsVector);
  /// Same as before, but for CaloHitCollection instead of vector
  void mergeHits(const fcc::CaloHitCollection& aHits, std::unordered_map<uint64_t, fcc::CaloHit*>& cellIdMap, std::vector<fcc::CaloHit*>& mergedHitsVector);
  /// Add a new CaloHit to the vector of mergedHits and cellID+position in the vector to the map
  void addHitToVectorAndMap(const fcc::CaloHit& ehit, std::unordered_map<uint64_t, fcc::CaloHit*>& cellIdMap,std::vector<fcc::CaloHit*>& mergedHitsVector);
  
};

#endif /* RECCALORIMETER_MERGECALOHITSTOOL_H */
