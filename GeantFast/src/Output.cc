#include "Output.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "g4root.hh"

Output* Output::fOutput = nullptr;

Output::Output(): fFileNameWithRunNo(false), fCurrentID()
{
   fFileName = "DefaultOutput.root";
}

Output::~Output()
{
   delete G4AnalysisManager::Instance();
}

Output* Output::Instance()
{
   if(!fOutput)
   {
      fOutput = new Output();
   }
   return fOutput;
}

void Output::SetFileName(G4String aName)
{
   fFileName = aName;
}

void Output::AppendName(G4bool aApp)
{
   fFileNameWithRunNo = aApp;
}

G4String Output::GetFileName()
{
   return fFileName;
}

void Output::StartAnalysis(G4int aRunID)
{
   G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
   if ( fFileNameWithRunNo)
   {
      fFileName +=  "_run";
      fFileName += G4UIcommand::ConvertToString(aRunID);
   }
   analysisManager->SetVerboseLevel(1);
   analysisManager->SetFileName(fFileName);
   analysisManager->OpenFile(fFileName);
}

void Output::EndAnalysis()
{
   G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
   analysisManager->Write();
   analysisManager->CloseFile();
}

void Output::CreateNtuples()
{
   const G4Event* event = G4RunManager::GetRunManager()->GetCurrentEvent();
   G4String evName = "Event_";
   evName += G4UIcommand::ConvertToString(event->GetEventID());
   G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
   G4int ntupID = event->GetEventID();

   analysisManager->CreateNtuple(evName, evName);
   analysisManager->CreateNtupleIColumn("particleID");  // column Id = 0
   analysisManager->CreateNtupleIColumn("PID");  // column Id = 1
   analysisManager->CreateNtupleDColumn("MC_pX");  // column Id = 2
   analysisManager->CreateNtupleDColumn("MC_pY"); // column Id = 3
   analysisManager->CreateNtupleDColumn("MC_pZ"); // column Id = 4

   analysisManager->CreateNtupleDColumn("tracker_res"); // column Id = 5
   analysisManager->CreateNtupleDColumn("tracker_eff"); // column Id = 6
   analysisManager->CreateNtupleDColumn("tracker_pX");  // column Id = 7
   analysisManager->CreateNtupleDColumn("tracker_pY"); // column Id = 8
   analysisManager->CreateNtupleDColumn("tracker_pZ"); // column Id = 9

   analysisManager->CreateNtupleDColumn("emcal_res"); // column Id = 10
   analysisManager->CreateNtupleDColumn("emcal_eff");  // column Id = 11
   analysisManager->CreateNtupleDColumn("emcal_X");  // column Id = 12
   analysisManager->CreateNtupleDColumn("emcal_Y"); // column Id = 13
   analysisManager->CreateNtupleDColumn("emcal_Z"); // column Id = 14
   analysisManager->CreateNtupleDColumn("emcal_E"); // column Id = 15

   analysisManager->CreateNtupleDColumn("hcal_res"); // column Id = 16
   analysisManager->CreateNtupleDColumn("hcal_eff"); // column Id = 17
   analysisManager->CreateNtupleDColumn("hcal_X");  // column Id = 18
   analysisManager->CreateNtupleDColumn("hcal_Y"); // column Id = 19
   analysisManager->CreateNtupleDColumn("hcal_Z"); // column Id = 20
   analysisManager->CreateNtupleDColumn("hcal_E");  // column Id = 21
   analysisManager->FinishNtuple(ntupID);
}

void Output::CreateHistograms()
{
   G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
   analysisManager->CreateH1("Pdiff","momentum smeared in tracker", 100, 0.8,1.2);
   analysisManager->SetH1XAxisTitle(0, "p_{smeared}/p_{true}");
   analysisManager->SetH1YAxisTitle(0, "Entries");
   analysisManager->CreateH1("EMCalEdiff","energy smeared in EMCal", 100, 0.8, 1.2);
   analysisManager->SetH1XAxisTitle(1, "E_{smeared}/E_{true}");
   analysisManager->SetH1YAxisTitle(1, "Entries");
   analysisManager->CreateH1("HCalEdiff","energy smeared in HCal", 100, 0., 2);
   analysisManager->SetH1XAxisTitle(2, "E_{smeared}/E_{true}");
   analysisManager->SetH1YAxisTitle(2, "Entries");
}

void Output::SaveTrack(SaveType aWhatToSave, G4int aPartID,  G4int aPDG,
                          G4ThreeVector aVector, G4double aResolution, G4double aEfficiency, G4double aEnergy)
{
   const G4Event* event = G4RunManager::GetRunManager()->GetCurrentEvent();
   G4int evNo = event->GetEventID();
   G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
   switch(aWhatToSave)
   {
   case Output::eNoSave:
      break;
   case  Output::eSaveMC:
   {
      analysisManager->FillNtupleIColumn(evNo, 0, aPartID);
      analysisManager->FillNtupleIColumn(evNo, 1, aPDG);
      analysisManager->FillNtupleDColumn(evNo, 2, aVector.x());
      analysisManager->FillNtupleDColumn(evNo, 3, aVector.y());
      analysisManager->FillNtupleDColumn(evNo, 4, aVector.z());
      fCurrentID = aPartID;
      break;
   }
   case  Output::eSaveTracker:
   {
      if (aPartID != fCurrentID)
         G4cout<<" Wrong particle - trying to save Tracker information of different particle"<<G4endl;
      analysisManager->FillNtupleDColumn(evNo, 5, aResolution);
      analysisManager->FillNtupleDColumn(evNo, 6, aEfficiency);
      analysisManager->FillNtupleDColumn(evNo, 7, aVector.x());
      analysisManager->FillNtupleDColumn(evNo, 8, aVector.y());
      analysisManager->FillNtupleDColumn(evNo, 9, aVector.z());
      break;
   }
   case  Output::eSaveEMCal:
   {
      if (aPartID != fCurrentID)
         G4cout<<" Wrong particle - trying to save EMCal information of different particle"<<G4endl;
      analysisManager->FillNtupleDColumn(evNo, 10, aResolution);
      analysisManager->FillNtupleDColumn(evNo, 11, aEfficiency);
      analysisManager->FillNtupleDColumn(evNo, 12, aVector.x());
      analysisManager->FillNtupleDColumn(evNo, 13, aVector.y());
      analysisManager->FillNtupleDColumn(evNo, 14, aVector.z());
      analysisManager->FillNtupleDColumn(evNo, 15, aEnergy);
      break;
   }
   case  Output::eSaveHCal:
   {
      if (aPartID != fCurrentID)
         G4cout<<" Wrong particle - trying to save HCal information of different particle"<<G4endl;
      analysisManager->FillNtupleDColumn(evNo, 16, aResolution);
      analysisManager->FillNtupleDColumn(evNo, 17, aEfficiency);
      analysisManager->FillNtupleDColumn(evNo, 18, aVector.x());
      analysisManager->FillNtupleDColumn(evNo, 19, aVector.y());
      analysisManager->FillNtupleDColumn(evNo, 20, aVector.z());
      analysisManager->FillNtupleDColumn(evNo, 21, aEnergy);
      analysisManager->AddNtupleRow(evNo);
      break;
   }
   }
   return;
}

void Output::FillHistogram(G4int aHistNo, G4double aValue) const
{
   G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
   analysisManager->FillH1(aHistNo, aValue);
   return;
}
