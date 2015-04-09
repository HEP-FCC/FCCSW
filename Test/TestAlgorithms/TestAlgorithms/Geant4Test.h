//
//  Gent4Test.h
//  
//
//  Created by Julia Hrdinka on 26/02/15.
//
//

#ifndef GEANT4TEST_H
#define GEANT4TEST_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DetDesInterfaces/IGeoSvc.h"

class Geant4Test: public GaudiAlgorithm {
    friend class AlgFactory<Geant4Test> ;
    
public:
    /// Constructor.
    Geant4Test(const std::string& name, ISvcLocator* svcLoc);
    /// Initialize.
    virtual StatusCode initialize();
    /// Execute.
    virtual StatusCode execute();
    /// Finalize.
    virtual StatusCode finalize();
    
private:
    
    IGeoSvc* m_geoSvc;
    
};

#endif //GEANT4TEST_H
