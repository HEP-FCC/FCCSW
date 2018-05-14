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
 *  @author Coralie Neubueser
 */

class TopoCaloNeighbours : public GaudiTool, virtual public ICaloReadNeighboursMap {
public:
  TopoCaloNeighbours(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TopoCaloNeighbours() = default;
 
  /** Read a map of cellIDs to vector of cellIDs (neighbours).
   */
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;
  
  /** Function to be called for the neighbours of a cell.
   *   @param[in] aCellId, cellid of the cell of interest.
   *   @return vector of cellIDs, corresponding to the cells neighbours.
   */
  virtual std::vector<uint64_t>& neighbours(uint64_t aCellId) final;

private:
  /// Name of input root file that contains the TTree with cellID->vec<neighboursCellID>
  Gaudi::Property<std::string> m_fileName{this, "fileName", "neighbours_map.root"};
  /// Output map to be used for the fast lookup in the topo-clusering algorithm
  std::unordered_map<uint64_t, std::vector<uint64_t>> m_map;
};

#endif /* RECCALORIMETER_TOPOCALONEIGHBOURS_H */
