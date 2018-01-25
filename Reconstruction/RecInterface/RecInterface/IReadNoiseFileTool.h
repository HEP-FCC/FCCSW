#ifndef RECINTERFACE_IREADNOISEFILETOOL_H
#define RECINTERFACE_IREADNOISEFILETOOL_H

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class IReadNoiseFileTool
 *
 *  Abstract interface to read calorimeter noise file tool
 *
 *  @author Coralie Neubueser
 *  @date   2018-01
 */

class IReadNoiseFileTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IReadNoiseFileTool, 1, 0);

  virtual StatusCode initNoiseFromFile() = 0;
  virtual double getNoiseConstantPerCell(int64_t aCellID) = 0;
};

#endif /* RECINTERFACE_IREADNOISEFILETOOL_H */
