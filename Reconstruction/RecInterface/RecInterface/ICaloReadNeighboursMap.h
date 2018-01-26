#ifndef RECINTERFACE_ICALOREADNEIGHBOURSMAP_H
#define RECINTERFACE_ICALOREADNEIGHBOURSMAP_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class ICaloReadNeighboursMap RecInterface/RecInterface/ICaloReadNeighboursMap.h ICaloReadNeighboursMap.h
 *
 *  Interface to the service creating a map for the calorimetry.
 *
 *  @author Anna Zaborowska
 */

class ICaloReadNeighboursMap : virtual public IAlgTool {
public:
  DeclareInterfaceID(ICaloReadNeighboursMap, 1, 0);

  virtual std::vector<uint64_t>& neighbours(uint64_t aCellId) = 0;
};
#endif /* RECINTERFACE_ICALOREADNEIGHBOURSMAP_H */
