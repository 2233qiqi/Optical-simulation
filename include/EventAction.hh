#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void AddCount(G4int pixelID) {
        if(pixelID >= 0 && pixelID < 64) {
            fSiPMCounts[pixelID]++;
        }
    }

  private:
    std::vector<G4int> fSiPMCounts;
};

#endif