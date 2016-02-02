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

StatusCode SmearingTool::smear(fcc::ParticleCollection* particlecoll)
{
	// FIXME / TODO: This function should be renamed or re-written, as I would expect something else from the name...
    for (const auto& part : *particlecoll) {
        m_points << part.Core().Vertex.X << " " << part.Core().Vertex.Y << " " << part.Core().Vertex.Z << std::endl;
    }

    return StatusCode::SUCCESS;
}
