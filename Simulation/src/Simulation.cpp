#include "Simulation.h"


DECLARE_COMPONENT(Simulation)

Simulation::Simulation(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
{
  declareInput("hepmc", m_hepmchandle);
}

StatusCode Simulation::initialize() {
	if (GaudiAlgorithm::initialize().isFailure())
		return StatusCode::FAILURE;
	return StatusCode::SUCCESS;
}

StatusCode Simulation::execute() {
    return StatusCode::SUCCESS;
}

StatusCode Simulation::finalize() {
  return GaudiAlgorithm::finalize();
}
