#ifndef TEST_GEOTOGDMLDUMPSVC_H
#define TEST_GEOTOGDMLDUMPSVC_H

#include "TestDD4hep/ITestDD4hepSvc.h"

// Gaudi
#include "GaudiKernel/Service.h"

class InspectGeoSvc: public extends1<Service, ITestDD4hepSvc> {
public:
   /// Constructor.
   explicit InspectGeoSvc(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Finalize.
   virtual StatusCode finalize();
private:
   /// Name of the GDML output file
   std::string m_gdmlFileName;
};

#endif
