#include "Output.hh"
#include "RunAction.hh"
#include "G4Run.hh"

RunAction::RunAction(const G4String aOutName="SimpleOutput")
 : G4UserRunAction()
{
   Output::Instance()->SetFileName(aOutName);
}

RunAction::~RunAction()
{
   delete Output::Instance();
}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
   Output::Instance()->StartAnalysis(aRun->GetRunID());
   Output::Instance()->CreateHistograms();
}

void RunAction::EndOfRunAction(const G4Run* /*aRun*/)
{
   Output::Instance()->EndAnalysis();
}
