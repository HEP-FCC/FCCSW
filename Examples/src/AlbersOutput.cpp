#include "AlbersOutput.h"

DECLARE_COMPONENT(AlbersOutput)

AlbersOutput::AlbersOutput(const std::string& name, ISvcLocator* svcLoc) :
		GaudiAlgorithm(name, svcLoc)
{

}

StatusCode AlbersOutput::initialize() {
	if (GaudiAlgorithm::initialize().isFailure())
		return StatusCode::FAILURE;
        // check whether we have the AlbersEvtSvc active

	return StatusCode::SUCCESS;
}

StatusCode AlbersOutput::execute() {
  // look into content of the event
  return StatusCode::SUCCESS;
}

StatusCode AlbersOutput::finalize() {
	if (GaudiAlgorithm::finalize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}
