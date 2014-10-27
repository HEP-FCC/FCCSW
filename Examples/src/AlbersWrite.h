#ifndef _ALBERSWRITE_H_
#define _ALBERSWRITE_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "DataObjects/JetCollection.h"


class AlbersWrite: public GaudiAlgorithm {
  friend class AlgFactory<AlbersWrite> ;

public:
  /// Constructor.
  AlbersWrite(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the jets to be written
  DataObjectHandle<JetCollection> m_jethandle;

};

#endif
