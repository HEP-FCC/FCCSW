#ifndef RECCALORIMETER_TOPOCALONOISYCELLS_H
#define RECCALORIMETER_TOPOCALONOISYCELLS_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ICaloReadCellNoiseMap.h"

class IGeoSvc;

/** @class TopoCaloNoisyCells Reconstruction/RecCalorimeter/src/components/TopoCaloNoisyCells.h
 *TopoCaloNoisyCells.h
 *
 *  Tool that reads a ROOT file containing the TTree with branch "cellId" and branch "neighbours".
 *  This tools reads the tree, creates a map, and allows a lookup of all neighbours of a cell.
 *
 *  @author Anna Zaborowska
 */

class TopoCaloNoisyCells : public GaudiTool, virtual public ICaloReadCellNoiseMap {
public:
  TopoCaloNoisyCells(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TopoCaloNoisyCells() = default;
  /** Read a map of cellIDs to vector of cellIDs.
   */
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;
  /** Prepare a map of all existing cells in current geometry.
   *   return Status code.
   */
  virtual double noise(uint64_t aCellId) final;

private:
  /// Name 
  Gaudi::Property<std::string> m_fileName{this, "fileName", "/afs/cern.ch/user/c/cneubuse/public/FCChh/cellNoise_map_segHcal.root"};
  std::unordered_map<uint64_t, double> m_map;
};

#endif /* RECCALORIMETER_TOPOCALONOISYCELLS_H */
