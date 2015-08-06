#include "FCCDetectorConstruction.hh"

#include "G4ProductionCuts.hh"
#include "G4SystemOfUnits.hh"
#include "G4RegionStore.hh"
#include "G4GDMLParser.hh"

FCCDetectorConstruction::FCCDetectorConstruction()
{;}

FCCDetectorConstruction::~FCCDetectorConstruction()
{
   for (G4int iterTracker=0; iterTracker<G4int(fTrackerSmearModel.size()); iterTracker++)
   {
      delete fTrackerSmearModel[iterTracker];
   }
   for (G4int iterEMCal=0; iterEMCal<G4int(fEMCalSmearModel.size()); iterEMCal++)
   {
      delete fEMCalSmearModel[iterEMCal];
   }
   for (G4int iterHCal=0; iterHCal<G4int(fHCalSmearModel.size()); iterHCal++)
   {

      delete fHCalSmearModel[iterHCal];
   }
}

G4VPhysicalVolume* FCCDetectorConstruction::Construct()
{
   G4GDMLParser parser;
   parser.Read("FCCFullDetector.gdml");
   G4cout << "Geometry loaded from  file .......FCCFullDetector.gdml " <<G4endl;

   std::vector<G4Region*> TrackerList;
   std::vector<G4Region*> ECalList;
   std::vector<G4Region*> HCalList;
   std::vector<G4Region*> MuonList;

   const G4GDMLAuxMapType* aAuxMap = parser.GetAuxMap();
   for(G4GDMLAuxMapType::const_iterator iter=aAuxMap->begin();
       iter!=aAuxMap->end(); iter++)
   {
      for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin();
           vit!=(*iter).second.end();vit++)
      {
         if ((*vit).type=="FastSimModel")
         {
            G4LogicalVolume* myvol = (*iter).first;
            if ((myvol->GetName()).find("Tracker") != std::string::npos){
               TrackerList.push_back(new G4Region(myvol->GetName()));
               TrackerList.back()->AddRootLogicalVolume(myvol);
            }
            else if ((myvol->GetName()).find("HCal") != std::string::npos){
               HCalList.push_back(new G4Region(myvol->GetName()));
               HCalList.back()->AddRootLogicalVolume(myvol);
            }
            else if ((myvol->GetName()).find("ECal") != std::string::npos){
               ECalList.push_back(new G4Region(myvol->GetName()));
               ECalList.back()->AddRootLogicalVolume(myvol);
            }
            else if ((myvol->GetName()).find("Muon") != std::string::npos){
               MuonList.push_back(new G4Region(myvol->GetName()));
               MuonList.back()->AddRootLogicalVolume(myvol);
            }
            else {
               G4cout << G4endl << "NOT A KNOWN DETECTOR !!!" << G4endl;
            }
         }
      }
   }
   for (G4int iterTracker=0; iterTracker<G4int(TrackerList.size()); iterTracker++)
   {
      TrackerList[iterTracker]->SetProductionCuts(new G4ProductionCuts());
      TrackerList[iterTracker]->GetProductionCuts()->SetProductionCut
         (1.* ((*TrackerList[iterTracker]->GetRootLogicalVolumeIterator())->GetMaterial()->GetRadlen()) );
      TrackerList[iterTracker]->GetProductionCuts()->SetProductionCut(
          1*m, idxG4GammaCut );
      fTrackerSmearModel.push_back( new FCCFastSimModelTracker("fastSimModelTracker",TrackerList[iterTracker], FCCDetectorParametrisation::eCMS) );
   }
   for (G4int iterECal=0; iterECal<G4int(ECalList.size()); iterECal++)
   {
      ECalList[iterECal]->SetProductionCuts(new G4ProductionCuts());
      ECalList[iterECal]->GetProductionCuts()->SetProductionCut
         (0.5* ((*ECalList[iterECal]->GetRootLogicalVolumeIterator())->GetMaterial()->GetRadlen()) );
      ECalList[iterECal]->GetProductionCuts()->SetProductionCut(
         0.1*m, idxG4GammaCut );
      fEMCalSmearModel.push_back( new FCCFastSimModelEMCal("fastSimModelEMCal",ECalList[iterECal], FCCDetectorParametrisation::eCMS) );
   }
   for (G4int iterHCal=0; iterHCal<G4int(HCalList.size()); iterHCal++)
   {
      HCalList[iterHCal]->SetProductionCuts(new G4ProductionCuts());
      HCalList[iterHCal]->GetProductionCuts()->SetProductionCut(
         0.5* ((*HCalList[iterHCal]->GetRootLogicalVolumeIterator())->GetMaterial()->GetRadlen()) );
      HCalList[iterHCal]->GetProductionCuts()->SetProductionCut(
         1.*m, idxG4GammaCut );
      fHCalSmearModel.push_back( new FCCFastSimModelHCal("fastSimModelHCal",HCalList[iterHCal], FCCDetectorParametrisation::eCMS) );
   }
   //------------------
   // Returns the pointer to
   // the physical world:
   //-----------------------
   return parser.GetWorldVolume();
}
