#include "AquilaSave.h"
#include "AquilaFiringPreview.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaFittingPreviewTest, "Aquila.Combat.FittingPreview",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaFittingPreviewTest::RunTest(const FString&)
{
    for (const auto& Model : AquilaWeapons::Models)
    for (auto Ammo : {EAquilaAmmo::Standard, EAquilaAmmo::Piercing, EAquilaAmmo::Phase})
    for (bool bChip : {false,true})
    {
        FAquilaFiringPreview D({0,Model.Weapon,Ammo,bChip});
        const bool bPlasma = FAquilaTurret::IsPlasma(Model.Weapon);
        const bool bPhase = !bPlasma && Ammo == EAquilaAmmo::Phase;
        const bool bPierces = bPlasma || Ammo == EAquilaAmmo::Piercing;
        int32 ShieldHits = 0;
        uint64 Seen = 0;
        bool bFlew = false, bCrossed = false, bBlocked = false, bHurt = false, bCountered = false;
        for (int32 Frame=0; Frame<2400 && D.Pass==1; ++Frame)
        {
            D.Advance(1./120);
            if (D.Pass != 1) break;
            for (const auto& R : D.Range.Rounds)
            {
                if (R.bEnemy) continue;
                bFlew |= R.Weapon == Model.Weapon && R.Ammo == (bPlasma ? EAquilaAmmo::Standard : Ammo);
                const double ToTarget = D.Range.EnemyPosition.X - R.Position.X;
                bCrossed |= D.Range.EnemyShield == D.ShieldCapacity && ToTarget < D.Range.ShieldRadius && ToTarget > D.Range.HullRadius;
            }
            for (const auto& Hit : D.Range.Impacts)
            {
                if (!Hit.bTargetEnemy || Hit.Serial <= Seen) continue;
                Seen = Hit.Serial;
                ShieldHits += int32(Hit.bShield);
                bBlocked |= Hit.bBlocked && D.Range.EnemyHull == D.HullCapacity;
                bHurt |= Hit.bHull && D.Range.EnemyShield == 0;
            }
            bCountered |= D.Range.DuelStats->Shots[0].Intercepted > 0;
        }
        if (!TestTrue(FString::Printf(TEXT("%s payload %d, chip %d follows both layers and loops"),Model.Name,int32(Ammo),bChip),
            bFlew && bCrossed==bPhase && ShieldHits==(bPhase ? 0 : 2) && bBlocked==!bPierces && bHurt==bPierces &&
            bCountered==bChip && D.Pass==2 && D.Range.EnemyShield==D.ShieldCapacity && D.Range.EnemyHull==D.HullCapacity &&
            D.DescriptionAge > 1.)) return false;
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaReserveTest, "Aquila.Combat.ReserveCapacitor",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaReserveTest::RunTest(const FString&)
{
    FAquilaDuelLoadout Fit;
    Fit.Sentinel.Armor = EAquilaArmor::Light;
    Fit.Resolute.bReserveCapacitor = Fit.Sentinel.bReserveCapacitor = true;
    FAquilaCampaign C;
    auto Reset = [&]() {
        C = FAquilaCampaign();
        C.BeginDuel(Fit);
        C.Order = EAquilaOrder::Hold;
        for (auto& Bank : C.Guns) for (double& Clock : Bank.Clock) Clock = 30;
    };
    auto Launch = [&](bool AtSentinel, double Damage, EAquilaAmmo Ammo = EAquilaAmmo::Standard) {
        const FVector Center = AtSentinel ? C.EnemyPosition : C.ShipPosition;
        const double Charge = AtSentinel ? C.EnemyShield : C.Shield;
        auto& Shot = C.Rounds.AddDefaulted_GetRef();
        Shot.Ammo = Ammo;
        Shot.bEnemy = !AtSentinel;
        Shot.Position = Center + FVector((Shot.HitsShield(Charge) ? C.ShieldRadius : C.HullRadius) + 1,0,0);
        Shot.Velocity = FVector(-8500,0,0);
        Shot.Turret = 0;
        Shot.Weapon = EAquilaWeapon::KineticCannon;
        Shot.DamageScale = Damage / 10.;
        Shot.Id = ++C.DuelStats->NextRoundId;
    };
    Reset();
    C.Shield = C.EnemyShield = 10;
    Launch(false,10);
    C.Advance(1./120);
    TestTrue(TEXT("Exact break restores 100 only on its owner"), C.Shield == 100 && C.Hull == 100 &&
        C.DuelStats->bReserveSpent[0] && !C.DuelStats->bReserveSpent[1] && C.EnemyShield == 10);
    Launch(true,12);
    C.Advance(1./120);
    TestTrue(TEXT("The other ship's reserve absorbs its own two overflow points"), C.EnemyShield == 98 && C.EnemyHull == 100 &&
        C.DuelStats->bReserveSpent[1]);
    Launch(false,104);
    C.Advance(1./120);
    TestTrue(TEXT("A spent capacitor cannot prevent the next hull hit"), C.Shield == 0 && C.Hull == 96);
    Reset();
    C.Shield = 4;
    Launch(false,9);
    Launch(false,9);
    C.Advance(1./120);
    TestTrue(TEXT("Two same-step impacts share one reserve"), C.Shield == 86 && C.Hull == 100 && C.Impacts.Num() == 2);
    Reset();
    C.Shield = 10;
    Launch(false,115);
    C.Advance(1./120);
    TestTrue(TEXT("A single hit can exhaust both banks and damage the hull"), C.Shield == 0 && C.Hull == 95);
    Reset();
    Launch(true,10,EAquilaAmmo::Phase);
    C.Advance(1./120);
    TestTrue(TEXT("A shield-bypassing hit leaves the reserve armed"), !C.DuelStats->bReserveSpent[1] && C.EnemyShield == 100 && C.EnemyHull == 90);
    Launch(true,100,EAquilaAmmo::Phase);
    C.Advance(1./120);
    TestTrue(TEXT("A capacitor cannot revive a destroyed ship"), !C.DuelStats->bReserveSpent[1] && C.EnemyHull == 0);
    Fit.Resolute.bShield = false;
    Fit.Sentinel.bReserveCapacitor = false;
    Reset();
    Launch(false,10);
    Launch(true,100);
    C.Advance(1./120);
    TestTrue(TEXT("An empty socket and a stock shield have no extra bank"), !C.Loadout.Resolute.bReserveCapacitor &&
        C.Shield == 0 && C.Hull == 90 && C.EnemyShield == 0 && !C.DuelStats->bReserveSpent[0] && !C.DuelStats->bReserveSpent[1]);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaPhaseTest, "Aquila.Combat.PhaseRounds",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaPhaseTest::RunTest(const FString&)
{
    for (bool bEnemy : {false, true})
    for (auto Armor : {EAquilaArmor::Light, EAquilaArmor::Prism, EAquilaArmor::Reinforced})
    for (bool bGraze : {false, true})
    {
        FAquilaDuelLoadout Fit;
        auto& Shooter = bEnemy ? Fit.Sentinel : Fit.Resolute;
        Shooter.Weapon = EAquilaWeapon::TwinTurrets;
        Shooter.PrepareRack();
        Shooter.Rack->Stock[0].Ammo = EAquilaAmmo::Phase;
        (bEnemy ? Fit.Resolute : Fit.Sentinel).Armor = Armor;
        FAquilaCampaign C;
        C.BeginDuel(Fit);
        C.Order = EAquilaOrder::Hold;
        C.Guns[bEnemy].Clock[0] = 0;
        C.FireBattery(bEnemy, FVector::ZeroVector, 35000, 1./120);
        if (!TestEqual(TEXT("One fitted turret fired"), C.Rounds.Num(), 1)) return false;
        auto& Shot = C.Rounds[0];
        TestTrue(TEXT("Launch captures the selected phase payload"), Shot.Ammo == EAquilaAmmo::Phase);
        const FVector Axis = bEnemy ? -C.DuelStats->Axis : C.DuelStats->Axis;
        const FVector Center = bEnemy ? C.ShipPosition : C.EnemyPosition;
        Shot.Position = Center - Axis * 3100 + (bGraze ? 2500. : 0.) * C.DuelStats->Sideways;
        Shot.Velocity = Axis * 8500;
        Shot.DamageScale = 1;
        for (auto& Side : C.Guns) for (double& Clock : Side.Clock) Clock = 30;
        const double& Shield = bEnemy ? C.Shield : C.EnemyShield;
        const double& Hull = bEnemy ? C.Hull : C.EnemyHull;
        C.Advance(.08);
        TestTrue(TEXT("No impact or shield cost at the outside surface"), C.Rounds.Num() == 1 && C.Impacts.IsEmpty() && Shield == 100 && Hull == 100);
        for (int32 Frame = 0; Frame < 5; ++Frame) C.Advance(.2);
        TestTrue(TEXT("A hull graze clears the shield without a hit"), C.Rounds.IsEmpty() && C.DuelStats->Shots[bEnemy].Misses == int32(bGraze));
        TestEqual(TEXT("Shield charge never changes"), Shield, 100.);
        const bool bBlocked = Armor == EAquilaArmor::Reinforced;
        TestEqual(TEXT("Only exposed compatible plating takes damage"), Hull, bGraze || bBlocked ? 100. : 94.);
        if (bGraze) { TestTrue(TEXT("No flash for a shield-only graze"), C.Impacts.IsEmpty()); continue; }
        if (!TestEqual(TEXT("One impact on the hull"), C.Impacts.Num(), 1)) return false;
        const auto& Hit = C.Impacts[0];
        TestTrue(TEXT("The contact belongs to the hull surface"), !Hit.bShield && Hit.bBlocked == bBlocked && Hit.bHull == !bBlocked &&
            FMath::IsNearlyEqual(Hit.LocalContact.Size(), C.HullRadius, .001));
        TestTrue(TEXT("A phase rejection does not suppress a neighbor shooting the shield"),
            C.Guns[bEnemy].Blocked[0] == bBlocked && !C.Guns[bEnemy].Blocked[1] && !(bEnemy ? C.bEnemyArmorBlocked : C.bArmorBlocked));
    }
    FAquilaDuelLoadout Fit;
    Fit.Resolute.PrepareRack();
    Fit.Resolute.Rack->Stock[0].Ammo = EAquilaAmmo::Phase;
    Fit.Resolute.Rack->Stock[0].Weapon = EAquilaWeapon::PulseRepeater;
    FAquilaCampaign C;
    C.BeginDuel(Fit);
    C.Guns[0].Clock[0] = 0;
    C.FireBattery(false, FVector::ZeroVector, 35000, .01);
    if (!TestEqual(TEXT("Plasma mount fires"), C.Rounds.Num(), 1)) return false;
    TestTrue(TEXT("A fixed plasma payload still hits the shield"), C.Rounds[0].HitsShield(100));
    FAquilaCampaign Journal;
    Journal.StartPractice();
    Journal.RevisionId = FGuid::NewGuid().ToString();
    Journal.Sequence = 1;
    for (auto Ammo : {EAquilaAmmo::Standard, EAquilaAmmo::Piercing, EAquilaAmmo::Phase})
        Journal.Rounds.AddDefaulted_GetRef().Ammo = Ammo;
    FAquilaCampaign Restored;
    FString Error;
    TestTrue(TEXT("New and legacy projectile payloads survive journal encoding"), FAquilaSave::Decode(FAquilaSave::Encode(Journal), Restored, Error) &&
        Restored.Rounds.Num() == 3 && Restored.Rounds[0].Ammo == EAquilaAmmo::Standard &&
        Restored.Rounds[1].Ammo == EAquilaAmmo::Piercing && Restored.Rounds[2].Ammo == EAquilaAmmo::Phase);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaLastVolleyTest, "Aquila.Combat.InterceptionAfterDestruction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaLastVolleyTest::RunTest(const FString&)
{
    for (bool bEnemy : {false, true}) // The surviving ship.
    for (bool bDistant : {false, true})
    for (double Cooldown : {0.8, 5.0})
    for (bool bChip : {false, true})
    {
        FAquilaDuelLoadout Fit;
        auto& Survivor = bEnemy ? Fit.Sentinel : Fit.Resolute;
        Survivor.Armor = EAquilaArmor::Light;
        Survivor.Weapon = EAquilaWeapon::TwinTurrets;
        Survivor.PrepareRack();
        for (auto& Gun : Survivor.Rack->Stock) Gun.bIntercept = bChip;
        (bEnemy ? Fit.Resolute : Fit.Sentinel).Armor = EAquilaArmor::Reinforced;
        FAquilaCampaign C;
        C.BeginDuel(Fit);
        C.Order = EAquilaOrder::Hold;
        const FVector Axis = bEnemy ? -C.DuelStats->Axis : C.DuelStats->Axis;
        const FVector Center = bEnemy ? C.EnemyPosition : C.ShipPosition;
        auto& Target = bEnemy ? C.ShipPosition : C.EnemyPosition;
        if (bDistant) Target = Center + Axis * 90000;
        auto& TargetHull = bEnemy ? C.Hull : C.EnemyHull;
        TargetHull = 1;
        (bEnemy ? C.Shield : C.EnemyShield) = 0;
        for (double& Clock : C.Guns[bEnemy].Clock) Clock = Cooldown;
        for (double& Clock : C.Guns[!bEnemy].Clock) Clock = 30;
        C.Guns[bEnemy].Blocked[0] = C.Guns[bEnemy].Blocked[1] = true;
        auto& Incoming = C.Rounds.AddDefaulted_GetRef();
        Incoming.Id = ++C.DuelStats->NextRoundId;
        Incoming.Position = Center + Axis * 18000;
        Incoming.Velocity = -Axis * 7000;
        Incoming.bEnemy = !bEnemy;
        Incoming.Weapon = EAquilaWeapon::KineticCannon;
        Incoming.Turret = 0;
        Incoming.Ammo = EAquilaAmmo::Phase;
        auto& FinalHit = C.Rounds.AddDefaulted_GetRef();
        FinalHit.Id = ++C.DuelStats->NextRoundId;
        FinalHit.Position = Target - Axis * (C.HullRadius + 2);
        FinalHit.Velocity = Axis * 4000;
        FinalHit.bEnemy = bEnemy;
        FinalHit.Weapon = EAquilaWeapon::PlasmaCaster;
        FinalHit.Turret = 0;
        C.Advance(1./120);
        TestTrue(TEXT("Ship explodes while its last round is outside chip range"), TargetHull == 0 && C.Rounds.Num() == 1 && C.DuelStats->ScreenShots.IsEmpty());
        int32 MaxClaims = 0;
        for (int32 Frame = 0; Frame < 4*60; ++Frame)
        {
            C.Advance(1./60);
            MaxClaims = FMath::Max(MaxClaims, C.DuelStats->ScreenShots.Num());
        }
        const bool bSaved = bChip && Cooldown < 1;
        TestEqual(TEXT("Two guns can claim the last round only once"), MaxClaims, int32(bSaved));
        TestEqual(TEXT("Only the ready survivor intercepts after the kill"), C.DuelStats->Shots[bEnemy].Intercepted, int32(bSaved));
        TestEqual(TEXT("An unchallenged posthumous phase round still damages the survivor"), bEnemy ? C.EnemyHull : C.Hull, bSaved ? 100. : 90.);
        TestTrue(TEXT("No new ship attacks or lingering shots"), C.DuelStats->NextRoundId == 2 && C.Rounds.IsEmpty() && C.DuelStats->ScreenShots.IsEmpty());
    }
    return true;
}
#endif
