#ifndef RECINTERFACE_ICALOREADCELLNOISEMAP_H
#define RECINTERFACE_ICALOREADCELLNOISEMAP_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class ICaloReadCellNoiseMap RecInterface/RecInterface/ICaloReadCellNoiseMap.h ICaloReadCellNoiseMap.h
 *
 *  Interface to the service creating a map for the calorimetry.
 *
 *  @author Coralie Neubueser
 */

class ICaloReadCellNoiseMap : virtual public IAlgTool {
public:
  DeclareInterfaceID(ICaloReadCellNoiseMap, 1, 0);

  virtual double noiseRMS(uint64_t aCellId) = 0;
  virtual double noiseOffset(uint64_t aCellId) = 0;
};
#endif /* RECINTERFACE_ICALOREADNEIGHBOURSMAP_H */
