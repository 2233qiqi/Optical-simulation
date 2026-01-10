#include "EventAction.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"

EventAction::EventAction(): G4UserEventAction()
{
        fSiPMCounts.resize(64, 0);
}

EventAction::~EventAction()
{
    
}

void EventAction::BeginOfEventAction(const G4Event*)
{
    std::fill(fSiPMCounts.begin(), fSiPMCounts.end(), 0);
}


void EventAction::EndOfEventAction(const G4Event* event)
{
    auto analysisManager = G4AnalysisManager::Instance();

    analysisManager->FillNtupleIColumn(0, event->GetEventID());
    
    for(int i = 0; i < 64; i++) {
        analysisManager->FillNtupleIColumn(i + 1, fSiPMCounts[i]);
    }
    
    analysisManager->AddNtupleRow();
}