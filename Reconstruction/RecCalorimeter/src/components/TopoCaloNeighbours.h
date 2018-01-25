#ifndef RECCALORIMETER_TOPOCALONEIGHBOURS_H
#define RECCALORIMETER_TOPOCALONEIGHBOURS_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ICaloReadNeighboursMap.h"

class IGeoSvc;

/** @class TopoCaloNeighbours Reconstruction/RecCalorimeter/src/components/TopoCaloNeighbours.h
 *TopoCaloNeighbours.h
 *
 *  Tool that reads a ROOT file containing the TTree with branch "cellId" and branch "neighbours".
 *  This tools reads the tree, creates a map, and allows a lookup of all neighbours of a cell.
 *
 *  @author Anna Zaborowska
 */

class TopoCaloNeighbours : public GaudiTool, virtual public ICaloReadNeighboursMap {
public:
  TopoCaloNeighbours(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TopoCaloNeighbours() = default;
  /** Read a map of cellIDs to vector of cellIDs (neighbours).
   */
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;
  /** Prepare a map of all existing cells in current geometry.
   *   return Status code.
   */
  virtual std::vector<uint64_t>& neighbours(uint64_t aCellId) final;

private:
  /// Name 
  Gaudi::Property<std::string> m_fileName{this, "fileName", "/afs/cern.ch/user/a/azaborow/public/ECal/neighbours/neighbours_map.root"};
  std::unordered_map<uint64_t, std::vector<uint64_t>> m_map;
};

#endif /* RECCALORIMETER_TOPOCALONEIGHBOURS_H */
