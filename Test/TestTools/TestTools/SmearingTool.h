//
//  SmearingTool1.h
//
//
//  Created by Julia Hrdinka on 14/04/15.
//
//

#ifndef SMEARINGTOOL_H
#define SMEARINGTOOL_H

//Interface
#include "TestInterfaces/IDigitizer.h"
//Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "datamodel/Particle.h"
#include "datamodel/ParticleCollection.h"
//std
#include <iostream>
#include <fstream>
#include <stdlib.h>

class SmearingTool : public AlgTool, virtual public IDigitizer {

public:
    SmearingTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~SmearingTool() {}
    virtual StatusCode initialize() final;
    virtual StatusCode finalize() final;
    virtual StatusCode smear(fcc::ParticleCollection* particlecoll) final;

private:

    mutable std::ofstream   m_points;
};

#endif //SMEARINGTOOL_H
