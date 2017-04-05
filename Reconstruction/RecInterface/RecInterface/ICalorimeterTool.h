#ifndef RECINTERFACE_ICALORIMETERTOOL_H
#define RECINTERFACE_ICALORIMETERTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class ICalorimeterTool RecInterface/RecInterface/ICalorimeterTool.h ICalorimeterTool.h
 *
 *  Abstract interface to calorimeter geometry tool
 *
 *  @author Anna Zaborowska
 */

class ICalorimeterTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ICalorimeterTool, 1, 0);

  virtual StatusCode prepareEmptyCells(std::unordered_map<uint64_t, double>& aCells) = 0;
};

#endif /* RECINTERFACE_ICALORIMETERTOOL_H */
