#include "ACTSLogger.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"


namespace Acts {
std::unique_ptr<Logger> getDefaultLogger(const std::string& name, const Logging::Level& lvl, std::ostream*) {
  using namespace Logging;
  //ServiceHandle<IMessageSvc>* msgSvc = new ServiceHandle<IMessageSvc>("MessageSvc", name);
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", name);
  msgSvc->setOutputLevel(lvl + 1);
  auto printPol = std::make_unique<GaudiPrintPolicy>(&(*msgSvc));
  printPol->setName(name);
  return std::make_unique<Acts::Logger>(std::move(printPol),
                                        std::make_unique<GaudiFilterPolicy>(&(*msgSvc)));
}
}
