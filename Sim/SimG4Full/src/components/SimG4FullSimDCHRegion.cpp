#include "SimG4FullSimDCHRegion.h"

// FCCSW
//#include "SimG4Fast/FastSimModelTracker.h"

// Geant4
#include "G4RegionStore.hh"
#include "G4TransportationManager.hh"

//#include "G4VFastSimulationModel.hh"

DECLARE_TOOL_FACTORY(SimG4FullSimDCHRegion)

SimG4FullSimDCHRegion::SimG4FullSimDCHRegion(const std::string& type, const std::string& name,
                                                     const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ISimG4RegionTool>(this);

}

SimG4FullSimDCHRegion::~SimG4FullSimDCHRegion() 
{
  delete fStepLimit;
}

StatusCode SimG4FullSimDCHRegion::initialize() {
  std::cout << "nalipourTest: SimG4FullSimDCHRegion ---> initialize" << std::endl;
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (m_volumeNames.size() == 0) {
    error() << "No detector name is specified for the parametrisation" << endmsg;
    return StatusCode::FAILURE;
  }

  fStepLimit = new G4UserLimits();
  fStepLimit->SetMaxAllowedStep(m_maxStepLength);

  return StatusCode::SUCCESS;
}

StatusCode SimG4FullSimDCHRegion::finalize() 
{
  std::cout << "nalipourTest: SimG4FullSimDCHRegion ---> finalize" << std::endl; 
  return GaudiTool::finalize(); 
}

StatusCode SimG4FullSimDCHRegion::create() {
  std::cout << "nalipourTest: SimG4FullSimDCHRegion ---> create" << std::endl;

  G4LogicalVolume* world =
      (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
  for (const auto& trackerName : m_volumeNames) {
    for (int iter_region = 0; iter_region < world->GetNoDaughters(); ++iter_region) {
      if (world->GetDaughter(iter_region)->GetName().find(trackerName) != std::string::npos) {

        /// all G4Region objects are deleted by the G4RegionStore
	
  	// std::cout << "NoDaughters=" << world->GetDaughter(iter_region)->GetLogicalVolume()->GetNoDaughters() << std::endl; // ok
        m_g4regions.emplace_back(
            new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName()+"_fullsim"));
        m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
  	std::cout << "m_maxStepLength=" << m_maxStepLength << std::endl;
  	//auto fStepLimit = new G4UserLimits();

  	m_g4regions.back()->SetUserLimits(fStepLimit);
        
        info() << "Attaching a Tracker fast simulation model to the region " << m_g4regions.back()->GetName() << endmsg;
	/*
  	int numDaughters=world->GetDaughter(iter_region)->GetLogicalVolume()->GetNoDaughters();
  	for (auto i = 0; i<numDaughters; ++i)
  	  {
  	    auto daug=world->GetDaughter(iter_region)->GetLogicalVolume()->GetDaughter(i)->GetLogicalVolume()->IsRegion();
  	    auto namedaugRegion = world->GetDaughter(iter_region)->GetLogicalVolume()->GetDaughter(i)->GetLogicalVolume()->GetRegion()->GetName();
  	    auto daughterName = world->GetDaughter(iter_region)->GetLogicalVolume()->GetDaughter(i)->GetName();
  	    auto d_region= world->GetDaughter(iter_region)->GetLogicalVolume()->GetDaughter(i)->GetLogicalVolume()->GetRegion();
  	    auto d_cut = d_region->GetUserLimits();
  	    std::cout << i << "------------- daug=" << daug << ", regionName=" << namedaugRegion << ", daughterName=" << daughterName<< std::endl;
  	    std::cout << "cut=" << d_cut << std::endl;
  	  }
	*/

      }
    }
  }

  /*
  for (const auto& reg : m_g4regions) 
    {
      std::cout << "nalipourTest: region name =" << reg->GetName() << ", rootVolumes=" <<  reg->GetNumberOfRootVolumes() << std::endl;

      auto limit = reg->GetUserLimits();
      std::cout << "limit type: " << limit->GetType() << std::endl;
      auto it = reg->GetRootLogicalVolumeIterator();
      
      for (auto i=it->begin(); i != it->end(); ++i)
	{
	  std::cout << "------name=" << i->GetName << std::endl;
	}
    }
  */


  return StatusCode::SUCCESS;
}
