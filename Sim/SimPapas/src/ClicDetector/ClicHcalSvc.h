#ifndef CLICHCALSVC_H
#define CLICHCALSVC_H

#include "SimPapas/IPapasCalorimeterSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

/** @class DetetctorSvc DetetctorSvc.h DetetctorSvc/DetetctorSvc.h
 *
 *  This service provides the Papas Clic HCAL
 *  @author alice.robson@cern.ch
 */

namespace papas {
class Calorimeter;
}

class ClicHcalSvc : public extends1<Service, IPapasCalorimeterSvc> {

public:
  /// Default constructor
  ClicHcalSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~ClicHcalSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to hcal
  virtual std::shared_ptr<papas::Calorimeter> calorimeter() const;

private:
  // HCAL
  std::shared_ptr<papas::Calorimeter> m_hcal;
  Gaudi::Property<double> m_innerRadius{this, "innerRadius", 2.4, "Hcal inner radius"};
  Gaudi::Property<double> m_innerZ{this, "innerZ", 2.85, "Hcal inner z"};
  Gaudi::Property<double> m_outerRadius{this, "outerRadius", 4.8, "Hcal outer radius"};
  Gaudi::Property<double> m_outerZ{this, "outerZ", 5.3, "Hcal outer Z"};
  Gaudi::Property<double> m_clusterSize{this, "clusterSize", 0.25, "Hcal cluster size"};
  Gaudi::Property<std::vector<double>> m_eresBarrel{
      this, "eresBarrel", {0.6, 0., 0.025}, "Hcal energy response parameters barrel"};
  Gaudi::Property<double> m_X0{this, "x0", 0.018, "Hcal material x0"};
  Gaudi::Property<double> m_lambdaI{this, "lambdaI", 0.17, "Hcal material lambdaI"};
  Gaudi::Property<double> m_eResponse{this, "eResponse", 1., "Hcal eresponse"};
  Gaudi::Property<double> m_etaAcceptance{this, "etaAcceptance", 2.76, "Hcal eta acceptance"};
};

inline std::shared_ptr<papas::Calorimeter> ClicHcalSvc::calorimeter() const { return m_hcal; }

#endif  // CLICHCALSVC_H
