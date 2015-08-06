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

#include "GaudiKernel/ITHistSvc.h"
#include "TProfile.h"
#include "TFile.h"
//std
#include <iostream>
#include <fstream>
#include <stdlib.h>

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
 //   mutable std::ofstream m_radlen;
 //   ITHistSvc*      m_ths;
 //   TProfile*       m_tInX0;
 //   TFile* file;
    
};

#endif //GEANT4TEST_H
