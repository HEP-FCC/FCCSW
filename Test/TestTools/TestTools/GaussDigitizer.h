//
//  GaussDigitizer.h
//  
//
//  Created by Julia Hrdinka on 22/04/15.
//
//

#ifndef GAUSSDIGITIZER_H
#define GAUSSDIGITIZER_H

//Interface
#include "TestInterfaces/IDigitizer.h"
//Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

#include "DataObjects/Particle.h"
#include "DataObjects/ParticleCollection.h"
//std
#include <iostream>
#include <fstream>
#include <stdlib.h>

class GaussDigitizer : public AlgTool, virtual public IDigitizer {
    
public:
    GaussDigitizer(const std::string& type, const std::string& name, const IInterface* parent);
    ~GaussDigitizer() {}
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode smear(ParticleCollection* particlecoll) override;
    
private:
    
//    mutable SmartIF<IRndmGenSvc>      m_RGS;      ///< Random Number Generator Service
    IRndmGenSvc*    m_RGS;
    Rndm::Numbers   m_gaussDist;

};


#endif //GAUSSDIGITIZER_H
