#ifndef SIMPAPAS_IPAPASTOOL_H
#define SIMPAPAS_IPAPASTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#define WITHSORT 1
#include "papas/datatypes/Event.h"

namespace papas {
class Detector;
}

/** @class IPapasTool IPapasTool.h
 *
 *  Interface for a tool to run under Papas
 *
 *  @author A. Robson
 */

class IPapasTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IPapasTool, 1, 0);

  /**  Empty the tool storage structures ready for next event
   *   @return status code
   */
  virtual StatusCode clear() = 0;

  /**  Run the tool
   *   @param[in] pEvent PapasEvent that contains collections of clusters, particles etc
   *   @return status code
   */
  virtual StatusCode run(papas::Event& pEvent, std::shared_ptr<papas::Detector> det = nullptr) = 0;
};
#endif /* SIMPAPASINTERFACE_IPAPASTOOL_H */
