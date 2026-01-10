#include "PhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4SystemOfUnits.hh"


PhysicsList::PhysicsList() : G4VUserPhysicsList()
{
  RegisterPhysics(new G4EmStandardPhysics());
  RegisterPhysics(new G4OpticalPhysics());
}

PhysicsList::~PhysicsList()
{
}

void PhysicsList::SetCuts()
{

    G4VUserPhysicsList::SetCuts();

    defaultCutValue = 1 * nm;
    SetCutsWithDefault();
}