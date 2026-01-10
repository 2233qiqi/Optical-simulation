#include "SteppingAction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4VPhysicalVolume.hh"

SteppingAction::SteppingAction(EventAction* eventAction): G4UserSteppingAction(),fEventAction(eventAction)
{

}

SteppingAction::~SteppingAction()
{

}

void SteppingAction::UserSteppingAction(const G4Step* step)
{

   /* G4Track* track = step->GetTrack();
    G4String particleName = track->GetDefinition()->GetParticleName();

    // ---------------------------------------------------------
    // 诊断 1: 检查源粒子是否打中了东西？
    // ---------------------------------------------------------
    // 如果是源粒子（比如 gamma），且步长为0（刚生成），打印位置
    if (track->GetTrackID() == 1 && track->GetCurrentStepNumber() == 1) {
        G4cout << ">>> Primary Particle Generated: " << particleName 
               << " at Position: " << track->GetPosition() << G4endl;
    }

    // ---------------------------------------------------------
    // 诊断 2: 检查是否有光子产生？
    // ---------------------------------------------------------
    if (particleName == "opticalphoton") {
        
        // 这是一个计数器，防止打印太多刷屏，只打印前10个光子的行为
        static G4int photonPrintCount = 0;
        if (photonPrintCount < 20) {
            
            G4StepPoint* preStep = step->GetPreStepPoint();
            G4StepPoint* postStep = step->GetPostStepPoint();

            G4String preVolName = "World";
            if (preStep->GetPhysicalVolume()) preVolName = preStep->GetPhysicalVolume()->GetName();

            G4String postVolName = "Out_of_World";
            if (postStep->GetPhysicalVolume()) postVolName = postStep->GetPhysicalVolume()->GetName();

            G4cout << "--- Photon Step ---" << G4endl;
            G4cout << "   From: " << preVolName << " -> To: " << postVolName << G4endl;
            
            photonPrintCount++;
        }
    }*/
    G4Track* track = step->GetTrack();

    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) 
    return;

    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4StepPoint* postStepPoint = step->GetPostStepPoint();

    if (!postStepPoint->GetPhysicalVolume())
    return;
    
    G4String preVolName = preStepPoint->GetPhysicalVolume()->GetName();
    G4String postVolName = postStepPoint->GetPhysicalVolume()->GetName();

    if (preVolName != "SiPM" && postVolName == "SiPM") {
        
    
        G4int copyNo = postStepPoint->GetPhysicalVolume()->GetCopyNo();

        fEventAction->AddCount(copyNo);

        track->SetTrackStatus(fStopAndKill); 
    }
}