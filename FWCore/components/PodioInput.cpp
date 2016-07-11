#include "PodioInput.h"
#include "TFile.h"
#include "TROOT.h"
#include "FWCore/PodioDataSvc.h"

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

  // TODO: Move this to TChain! Check maxEvt < numEntries
  m_file = std::unique_ptr<TFile>(TFile::Open(m_filename.c_str(), "READ"));
  if (m_file->IsZombie()) {
    return Error("Cannot open file");
  }

  m_datatree = dynamic_cast<TTree*>(m_file->Get("events"));
  m_metadatatree = dynamic_cast<TTree*>(m_file->Get("metadata"));
  auto collectionIDs = new podio::CollectionIDTable();
  m_metadatatree->SetBranchAddress("CollectionIDs", &collectionIDs);
  m_metadatatree->GetEntry(0);
  m_podioDataSvc->setCollectionIDs(collectionIDs);
  return StatusCode::SUCCESS;
}

StatusCode PodioInput::getCollection(const std::string& name) {
  // because of the store clears after each event, we have to re-create on each event
  auto branch = m_datatree->GetBranch(name.c_str());
    if (branch == nullptr) {
      error() << "Cannot find collection " << name << " in input file " << m_filename << endmsg;
      return StatusCode::FAILURE;
    }
    TClass* dataClass(nullptr);
    TClass* collectionClass(nullptr);

    auto result = m_collectionTypes.find(name);
    if (result != m_collectionTypes.end()) {
      dataClass = result->second.first;
      collectionClass = result->second.second;
    } else {
      auto dataClassName = branch->GetClassName();
      dataClass = gROOT->GetClass(dataClassName);
      if (dataClass == nullptr) {
        error() << "Data class " << dataClassName << " missing from ROOT dictionaries..." << endmsg;
        return StatusCode::FAILURE;
      }
      // now create the transient collections
      // some workaround until gcc supports regex properly:

      auto dataClassString = std::string(dataClassName);
      auto start = dataClassString.find("<");
      auto end   = dataClassString.find(">");
      //getting "TypeCollection" out of "vector<TypeData>"
      auto classname = dataClassString.substr(start+1, end-start-5);
      auto collectionClassName = classname+"Collection";
      debug() << "Registering collection: " << collectionClassName << endmsg;
      collectionClass = gROOT->GetClass(collectionClassName.c_str());
      if (collectionClass == nullptr) return StatusCode::FAILURE;
      // cache classes found for future usage
      m_collectionTypes[name] = std::pair<TClass*,TClass*>(dataClass, collectionClass);
    }
    // now create buffers and collections (ownership is transferred to event store when registering)
    void* buffer = dataClass->New();
    podio::CollectionBase* collection = nullptr;
    collection = static_cast<podio::CollectionBase*>(collectionClass->New());
    // connect buffer, collection and branch
    collection->setBuffer(buffer);
    branch->SetAddress(collection->getBufferAddress());
    branch->GetEntry(m_eventNum);
    // load the collections containing references
    // TODO: Check if reference collection is already in m_collectionNames, otherwise add it
    auto refCollections = collection->referenceCollections();
    if (refCollections != nullptr) {
      info() << "adding refCollections: " << refCollections->size() << endmsg;
      for (int i = 0, end = refCollections->size(); i!=end; ++i){
        auto refBranch = m_datatree->GetBranch((name+"#"+std::to_string(i)).c_str());
        refBranch->SetAddress(&(*refCollections)[i]);
        refBranch->GetEntry(m_eventNum);
      }
    }
    auto wrapper = new DataWrapper<podio::CollectionBase>;
    wrapper->setData(collection);
    // this also initialises the IDs correctly:
    m_podioDataSvc->registerReadObject(name, wrapper);
    collection->prepareAfterRead();
    return SUCCESS;
}

StatusCode PodioInput::execute() {
  for (auto& name : m_collectionNames) {
    if (getCollection(name).isFailure()) {
      return StatusCode::FAILURE;
    }
  }
  m_podioDataSvc->setReferences();
  m_eventNum++;
  return StatusCode::SUCCESS;
}

StatusCode PodioInput::finalize() {
  if (GaudiAlgorithm::finalize().isFailure())
    return StatusCode::FAILURE;

  m_file->Close();
  return StatusCode::SUCCESS;
}
