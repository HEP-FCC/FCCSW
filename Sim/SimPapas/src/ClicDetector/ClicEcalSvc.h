#ifndef CLICECALSVC_H
#define CLICECALSVC_H

#include "SimPapas/IPapasCalorimeterSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

/** @class CliceEcalSvc
 *
 *  This service provides the Papas ECAL
 *  @author alice.robson@cern.ch
 */

namespace papas {
class Calorimeter;
}

class ClicEcalSvc : public extends1<Service, IPapasCalorimeterSvc> {

public:
  /// Default constructor
  ClicEcalSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~ClicEcalSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to Ecal
  virtual std::shared_ptr<papas::Calorimeter> calorimeter() const;

private:
  ///< ECAL
  std::shared_ptr<papas::Calorimeter> m_ecal;
  Gaudi::Property<double> m_innerRadius{this, "innerRadius", 2.15, "Ecal inner radius"};
  Gaudi::Property<double> m_innerZ{this, "innerZ", 2.6, "Ecal inner z"};
  Gaudi::Property<double> m_depth{this, "depth", .25, "Ecal depth"};
  Gaudi::Property<double> m_clusterSizePhoton{this, "clusterSizePhoton", .015, "Ecal cluster size Photon"};
  Gaudi::Property<double> m_clusterSize{this, "clusterSize", 0.045, "Ecal cluster size"};
  Gaudi::Property<double> m_etaAcceptance{this, "etaAcceptance", 2.76, "Ecal eta acceptance"};
  Gaudi::Property<std::vector<double>> m_emin{this, "emin", {0.5, 0.5}, "Ecal barrel and endcap emin"};
  Gaudi::Property<std::vector<double>> m_eresBarrel{
      this, "eresBarrel", {0.167, 0.010, 0.011}, "Ecal energy response parameters barrel"};
  Gaudi::Property<int> m_nX0{this, "x0", 23, "Ecal material number x0"};
  Gaudi::Property<int> m_nLambdaI{this, "nLambdaI", 1, "Ecal material number LambdaI"};
  Gaudi::Property<double> m_eResponse{this, "eresponse", 1., "Ecal eresponse"};
};

inline std::shared_ptr<papas::Calorimeter> ClicEcalSvc::calorimeter() const { return m_ecal; }

#endif  // CLICECALSVC_H
