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
 *  Tool that reads a ROOT file containing the TTree with branchs "cellId", "noiseLevel", and "noiseOffset".
 *  This tool reads the tree, creates a map, and allows a lookup of noise level and mean noise of a cell, by its cellID.
 *
 *  @author Coralie Neubueser
 */

class TopoCaloNoisyCells : public GaudiTool, virtual public ICaloReadCellNoiseMap {
public:
  TopoCaloNoisyCells(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TopoCaloNoisyCells() = default;
  /** Read a root file and the stored TTree of cellIDs to noise values.
   * return StatusCode
   */
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;
 
  /** Expected noise per cell in terms of sigma of Gaussian distibution.
   *   @param[in] aCellId of the cell of interest.
   *   return double.
   */
  virtual double noiseRMS(uint64_t aCellId) final;
  
  /** Expected noise per cell in terms of mean of distibution.
   *   @param[in] aCellId of the cell of interest.
   *   return double.
   */ 
  virtual double noiseOffset(uint64_t aCellId) final;

private:
  /// Name
  Gaudi::Property<std::string> m_fileName{this, "fileName",
                                          "/afs/cern.ch/user/c/cneubuse/public/FCChh/cellNoise_map_segHcal.root"};
  std::unordered_map<uint64_t, std::pair<double,double>> m_map;
};

#endif /* RECCALORIMETER_TOPOCALONOISYCELLS_H */
