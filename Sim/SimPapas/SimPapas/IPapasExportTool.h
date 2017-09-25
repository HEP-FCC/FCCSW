#ifndef SIMPAPAS_IPAPASEXPORTTOOL_H
#define SIMPAPAS_IPAPASEXPORTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "papas/datatypes/Event.h"

namespace papas {
class Detector;
}

/** @class IPapasExportTool IPapasExportTool.h
 *
 *  Interface for a data export tool to run under Papas
 *
 *  @author A. Robson
 */

class IPapasExportTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IPapasExportTool, 1, 0);

  /**  Create any output structures that are needed
   *   @return status code
   */
  virtual StatusCode createOutputStructures() = 0;

  /**  Run the tool
   *   @param[in] pEvent PapasEvent that contains collections of clusters, particles etc
   *   @return status code
   */
  virtual StatusCode run(papas::Event& pEvent) = 0;
};
#endif /* SIMPAPASINTERFACE_IPAPASEXPORTTOOL_H */
