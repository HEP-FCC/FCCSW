//
//  CompareMaterial.h
//  
//
//  Created by Julia Hrdinka on 02/06/15.
//
//

#ifndef _CompareMaterial_h
#define _CompareMaterial_h

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DetDesInterfaces/IGeoSvc.h"
//tools
#include "GaudiKernel/IToolSvc.h"
#include "TestInterfaces/IPrintHits.h"

#include "DetDesInterfaces/IRecoGeoSvc.h"

class CompareMaterial: public GaudiAlgorithm {
    friend class AlgFactory<CompareMaterial> ;
    
public:
    /// Constructor.
    CompareMaterial(const std::string& name, ISvcLocator* svcLoc);
    /// Initialize.
    virtual StatusCode initialize();
    /// Execute.
    virtual StatusCode execute();
    /// Finalize.
    virtual StatusCode finalize();
    StatusCode runGeant4();
    
private:
    
    IGeoSvc*    m_geoSvc;
    IRecoGeoSvc* m_recoGeoSvc;
    IToolSvc*   m_toolsvc;
    IPrintHits* m_printhits;
    std::vector<Alg::Vector3D> m_directions;
    
};

#endif
