#include "SimPapas/PapasLogger.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

/*
 Based on ATS logging system
 This function is used to override the logger in the papaslibrary (using LDPRELOAD)
*/
namespace papaslog {
std::shared_ptr<const Logger> getDefaultLogger(const std::string& name, const Logging::Level&, std::ostream*) {
  using namespace Logging;
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", name);
  return std::make_shared<const papaslog::Logger>(std::make_shared<GaudiPrintPolicy>(&(*msgSvc)),
                                                  std::make_shared<GaudiFilterPolicy>(&(*msgSvc)));
}
}
