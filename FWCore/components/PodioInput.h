#ifndef FWCORE_PODIOINPUT_H
#define FWCORE_PODIOINPUT_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"
#include "FWCore/KeepDropSwitch.h"
#include "FWCore/DataWrapper.h"

#include "TTree.h"

#include <vector>
#include <string>

// forward declarations
class TFile;
class PodioDataSvc;


/** @class PodioInput FWCore/components/PodioInput.h PodioInput.h
 *
 *  Class that allows to read ROOT files written with PodioOutput
 *  TODO: Work with TChains, automatically load needed libraries (needs changes to scheduler), automatically
 *  detect collections that are required due to PODIO-relations, check nEvents and stop run if EOF is reached
 *
 *  @author J. Lingemann
 */

class PodioInput : public GaudiAlgorithm {
  friend class AlgFactory<PodioInput> ;

public:
  /// Constructor.
  PodioInput(const std::string& name, ISvcLocator* svcLoc);
  /// Initialization of PodioInput. Acquires the data service, opens root file and creates trees.
  virtual StatusCode initialize();
  /// Execute. Re-creates collections that are specified to be read and sets references.
  virtual StatusCode execute();
  /// Finalize. Closes ROOT file.
  virtual StatusCode finalize();

private:
  /// Does the actual work of getting a collection from the file. Collection type is derived from ROOT dict.
  StatusCode getCollection(const std::string& name);
  /// Root file name the input is read from. Set by option filename
  std::string m_filename;
  /// Cache of collection types that are derived from ROOT dictionaries
  std::map<std::string, std::pair<TClass*, TClass*>> m_collectionTypes;
  /// Name of collections to read. Set by option collections
  std::vector<std::string> m_collectionNames;
  /// Data service: needed to register objects and get collection IDs. Just an observing pointer.
  PodioDataSvc* m_podioDataSvc;
  /// The actual ROOT file
  std::unique_ptr<TFile> m_file;
  /// Tree that is filled with collections
  TTree* m_datatree;
  /// Tree that contains the CollectionIDTable (this is used to initialize the ID table of the data svc.)
  TTree* m_metadatatree;
  /// Counter of the event number
  int m_eventNum;
};

#endif
