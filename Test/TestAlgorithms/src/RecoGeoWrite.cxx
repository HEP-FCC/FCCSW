#include "TestAlgorithms/RecoGeoWrite.h"


DECLARE_COMPONENT(RecoGeoWrite)

RecoGeoWrite::RecoGeoWrite(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc),
m_toolsvc(0),
m_smeartool(0)
{
    declareInput("particles", m_particles, "out");
}

StatusCode RecoGeoWrite::initialize() {
    
    if (GaudiAlgorithm::initialize().isFailure()){
        return StatusCode::FAILURE;
    }
    
    if (service("ToolSvc", m_toolsvc).isFailure()) {
        error() << "Couldn't get ToolSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    if (m_toolsvc->retrieveTool("GaussDigitizer", m_smeartool).isFailure()) {
        error() << "Couldn't get Smearing Tool" << endmsg;
    }
    
    return StatusCode::SUCCESS;
}


StatusCode RecoGeoWrite::execute() {
    
    ParticleCollection* particlecoll = m_particles.get();
    
    if (particlecoll) {
        m_smeartool->smear(particlecoll);
    }
    
    m_particles.put(particlecoll);
    
    return StatusCode::SUCCESS;
}


StatusCode RecoGeoWrite::finalize() {
    
    if (GaudiAlgorithm::finalize().isFailure())
    return StatusCode::FAILURE;
    
    return StatusCode::SUCCESS;
    
}
