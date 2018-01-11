#ifndef TESTRECONSTRUCTION_TESTACTSLOGGER_H
#define TESTRECONSTRUCTION_TESTACTSLOGGER_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"

// FCCSW
#include "FWCore/DataHandle.h"




class TestACTSLogger : public GaudiAlgorithm {
public:
  explicit TestACTSLogger(const std::string&, ISvcLocator*);
  virtual ~TestACTSLogger();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Execute.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

};
#endif /* TESTRECONSTRUCTION_TESTACTSLOGGER_H */
