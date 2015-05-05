//
//  PlotHits.h
//  
//
//  Created by Julia Hrdinka on 29/04/15.
//
//

#ifndef PLOTHITS_H
#define PLOTHITS_H

//Interface
#include "TestInterfaces/IPrintHits.h"
//Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
//ROOT
#include "TH1F.h"
#include "TProfile.h"
#include "TFile.h"

#include "RecoGeometry/Surface.h"
//#include "Algebra/AlgPrimitives.h"

//std
#include <iostream>
#include <fstream>
#include <stdlib.h>

class PrintHits : public AlgTool, virtual public IPrintHits {

public:
    
    PrintHits(const std::string& type, const std::string& name, const IInterface* parent);
    ~PrintHits() {}
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode printMaterial(std::vector<std::tuple<const Reco::Surface*, const Alg::Point3D, const Alg::Vector3D>>& hits) override;
    virtual StatusCode printHits(std::vector<std::pair<double, const Alg::Vector3D>>& hits) override;
   /// StatusCode plotHits()
    
private:
    MsgStream       m_log;
    ITHistSvc*      m_ths;
    TProfile*       m_tInX0;
    TProfile*       m_hitsEta;
    TProfile*       m_hitsPhi;
    TProfile*       m_ZEta;
    TProfile*       m_AEta;
    TProfile*       m_SensEta;
    mutable std::ofstream m_out;
    TFile* file;
};

#endif //PLOTHITS_H


