#ifndef _PODIOOUTPUT_H_
#define _PODIOOUTPUT_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "podio/CollectionBase.h"
#include "FWCore/KeepDropSwitch.h"

#include "TTree.h"

#include <vector>

// forward declarations
class TFile;
class PodioDataSvc;

class PodioOutput : public GaudiAlgorithm {
  friend class AlgFactory<PodioOutput> ;

public:
  /// Constructor.
  PodioOutput(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  bool m_first;
  std::string m_filename;
  std::vector<std::string> m_outputCommands;
  KeepDropSwitch m_switch;
  PodioDataSvc* m_podioDataSvc;
  TFile* m_file;
  TTree* m_datatree;
  TTree* m_metadatatree;
  std::vector<podio::CollectionBase*> m_storedCollections;

};

#endif
