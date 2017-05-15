#ifndef FWCORE_PODIOOUTPUT_H
#define FWCORE_PODIOOUTPUT_H

#include "FWCore/KeepDropSwitch.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "podio/CollectionBase.h"

#include "TTree.h"

#include <vector>

// forward declarations
class TFile;
class PodioDataSvc;

class PodioOutput : public GaudiAlgorithm {
  friend class AlgFactory<PodioOutput>;

public:
  /// Constructor.
  PodioOutput(const std::string& name, ISvcLocator* svcLoc);

  /// Initialization of PodioOutput. Acquires the data service, creates trees and root file.
  virtual StatusCode initialize();
  /// Execute. For the first event creates branches for all collections known to PodioDataSvc and prepares them for
  /// writing. For the following events it reconnects the branches with collections and prepares them for write.
  virtual StatusCode execute();
  /// Finalize. Writes the meta data tree; writes file and cleans up all ROOT-pointers.
  virtual StatusCode finalize();

private:
  void resetBranches(const std::vector<std::pair<std::string, podio::CollectionBase*>>& collections, bool prepare);
  void createBranches(const std::vector<std::pair<std::string, podio::CollectionBase*>>& collections, bool prepare);
  /// First event or not
  bool m_firstEvent;
  /// Root file name the output is written to
  Gaudi::Property<std::string> m_filename{this, "filename", "output.root", "Name of the file to create"};
  /// Commands which output is to be kept
  Gaudi::Property<std::vector<std::string>> m_outputCommands{
      this, "outputCommands", {"keep *"}, "A set of commands to declare which collections to keep or drop."};
  /// Switch for keeping or dropping outputs
  KeepDropSwitch m_switch;
  /// Needed for collection ID table
  PodioDataSvc* m_podioDataSvc;
  /// The actual ROOT file
  std::unique_ptr<TFile> m_file;
  /// The tree to be filled with collections
  TTree* m_datatree;
  /// The tree to be filled with meta data
  TTree* m_metadatatree;
  /// The stored collections
  std::vector<podio::CollectionBase*> m_storedCollections;
};

#endif
