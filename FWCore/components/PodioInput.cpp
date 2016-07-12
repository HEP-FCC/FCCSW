#include "PodioInput.h"

#include "TFile.h"
#include "TROOT.h"

#include "FWCore/PodioDataSvc.h"
#include "FWCore/DataWrapper.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertex.h"

DECLARE_COMPONENT(PodioInput)

PodioInput::PodioInput(const std::string& name, ISvcLocator* svcLoc) :
GaudiAlgorithm(name, svcLoc), m_eventNum(0)
{
  declareProperty("filename", m_filename="output.root", "Name of the file to create");
  declareProperty("collections", m_collectionNames, "Places of collections to read.");
}

StatusCode PodioInput::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;

  // check whether we have the PodioEvtSvc active
  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(evtSvc().get());
  if (0 == m_podioDataSvc) return StatusCode::FAILURE;

  m_reader.openFile(m_filename);
  m_eventMax = m_reader.getEntries();
  auto idTable = m_reader.getCollectionIDTable();
  m_podioDataSvc->setCollectionIDs(idTable);
  m_provider.setReader(&m_reader);
  for (auto& name : m_collectionNames) {
    debug() << "Registering collection " << name << endmsg;
    if (!idTable->present(name)) {
      error() << "Requested product " << name << "not found in input file " << m_filename << endmsg;
      return StatusCode::FAILURE;
    }
    m_collectionIDs.push_back(idTable->collectionID(name));
  }
  return StatusCode::SUCCESS;
}

StatusCode PodioInput::execute() {
  size_t cntr = 0;
  for (auto& id : m_collectionIDs) {
    podio::CollectionBase* collection(nullptr);
    m_provider.get(id, collection);
    auto wrapper = new DataWrapper<podio::CollectionBase>;
    wrapper->setData(collection);
    const std::string& collName = m_collectionNames.at(cntr++);
    debug() << "Registering collection to read " << collName << " with id " << id << endmsg;
    m_podioDataSvc->registerObject(collName, wrapper);
  }
  m_provider.clearCaches();
  m_reader.endOfEvent();
  if(m_eventNum++ > m_eventMax) {
    info() << "Reached end of file with event " << m_eventMax << endmsg;
    IEventProcessor* eventProcessor;
    service("ApplicationMgr",eventProcessor);
    eventProcessor->stopRun();
  }
  return StatusCode::SUCCESS;
}

StatusCode PodioInput::finalize() {
  if (GaudiAlgorithm::finalize().isFailure())
    return StatusCode::FAILURE;
  m_reader.closeFile();
  return StatusCode::SUCCESS;
}
