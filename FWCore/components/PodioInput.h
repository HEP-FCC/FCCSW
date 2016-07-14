#ifndef FWCORE_PODIOINPUT_H
#define FWCORE_PODIOINPUT_H
// Gaaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IEventProcessor.h"
// PODIO
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
// STL
#include <vector>
#include <string>

// forward declarations
// from FWCore:
class PodioDataSvc;


/** @class PodioInput FWCore/components/PodioInput.h PodioInput.h
 *
 *  Class that allows to read ROOT files written with PodioOutput
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
  /// PODIO reader for ROOT files
  podio::ROOTReader m_reader;
  /// PODIO EventStore, used to initialise collections
  podio::EventStore m_provider;
  /// ROOT file name the input is read from. Set by option filename
  std::string m_filename;
  /// Name of collections to read. Set by option collections (this is temporary)
  std::vector<std::string> m_collectionNames;
  /// Collection IDs (retrieved with CollectionIDTable from ROOT file, using collection names)
  std::vector<int> m_collectionIDs;
  /// Data service: needed to register objects and get collection IDs. Just an observing pointer.
  PodioDataSvc* m_podioDataSvc;
  /// Counter of the event number
  int m_eventNum;
  /// Number of events in the file / to process
  int m_eventMax;
};

#endif
