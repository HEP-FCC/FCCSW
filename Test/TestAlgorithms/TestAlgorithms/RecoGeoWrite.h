#ifndef RECOGEOWRITE_H
#define RECOGEOWRITE_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "datamodel/Particle.h"
#include "datamodel/ParticleCollection.h"
#include "FWCore/DataHandle.h"
//std
#include <iostream>
#include <fstream>
#include <stdlib.h>
//tools
#include "GaudiKernel/IToolSvc.h"
#include "TestInterfaces/IDigitizer.h"

class RecoGeoWrite: public GaudiAlgorithm {
    friend class AlgFactory<RecoGeoWrite> ;
    
public:
    /// Constructor.
    RecoGeoWrite(const std::string& name, ISvcLocator* svcLoc);
    /// Initialize.
    virtual StatusCode initialize();
    /// Execute.
    virtual StatusCode execute();
    /// Finalize.
    virtual StatusCode finalize();
    
private:
  
    DataHandle<ParticleCollection>          m_particles;
    IToolSvc*                               m_toolsvc;
    IDigitizer*                             m_smeartool;
    
};

#endif //RECOGEOWRITE_H
