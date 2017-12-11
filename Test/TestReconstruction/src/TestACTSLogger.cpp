#include "TestACTSLogger.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

#include "ACTS/Utilities/Logger.hpp"

using namespace Acts;

DECLARE_ALGORITHM_FACTORY(TestACTSLogger)

TestACTSLogger::TestACTSLogger(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {
}

TestACTSLogger::~TestACTSLogger() {}

StatusCode TestACTSLogger::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  {
  auto testLoggerInfo
        = Acts::getDefaultLogger("TestLoggerInfo", Acts::Logging::INFO);

          ACTS_LOCAL_LOGGER(testLoggerInfo);
          ACTS_INFO("TESTING INFO LOGGING LEVEL");
          ACTS_VERBOSE("TESTING DEBUG LOGGING LEVEL");
          ACTS_DEBUG("TESTING VERBOSE LOGGING LEVEL");
  }

  {

  auto testLoggerVerbose
        = Acts::getDefaultLogger("TestLoggerVerbose", Acts::Logging::VERBOSE);

          ACTS_LOCAL_LOGGER(testLoggerVerbose);
          ACTS_INFO("TESTING INFO LOGGING LEVEL");
          ACTS_VERBOSE("TESTING DEBUG LOGGING LEVEL");
          ACTS_DEBUG("TESTING VERBOSE LOGGING LEVEL");
  }


  return StatusCode::SUCCESS;
}

StatusCode TestACTSLogger::execute() {
  return StatusCode::SUCCESS;
}

StatusCode TestACTSLogger::finalize() { return GaudiAlgorithm::finalize(); }
