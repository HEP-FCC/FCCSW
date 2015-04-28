//
//  GaussDigitizer.cxx
//  
//
//  Created by Julia Hrdinka on 22/04/15.
//
//

#include "TestTools/GaussDigitizer.h"

DECLARE_COMPONENT(GaussDigitizer)

GaussDigitizer::GaussDigitizer(const std::string& type, const std::string& name, const IInterface* parent) :
AlgTool(type,name,parent)
{}

StatusCode GaussDigitizer::initialize()
{
    declareInterface<IDigitizer>(this);
    
    StatusCode sc = service( "RndmGenSvc", m_RGS, true);
    if( sc.isFailure() ) {
        throw GaudiException("Service [RndmGenSvc] not found", name(), sc);
    }
    
    return StatusCode::SUCCESS;
}

StatusCode GaussDigitizer::finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode GaussDigitizer::smear(ParticleCollection* particlecoll)
{
    std::vector<ParticleHandle> particles = particlecoll->getHandles();
    //hier dann fehler holen
    StatusCode sc = m_gaussDist.initialize(m_RGS, Rndm::Gauss(0.,0.2)); //fearst mean, second sigma
    if (sc.isFailure()) {
       throw GaudiException("Couldn't initialize gauss distribution", name(), sc);
    }

    for (auto& part : particles) {
        //geht nicht, weil const zurueckgibt
        part.mod().Core.Vertex.X += m_gaussDist();
        part.mod().Core.Vertex.Y += m_gaussDist();
    }
    return StatusCode::SUCCESS;
}