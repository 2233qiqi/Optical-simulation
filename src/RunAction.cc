#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include <string> 

RunAction::RunAction(): G4UserRunAction()
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetVerboseLevel(1);
    

    analysisManager->CreateNtuple("BGO_Sim", "Hits Data");
    

    analysisManager->CreateNtupleIColumn("EventID");
    

    for(int i = 0; i < 64; i++) {
        G4String columnName = "Pixel_" + std::to_string(i);
        analysisManager->CreateNtupleIColumn(columnName);
    }

    analysisManager->FinishNtuple();
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*)
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile("BGO_Result.root");
}

void RunAction::EndOfRunAction(const G4Run*)
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}
//hadd BGO_Final.root BGO_Result_t*.root