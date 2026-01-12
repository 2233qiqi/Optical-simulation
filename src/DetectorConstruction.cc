#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"

#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"


DetectorConstruction::DetectorConstruction() 
{

}
DetectorConstruction::~DetectorConstruction(){

}


G4VPhysicalVolume*DetectorConstruction::Construct()
{
   
    G4bool checkOverlaps = true;

    
    //world材料
    G4NistManager *nist = G4NistManager::Instance();
    G4Material *WorldMat = nist->FindOrBuildMaterial("G4_AIR");

    //BGO材料
    G4Material *BGO=new G4Material("BGO", 7.13*g/cm3, 3);
    BGO ->AddElement(nist->FindOrBuildElement("Bi"), 4);
    BGO ->AddElement(nist->FindOrBuildElement("Ge"), 3);
    BGO ->AddElement(nist ->FindOrBuildElement("O"), 12);
    
    //硅脂材料
    G4Material* OpticalGrease = new G4Material( "OpticalGrease",0.97*g/cm3, 3);
    OpticalGrease->AddElement(nist->FindOrBuildElement("Si"), 1);
    OpticalGrease->AddElement(nist->FindOrBuildElement("O"), 1);
    OpticalGrease->AddElement(nist->FindOrBuildElement("C"), 2);

    //玻璃
    G4Material* SiPM_Glass = nist->FindOrBuildMaterial("G4_Pyrex_Glass");
    const G4int nEntries = 2;
    G4double photonEnergy[nEntries] = { 1.5 * eV, 6.2 * eV }; 

    //  空气光学属性 
    G4double rindexAir[nEntries] = { 1.0, 1.0 };
    G4MaterialPropertiesTable* mptAir = new G4MaterialPropertiesTable();
    mptAir->AddProperty("RINDEX", photonEnergy, rindexAir, nEntries,true);
    WorldMat->SetMaterialPropertiesTable(mptAir); 

    G4double rindexBGO[nEntries] = { 2.15, 2.15 };
    G4double absLengthBGO[nEntries] = { 1.0 * m, 1.0 * m }; 
    G4double scintilFast[nEntries] = { 1.0, 1.0 }; 

    //BGO属性
    G4MaterialPropertiesTable* mptBGO = new G4MaterialPropertiesTable();
    mptBGO->AddProperty("RINDEX", photonEnergy, rindexBGO, nEntries, true);
    mptBGO->AddProperty("ABSORPTIONLENGTH", photonEnergy, absLengthBGO, nEntries, true);
    mptBGO->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scintilFast, nEntries, true);
    
    mptBGO->AddConstProperty("SCINTILLATIONYIELD", 8200. / MeV); 
    mptBGO->AddConstProperty("RESOLUTIONSCALE", 1.0);
    mptBGO->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 300.0 * ns); 
    mptBGO->AddConstProperty("SCINTILLATIONYIELD1", 1.0);      

    BGO->SetMaterialPropertiesTable(mptBGO);

    //光学硅脂属性
    G4double rindexGrease[nEntries] = { 1.47, 1.47 }; 
    G4double absLengthGrease[nEntries] = { 5.0 * m, 5.0 * m };
    G4MaterialPropertiesTable* mptGrease = new G4MaterialPropertiesTable();
    mptGrease->AddProperty("RINDEX", photonEnergy, rindexGrease, nEntries, true);
    mptGrease->AddProperty("ABSORPTIONLENGTH", photonEnergy, absLengthGrease, nEntries, true);
    OpticalGrease->SetMaterialPropertiesTable(mptGrease);

    //SIPM玻璃
    G4double rindexGlass[nEntries] = { 1.50, 1.50 };
    G4double absLengthGlass[nEntries] = { 5.0 * m, 5.0 * m };
    G4MaterialPropertiesTable* mptGlass = new G4MaterialPropertiesTable();
    mptGlass->AddProperty("RINDEX", photonEnergy, rindexGlass, nEntries, true);
    mptGlass->AddProperty("ABSORPTIONLENGTH", photonEnergy, absLengthGlass, nEntries, true);
    SiPM_Glass->SetMaterialPropertiesTable(mptGlass);
    

    //参数定义
    const G4int nSipmX = 8;
    const G4int nSipmY = 8;

    const G4double sipmXY = 6.13*mm;
    const G4double sipmZ  = 0.64*mm;

    const G4double greaseZ = 0.16*mm;

    const G4double bgoXY = 6.0*mm;
    const G4double bgoZ  = 50.0*mm;

    //世界
    G4double WordSize = 200.*mm;
    auto SolidWord =new G4Box("World",WordSize/2,WordSize/2,WordSize/2);
    auto LogicWorld =new G4LogicalVolume(SolidWord,WorldMat,"World");
    G4VPhysicalVolume *PhysWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), LogicWorld, "PhysWorld", 0, false, 0, checkOverlaps);
    LogicWorld->SetVisAttributes(G4VisAttributes::GetInvisible()); 

    //SiPM
    auto SolidSiPM =new G4Box ("SiPM",sipmXY/2,sipmXY/2,sipmZ/2);
    auto logicSiPM =new G4LogicalVolume(SolidSiPM,SiPM_Glass ,"SiPM");
    G4VisAttributes* sipmVis = new G4VisAttributes(G4Color(1.0, 1.0, 0.0)); 
    sipmVis->SetForceSolid(true); 
    logicSiPM->SetVisAttributes(sipmVis);

    //硅脂
    auto SolidGrease =new G4Box("Grease",sipmXY/2,sipmXY/2,greaseZ/2);
    auto LogicGrease =new G4LogicalVolume(SolidGrease,OpticalGrease,"Grease");
    G4VisAttributes* greaseVis = new G4VisAttributes(G4Color(1.0, 0.0, 0.0)); 
    LogicGrease->SetVisAttributes(greaseVis);

    //BGO
    auto SolidBGO =new G4Box ("BGO",bgoXY/2,bgoXY/2,bgoZ/2);
    auto LogicBGO =new G4LogicalVolume(SolidBGO,BGO,"BGO");
    G4VisAttributes* bgoVis = new G4VisAttributes(G4Color(0.0, 1.0, 1.0, 0.4)); 
    bgoVis->SetForceSolid(true);
    LogicBGO->SetVisAttributes(bgoVis);

    //位置
    G4double greaseZpos = 0.0;
    G4double sipmZpos   = - (greaseZ/2 + sipmZ/2); 
    G4double bgoZpos    =   (greaseZ/2 + bgoZ/2);  

    G4double arraySizeX = nSipmX * sipmXY;
    G4double arraySizeY = nSipmY * sipmXY;

    G4int copyNo = 0; 

    for (G4int ix = 0; ix < nSipmX; ix++)
    {
        for (G4int iy = 0; iy < nSipmY; iy++)
        {
            G4double x = (ix + 0.5)*sipmXY - arraySizeX/2;
            G4double y = (iy + 0.5)*sipmXY - arraySizeY/2;
            G4ThreeVector pos(x, y, 0); 

            auto PhySiPM = new G4PVPlacement(nullptr, G4ThreeVector(x, y, sipmZpos), logicSiPM, "SiPM", LogicWorld, false, copyNo,checkOverlaps);

            auto PhyGrease = new G4PVPlacement(nullptr, G4ThreeVector(x, y, greaseZpos), LogicGrease, "Grease", LogicWorld,  false, copyNo, checkOverlaps);

            auto PhyBGO = new G4PVPlacement(nullptr, G4ThreeVector(x, y, bgoZpos), LogicBGO, "BGO", LogicWorld, false, copyNo, checkOverlaps);
            
            copyNo++; 
        }
    }
    return PhysWorld;
}

