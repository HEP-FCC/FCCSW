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

#include "DataObjects/Particle.h"
#include "DataObjects/ParticleCollection.h"
//std
#include <iostream>
#include <fstream>
#include <stdlib.h>

class SmearingTool : public AlgTool, virtual public IDigitizer {

public:
    SmearingTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~SmearingTool() {}
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode smear(ParticleCollection* particlecoll) override;
    
private:
    
    mutable std::ofstream   m_points;
};

#endif //SMEARINGTOOL_H
