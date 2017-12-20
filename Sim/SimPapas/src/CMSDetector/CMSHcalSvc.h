#ifndef CMSHCALSVC_H
#define CMSHCALSVC_H

#include "SimPapas/IPapasCalorimeterSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

/** @class CMSHcalSvc
 *
 *  This service provides the Papas CMS HCAL
 *  @author alice.robson@cern.ch
 */

namespace papas {
class Calorimeter;
}

class CMSHcalSvc : public extends1<Service, IPapasCalorimeterSvc> {

public:
  /// Default constructor
  CMSHcalSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~CMSHcalSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to papas HCal
  virtual std::shared_ptr<papas::Calorimeter> calorimeter() const;

private:
  // HCAL
  std::shared_ptr<papas::Calorimeter> m_hcal;
  Gaudi::Property<double> m_innerRadius{this, "innerRadius", 1.9, "Hcal inner radius"};
  Gaudi::Property<double> m_innerZ{this, "innerZ", 2.6, "Hcal inner z"};
  Gaudi::Property<double> m_outerRadius{this, "outerRadius", 2.9, "Hcal outer radius"};
  Gaudi::Property<double> m_outerZ{this, "outerZ", 3.6, "Hcal outer Z"};
  Gaudi::Property<double> m_clusterSize{this, "clusterSize", 0.2, "Hcal cluster size"};
  Gaudi::Property<double> m_x0{this, "x0", 0.0, "Hcal material x0"};
  Gaudi::Property<double> m_lambdaI{this, "lambdaI", 0.17, "Hcal material lambdaI"};
  Gaudi::Property<double> m_etaCrack{this, "etaCrack", 1.3, "Hcal etaCrack"};
  Gaudi::Property<std::vector<std::vector<double>>> m_eres{
      this, "eres", {{0.8062, 2.753, 0.1501}, {6.803e-06, 6.676, 0.1716}}, "Hcal energy resolution parameters "};
  Gaudi::Property<std::vector<std::vector<double>>> m_eresp{
      this, "eresp", {{1.036, 4.452, -2.458}, {1.071, 9.471, -2.823}}, "Hcal energy response parameters "};
  Gaudi::Property<std::vector<double>> m_acceptanceParameters{this,
                                                              "acceptanceParameters",
                                                              {1., 1., -1.9381, -1.75330, 3., 1.1, 10., 1.05634,
                                                               -1.66943e-01, 1.05997e-02, 8.09522e-01, -9.90855,
                                                               -5.30366, 5., 7.},
                                                              "Hcal  acceptance parameters vector"};
};

inline std::shared_ptr<papas::Calorimeter> CMSHcalSvc::calorimeter() const { return m_hcal; }

#endif  // CMSHCALSVC_H
