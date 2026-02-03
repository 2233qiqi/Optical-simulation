#include "DetectorConstruction.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

// 构造函数
DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}

// 析构函数
DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    // 1. 获取材料管理器
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* bgo_mat   = nist->FindOrBuildMaterial("G4_BGO");
    
    // 定义钨铜合金 (W-Cu: 80% W, 20% Cu, 密度约 15.0 g/cm3)
    G4Material* wcu_mat = new G4Material("WCu_Alloy", 15.0*g/cm3, 2);
    wcu_mat->AddMaterial(nist->FindOrBuildMaterial("G4_W"), 80*perCent);
    wcu_mat->AddMaterial(nist->FindOrBuildMaterial("G4_Cu"), 20*perCent);

    // 2. 世界逻辑体
    G4double world_size = 2.5 * m;
    G4Box* solidWorld = new G4Box("World", world_size, world_size, world_size);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0);

    // --- 3. 探测器部分 (19x19 BGO Array) ---
    G4double cry_xy = 2.4 * mm;
    G4double cry_z  = 10.0 * mm;
    G4Box* solidBGO = new G4Box("BGOCrystal", cry_xy/2, cry_xy/2, cry_z/2);
    G4LogicalVolume* logicBGO = new G4LogicalVolume(solidBGO, bgo_mat, "BGOCrystal");

    for (G4int i = 0; i < 19; i++) {
        for (G4int j = 0; j < 19; j++) {
            G4double x = (i - 9) * cry_xy;
            G4double y = (j - 9) * cry_xy;
            new G4PVPlacement(0, G4ThreeVector(x, y, 0), logicBGO, "BGOCrystal", logicWorld, false, i * 19 + j, true);
        }
    }

    // --- 4. MURA 准直器部分 (37x37 Mask) ---
    G4int p = 19; // MURA 参数
    G4double mask_unit_xy = 2.4 * mm;
    G4double mask_thickness = 8.0 * mm;
    G4double MDD = 59.0 * mm; // 准直器到探测器的距离
    
    // 计算 MURA 基础图案 (19x19)
    G4int mura19[19];
    mura19[0] = 0; // MURA 定义: A_0 = 0
    for(int i=1; i<p; i++) {
        bool is_residue = false;
        for(int k=0; k<p; k++) {
            if((k*k) % p == i) { is_residue = true; break; }
        }
        mura19[i] = is_residue ? 1 : 0;
    }

    // 放置准直器单元 (37x37)
    G4Box* solidMaskUnit = new G4Box("MaskUnit", mask_unit_xy/2, mask_unit_xy/2, mask_thickness/2);
    G4LogicalVolume* logicMaskUnit = new G4LogicalVolume(solidMaskUnit, wcu_mat, "MaskUnit");

    G4double mask_z_pos = (cry_z / 2.0) + MDD + (mask_thickness / 2.0);

    for (G4int i = 0; i < 37; i++) {
        for (G4int j = 0; j < 37; j++) {
            // MURA 二维逻辑: A_ij = 1 if (A_i + A_j) mod 2 == 0, else 0 (这里 1 为开孔，0 为遮挡)
            // 循环嵌套扩展: M_ij = A_(i mod p, j mod p)
            G4int mi = i % p;
            G4int mj = j % p;
            
            G4int val;
            if (mi == 0) val = 0;
            else if (mj == 0) val = 1;
            else val = (mura19[mi] + mura19[mj]) % 2 == 0 ? 1 : 0;

            // 论文中：1 是孔，0 是调制单元(钨铜)。所以我们只放置 val == 0 的块。
            if (val == 0) {
                G4double x = (i - 18) * mask_unit_xy;
                G4double y = (j - 18) * mask_unit_xy;
                new G4PVPlacement(0, G4ThreeVector(x, y, mask_z_pos), logicMaskUnit, "MaskUnit", logicWorld, false, i * 37 + j, true);
            }
        }
    }

    // 可视化设置
    logicBGO->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 1.0, 1.0))); // 青色探测器
    logicMaskUnit->SetVisAttributes(new G4VisAttributes(G4Colour(0.5, 0.5, 0.5))); // 灰色准直器
    logicWorld->SetVisAttributes(new G4VisAttributes(G4Color(1.0,1.0,1.0,0.005)));

    return physWorld;
}