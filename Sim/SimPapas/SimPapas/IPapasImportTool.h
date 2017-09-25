#ifndef SIMPAPAS_IPAPASIMPORTTOOL_H
#define SIMPAPAS_IPAPASIMPORTTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "papas/datatypes/Event.h"

namespace papas {
class Detector;
}

/** @class IPapasImportTool IPapasImportTool.h
 *
 *  Interface for a data import tool to run under Papas
 *
 *  @author A. Robson
 */

class IPapasImportTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IPapasImportTool, 1, 0);
  
  /**  Empty the tool storage structures ready for next event
   *   @return status code
   */
  virtual StatusCode clear() = 0;
  
  /**  Run the tool
   *   @param[in] pEvent PapasEvent that contains collections of clusters, particles etc
   *   @return status code
   */
  virtual StatusCode run(papas::Event& pEvent,  unordered_map<papas::Identifier, int>& links, int>std::shared_ptr<papas::Detector> det = nullptr ) = 0;
};
#endif /* SIMPAPASINTERFACE_IPAPASIMPORTTOOL_H */
