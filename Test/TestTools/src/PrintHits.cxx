//
//  PrintHits.cxx
//  
//
//  Created by Julia Hrdinka on 29/04/15.
//
//

#include "TestTools/PrintHits.h"

#include "RecoGeometry/PlaneSurface.h"

DECLARE_COMPONENT(PrintHits)

PrintHits::PrintHits(const std::string& type, const std::string& name, const IInterface* parent) :
AlgTool(type,name,parent),
m_log(msgSvc(), name)
{}

StatusCode PrintHits::initialize()
{
    declareInterface<IPrintHits>(this);
    m_out.open ("tInX0_Eta.dat", std::ofstream::out);
    StatusCode sc = service("THistSvc", m_ths);
    if (sc.isFailure()) throw GaudiException("Service [THistSvc] not found", name(), sc);
    if (service("THistSvc", m_ths).isFailure()) m_log << MSG::ERROR << "Couldn't get THistSvc" << endmsg;
    file = new TFile("hitproperies.root", "RECREATE");
    if(!file->IsOpen()) m_log << MSG::WARNING << ("Could not open Root file") << endmsg;
    
    return StatusCode::SUCCESS;
}

StatusCode PrintHits::finalize()
{
    file->Print();
    return StatusCode::SUCCESS;
}

StatusCode PrintHits::printMaterial(std::vector<std::tuple<const Reco::Surface*, const Alg::Point3D, const Alg::Vector3D>>& hits)
{
    std::vector<std::tuple<const Reco::Surface*, const Alg::Point3D, const Alg::Vector3D>> keys_dedup;
    std::unique_copy(begin(hits),
                     end(hits),
                     back_inserter(keys_dedup),
                     [](const std::tuple<const Reco::Surface*, const Alg::Point3D, const Alg::Vector3D> &entry1,
                        const std::tuple<const Reco::Surface*, const Alg::Point3D, const Alg::Vector3D> &entry2) {
                         return (std::get<2>(entry1) == std::get<2>(entry2));}
                     );
    m_tInX0 = new TProfile("tInX0", "tInX0 over Eta", keys_dedup.size(), -10., 10.);
    if (m_ths->regHist("tInX0", m_tInX0).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    
    m_AEta = new TProfile("AInEta", "A over Eta", keys_dedup.size(), -10., 10.);
    if (m_ths->regHist("AInEta", m_AEta).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    
    m_ZEta = new TProfile("ZInEta", "Z over Eta", keys_dedup.size(), -10., 10.);
    if (m_ths->regHist("ZInEta", m_ZEta).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    
    m_SensEta = new TProfile("SensEta", "Sensitive percentage over Eta", keys_dedup.size(), -10., 10.);
    if (m_ths->regHist("SensEta", m_SensEta).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }

    for (auto& hit : hits)
    {
        Alg::Point2D locpos(0.,0.);

        if (std::get<0>(hit)->globalToLocal(std::get<1>(hit), Alg::Vector3D(),locpos) && std::get<0>(hit)) {
            float tInX0 = std::get<0>(hit)->material(locpos)->tInX0();
            tInX0 *= std::get<0>(hit)->pathlength(std::get<2>(hit))/(2.*std::get<0>(hit)->thickness());
            float eta   = std::get<2>(hit).Eta();
            Alg::Vector3D dir = std::get<2>(hit);
            float A = std::get<0>(hit)->material(locpos)->A();
            m_AEta->Fill(eta,A);
            float Z = std::get<0>(hit)->material(locpos)->Z();
            m_out << /*tInX0 << " " << eta << " " << dir.X() << " " << dir.Y() << " " << dir.Z() */ Z << " " << A << std::endl;
            m_tInX0->Fill(eta,tInX0);

            m_ZEta->Fill(eta,Z);
            float Sens = std::get<0>(hit)->material(locpos)->sensPer();
            m_SensEta->Fill(eta,Sens);
        }
    }
    std::cout << "SIZE: " << hits.size() << " " << keys_dedup.size() << std::endl;
//    m_tInX0->Rebin(10);
    m_out.close();
    std::cout << "PrintHits, after" << std::endl;
    m_tInX0->Write();
    m_AEta->Write();
    m_ZEta->Write();
    m_SensEta->Write();
    
    return StatusCode::SUCCESS;
}

StatusCode PrintHits::printHits(std::vector<std::pair<double, const Alg::Vector3D>>& hits)
{
  /*  std::vector<std::pair<double, const Alg::Vector3D>> keys_dedup;
    std::unique_copy(begin(hits),
                     end(hits),
                     back_inserter(keys_dedup),
                     [](const std::pair<double, const Alg::Vector3D> &entry1,
                        const std::pair<double, const Alg::Vector3D> &entry2) {
                         return (entry1.first == entry2.first);}
                     );*/
    std::cout << hits.size() << " SIZE " << hits.size() << std::endl;
    m_hitsEta = new TProfile("hitsEta", "Hits over Eta", hits.size(), -10., 10);
    if (m_ths->regHist("hitsEta", m_hitsEta).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    m_hitsPhi = new TProfile("hitsPhi", "Hits over Phi", hits.size(), -M_PI, M_PI);
    if (m_ths->regHist("hitsPhi", m_hitsPhi).isFailure()) {
        m_log << MSG::ERROR << "Coudn't register Histogram" << endmsg;
    }
    
    for (auto& hit : hits) {
        float eta = hit.second.Eta();
        float phi = hit.second.Phi();
        m_hitsEta->Fill(eta,hit.first);
        m_hitsPhi->Fill(phi,hit.first);
    }
    m_hitsEta->Rebin(10);
    m_hitsPhi->Rebin(10);
    
    m_hitsEta->Write();
    m_hitsPhi->Write();
    
    return StatusCode::SUCCESS;
}
