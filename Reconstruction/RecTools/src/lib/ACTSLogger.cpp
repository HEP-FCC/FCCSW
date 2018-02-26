#include "RecTracker/ACTSLogger.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace Acts {
std::unique_ptr<Logger> getDefaultLogger(const std::string& name, const Logging::Level&, std::ostream*) {
  using namespace Logging;
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", name);
  return std::make_unique<Acts::Logger>(std::make_unique<GaudiPrintPolicy>(&(*msgSvc)),
                                        std::make_unique<GaudiFilterPolicy>(&(*msgSvc)));
}
}
