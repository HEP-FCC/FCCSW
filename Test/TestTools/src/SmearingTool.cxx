//
//  SmearingTool.cpp
//  
//
//  Created by Julia Hrdinka on 14/04/15.
//
//

#include "TestTools/SmearingTool.h"

DECLARE_COMPONENT(SmearingTool)

SmearingTool::SmearingTool(const std::string& type, const std::string& name, const IInterface* parent) :
AlgTool(type,name,parent)
{
    m_points.open("points.dat", std::ofstream::out);
}

StatusCode SmearingTool::initialize()
{
    declareInterface<IDigitizer>(this);
    
    return StatusCode::SUCCESS;
}

StatusCode SmearingTool::finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode SmearingTool::smear(ParticleCollection* particlecoll)
{
    for (const auto& part : *particlecoll) {
        m_points << part.read().Core.Vertex.X << " " << part.read().Core.Vertex.Y << " " << part.read().Core.Vertex.Z << std::endl;
    }

    return StatusCode::SUCCESS;
}