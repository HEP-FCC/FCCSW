#include "TestAlgorithms/TestAlgorithm.h"


DECLARE_COMPONENT(TestAlgorithm)

TestAlgorithm::TestAlgorithm(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc)
{}

StatusCode TestAlgorithm::initialize() {
    
    if (GaudiAlgorithm::initialize().isFailure()){
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}


StatusCode TestAlgorithm::execute() {
    
  return StatusCode::SUCCESS;
}
    

StatusCode TestAlgorithm::finalize() {
    
    if (GaudiAlgorithm::finalize().isFailure())
        return StatusCode::FAILURE;
    
    return StatusCode::SUCCESS;
    
}
