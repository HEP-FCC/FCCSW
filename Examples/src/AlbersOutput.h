#ifndef _ALBERSOUTPUT_H_
#define _ALBERSOUTPUT_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "podio/CollectionBase.h"
#include "KeepDropSwitch.h"

#include "TTree.h"

#include <vector>

// forward declarations
class TFile;
class AlbersDataSvc;

class AlbersOutput : public GaudiAlgorithm {
  friend class AlgFactory<AlbersOutput> ;

public:
  /// Constructor.
  AlbersOutput(const std::string& name, ISvcLocator* svcLoc);
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
  AlbersDataSvc* m_albersDataSvc;
  TFile* m_file;
  TTree* m_datatree;
  TTree* m_metadatatree;
  std::vector<podio::CollectionBase*> m_storedCollections;

};

#endif
