#include "PodioInput.h"
#include "FWCore/PodioDataSvc.h"


DECLARE_COMPONENT(PodioInput)

PodioInput::PodioInput(const std::string& name, ISvcLocator* svcLoc) :
GaudiAlgorithm(name, svcLoc),m_first(true)
{
  declareProperty("filename", m_filename="input.root",
                  "Name of the file to read");
  std::vector<std::string> defaultCommands;
  defaultCommands.push_back("keep *");
  declareProperty("inputCommands", m_inputCommands=defaultCommands,
                  "A set of commands to declare which collections to keep or drop.");
}

StatusCode PodioInput::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  // check whether we have the PodioEvtSvc active
  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(evtSvc().get());
  if (nullptr == m_podioDataSvc) return StatusCode::FAILURE;
  m_switch = KeepDropSwitch(m_inputCommands);
  return StatusCode::SUCCESS;
}

StatusCode PodioInput::execute() {
  return StatusCode::SUCCESS;
}

StatusCode PodioInput::finalize() {
  if (GaudiAlgorithm::finalize().isFailure())
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}
