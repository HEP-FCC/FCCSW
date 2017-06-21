#ifndef SIMPAPAS_IPAPASTOOL_H
#define SIMPAPAS_IPAPASTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "papas/datatypes/Event.h"

/** @class IPapasTool IPapasTool.h
 *
 *  Interface for a tool to run under Papas
 *
 *  @author A. Robson
 */

class IPapasTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IPapasTool, 1, 0);

  /**  Create any output structures that are needed
   *   @return status code
   */
  virtual StatusCode createOutputStructures() = 0;

  /**  Empty the tool storage structures ready for next event
   *   @return status code
   */
  virtual StatusCode clear() = 0;

  /**  Run the tool
   *   @param[in] pEvent PapasEvent that contains collections of clusters, particles etc
   *   @return status code
   */
  virtual StatusCode run(papas::Event& pEvent) = 0;
};
#endif /* SIMPAPASINTERFACE_IPAPASTOOL_H */
