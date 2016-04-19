#ifndef TEST_GEOTOGDMLDUMPSVC_H
#define TEST_GEOTOGDMLDUMPSVC_H

#include "TestDD4hep/ITestDD4hepSvc.h"

// Gaudi
#include "GaudiKernel/Service.h"

/** @class GeoToGdmlDumpSvc TestDD4hep/TestDD4hep/GeoToGdmlDumpSvc.h GeoToGdmlDumpSvc.h
 *
 *  Service that dumps the geometry retrieved from the DD4hep GeoSvc to GDML file.
 *
 *  @author Anna Zaborowska
 */

class GeoToGdmlDumpSvc: public extends1<Service, ITestDD4hepSvc> {
public:
   /// Constructor.
   explicit GeoToGdmlDumpSvc(const std::string& name, ISvcLocator* svcLoc);
   /// Initialize.
   virtual StatusCode initialize();
   /// Finalize.
   virtual StatusCode finalize();

private:
   /// Name of the GDML output file
   std::string m_gdmlFileName;
};

#endif
