//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
#include "FCCFastSimGeometry.hh"
#include "FCCDetectorParametrisation.hh"
#include "G4ProductionCuts.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FCCFastSimGeometry::FCCFastSimGeometry(const G4GDMLAuxMapType* aAuxMap): fField(0)
{
   std::vector<G4Region*> TrackerList;
   std::vector<G4Region*> ECalList;
   std::vector<G4Region*> HCalList;
   std::vector<G4Region*> MuonList;

   // Retrieving Auxiliary Information

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

   // Attaching Fast Simulation models to the lists of G4Region

   for (G4int iterTracker=0; iterTracker<G4int(TrackerList.size()); iterTracker++)
   {
      TrackerList[iterTracker]->SetProductionCuts(new G4ProductionCuts());
      TrackerList[iterTracker]->GetProductionCuts()->SetProductionCut
         (1.* ((*TrackerList[iterTracker]->GetRootLogicalVolumeIterator())->GetMaterial()->GetRadlen()) );
      TrackerList[iterTracker]->GetProductionCuts()->SetProductionCut(
          1*m, idxG4GammaCut );
      fTrackerSmearModel.push_back( new FCCFastSimModelTracker("fastSimModelTracker",TrackerList[iterTracker], FCCDetectorParametrisation::eATLFAST));
   }
   for (G4int iterECal=0; iterECal<G4int(ECalList.size()); iterECal++)
   {
      ECalList[iterECal]->SetProductionCuts(new G4ProductionCuts());
      ECalList[iterECal]->GetProductionCuts()->SetProductionCut
         (0.5* ((*ECalList[iterECal]->GetRootLogicalVolumeIterator())->GetMaterial()->GetRadlen()) );
      ECalList[iterECal]->GetProductionCuts()->SetProductionCut(
         0.1*m, idxG4GammaCut );
      fEMCalSmearModel.push_back( new FCCFastSimModelEMCal("fastSimModelEMCal",ECalList[iterECal], FCCDetectorParametrisation::eATLFAST));
   }
   for (G4int iterHCal=0; iterHCal<G4int(HCalList.size()); iterHCal++)
   {
      HCalList[iterHCal]->SetProductionCuts(new G4ProductionCuts());
      HCalList[iterHCal]->GetProductionCuts()->SetProductionCut(
         0.5* ((*HCalList[iterHCal]->GetRootLogicalVolumeIterator())->GetMaterial()->GetRadlen()) );
      HCalList[iterHCal]->GetProductionCuts()->SetProductionCut(
         1.*m, idxG4GammaCut );

      fHCalSmearModel.push_back(new FCCFastSimModelHCal("fastSimModelHCal",HCalList[iterHCal], FCCDetectorParametrisation::eATLFAST));
   }

   // for (G4int iterMuon=0; iterMuon<G4int(MuonList.size()); iterMuon++)
   // {
   //    MuonList[iterMuon]->SetProductionCuts(new G4ProductionCuts());
   //    MuonList[iterMuon]->GetProductionCuts()->SetProductionCut(
   //       0.5* ((*MuonList[iterMuon]->GetRootLogicalVolumeIterator())->GetMaterial()->GetRadlen()) );
   //    MuonList[iterMuon]->GetProductionCuts()->SetProductionCut(
   //       1.*m, idxG4GammaCut );

   //    MuonSmearModel.push_back(
   //       new FCCMuonSmearModel("MuonKillModel",fMuonList[iterMuon]));
   // }


   // Electromagnetic field

   fField = new G4UniformMagField(G4ThreeVector(0.,0.,0.01));
   G4FieldManager* fieldMgr
      = G4TransportationManager::GetTransportationManager()
        ->GetFieldManager();
       fieldMgr->SetDetectorField(fField);
    fieldMgr->CreateChordFinder(fField);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


FCCFastSimGeometry::~FCCFastSimGeometry()
{
   delete fField;
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

