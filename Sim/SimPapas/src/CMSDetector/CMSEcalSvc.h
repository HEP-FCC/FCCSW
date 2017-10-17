#ifndef CMSECALSVC_H
#define CMSECALSVC_H

#include "SimPapas/IPapasCalorimeterSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

/** @class CMSEcalSvc.h CMSEcalSvc.h
 *
 *  This service provides the CMS ECAL service
 *  @author alice.robson@cern.ch
 */

namespace papas {
class Calorimeter;
}

class CMSEcalSvc : public extends1<Service, IPapasCalorimeterSvc> {

public:
  /// Default constructor
  CMSEcalSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~CMSEcalSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to ecal
  virtual std::shared_ptr<papas::Calorimeter> calorimeter() const;

private:
  // ECAL
  std::shared_ptr<papas::Calorimeter> m_ecal;
  Gaudi::Property<double> m_innerRadius{this, "innerRadius", 1.3, "Ecal inner radius"};
  Gaudi::Property<double> m_innerZ{this, "innerZ", 2, "Ecal inner z"};
  Gaudi::Property<double> m_outerRadius{this, "depth", 1.55, "Ecal  outer radius"};
  Gaudi::Property<double> m_outerZ{this, "outerZ", 2.1, "Ecal outer z"};
  Gaudi::Property<double> m_x0{this, "x0", 8.9e-3, "Ecal material x0"};
  Gaudi::Property<double> m_lambdaI{this, "lambdaI", 0.275, "Ecal material lambdaI"};
  Gaudi::Property<double> m_clusterSizePhoton{this, "clusterSizePhoton", .04, "Ecal cluster size Photon"};
  Gaudi::Property<double> m_clusterSize{this, "clusterSize", 0.07, "Ecal cluster size"};
  Gaudi::Property<double> m_etaCrack{this, "etaCrack", 1.479, "Ecal eta crack"};
  Gaudi::Property<double> m_etaAcceptance{this, "etaAcceptance", 2.93, "Ecal eta acceptance"};
  Gaudi::Property<double> m_ptAcceptance{this, "ptAcceptance", 0.2, "Ecal pt acceptance"};
  Gaudi::Property<double> m_etaEndcapMin{this, "etaEndcapMin", 1.479, "Ecal eta endcap min"};
  Gaudi::Property<double> m_etaEndcapMax{this, "etaEndcapMax", 3., "Ecal eta endcap max"};
  Gaudi::Property<std::vector<double>> m_emin{this, "emin", {0.3, 1}, "Ecal barrel and endcap emin"};
  Gaudi::Property<std::vector<std::vector<double>>> m_eres{
      this,
      "eres",
      {{4.22163e-02, 1.55903e-01, 7.14166e-03}, {-2.08048e-01, 3.25097e-01, 7.34244e-03}},
      "Ecal energy resolution parameters"};
  Gaudi::Property<std::vector<std::vector<double>>> m_eresp{
      this,
      "eresp",
      {{1.00071, -9.04973, -2.48554}, {9.95665e-01, -3.31774, -2.11123}},
      "Ecal energy response parameters"};
};

inline std::shared_ptr<papas::Calorimeter> CMSEcalSvc::calorimeter() const { return m_ecal; }

#endif  // CMSECALSVC_H
