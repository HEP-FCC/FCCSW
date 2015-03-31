#include "FCCOutput.hh"
#include "FCCRunAction.hh"
#include "G4Run.hh"

FCCRunAction::FCCRunAction(const G4String aOutName="SimpleOutput")
 : G4UserRunAction()
{
   FCCOutput::Instance()->SetFileName(aOutName);
}

FCCRunAction::~FCCRunAction()
{
   delete FCCOutput::Instance();
}

void FCCRunAction::BeginOfRunAction(const G4Run* aRun)
{
   FCCOutput::Instance()->StartAnalysis(aRun->GetRunID());
   FCCOutput::Instance()->CreateHistograms();
}

void FCCRunAction::EndOfRunAction(const G4Run* /*aRun*/)
{
   FCCOutput::Instance()->EndAnalysis();
}
