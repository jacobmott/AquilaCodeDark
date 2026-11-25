#include "AquilaSave.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaLoadoutTest, "Aquila.Combat.BothShipLoadouts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaLoadoutTest::RunTest(const FString&)
{
    for (bool bShooterEnemy : {false, true})
    for (const auto Weapon : {EAquilaWeapon::TwinTurrets, EAquilaWeapon::KineticCannon, EAquilaWeapon::PlasmaCaster, EAquilaWeapon::PulseRepeater})
    for (bool bPiercing : {false, true})
    for (const auto Armor : {EAquilaArmor::Light, EAquilaArmor::Reinforced, EAquilaArmor::Prism, EAquilaArmor::BareHull})
    for (bool bShield : {false, true})
    {
        FAquilaCampaign C;
        auto& Shooter = bShooterEnemy ? C.Loadout.Sentinel : C.Loadout.Resolute;
        auto& Target = bShooterEnemy ? C.Loadout.Resolute : C.Loadout.Sentinel;
        Shooter.Weapon = Weapon;
        Shooter.bPiercing = bPiercing;
        Target.Armor = Armor;
        Target.bShield = bShield;
        C.BeginExpeditionBattle();
        C.Order = EAquilaOrder::Hold;
        C.Shield = C.Loadout.Resolute.bShield ? 100 : 0;
        const FVector Center = bShooterEnemy ? C.ShipPosition : C.EnemyPosition;
        auto& Cycle = C.Guns[bShooterEnemy ? 1 : 0];
        Cycle.Clock[0] = 0;
        C.FireBattery(bShooterEnemy, FVector::ZeroVector, 35000, 1./120);
        if (!TestEqual(TEXT("A fitted mount launched a round"), C.Rounds.Num(), 1)) return false;
        auto& Shot = C.Rounds[0];
        const bool bPlasma = Weapon == EAquilaWeapon::PlasmaCaster || Weapon == EAquilaWeapon::PulseRepeater;
        double BaseDamage = bPiercing ? 8 : 6;
        double Speed = 8500;
        if (Weapon == EAquilaWeapon::KineticCannon) { BaseDamage = 10; Speed = 7000; }
        if (Weapon == EAquilaWeapon::PlasmaCaster) { BaseDamage = 36; Speed = 4000; }
        if (Weapon == EAquilaWeapon::PulseRepeater) { BaseDamage = 8; Speed = 22000; }
        TestTrue(TEXT("Flight speed and ammunition follow the selected battery"), Shot.bEnemy == bShooterEnemy &&
            Shot.Ammo == (!bPlasma && bPiercing ? EAquilaAmmo::Piercing : EAquilaAmmo::Standard) && FMath::IsNearlyEqual(Shot.Velocity.Size(), Speed, 1.e-6));
        const double Damage = BaseDamage * C.ExpeditionDamageScale;
        auto& Shield = bShooterEnemy ? C.Shield : C.EnemyShield;
        const double Absorbed = bShield ? Damage / 2 : 0;
        Shield = Absorbed;
        Shot.Position = Center + FVector((bShield ? C.ShieldRadius : C.HullRadius) + 1, 0, 0);
        Shot.Velocity = FVector(-8500, 0, 0);
        C.Step(1./120);
        const bool bReinforcedKinetic = Armor == EAquilaArmor::Reinforced && !bPlasma;
        const bool bBlocked = (bReinforcedKinetic && !bPiercing) || (Armor == EAquilaArmor::Prism && bPlasma);
        const double ExpectedHull = 100 - (bBlocked ? 0 : (Damage - Absorbed) * (bReinforcedKinetic ? 0.75 : 1));
        TestTrue(TEXT("Shield overflow and armor follow the defender on either side"),
            Shield == 0 && FMath::IsNearlyEqual(bShooterEnemy ? C.Hull : C.EnemyHull, ExpectedHull, 1.e-8) &&
            (bShooterEnemy ? C.bEnemyArmorBlocked : C.bArmorBlocked) == bBlocked && C.Impacts.Num() == 1 && C.Impacts[0].bShield == bShield);
    }
    FAquilaCampaign Draw;
    Draw.Loadout.Resolute.Armor = EAquilaArmor::Reinforced;
    Draw.BeginExpeditionBattle();
    for (int32 I = 0; I < 6000 && !(Draw.bEnemyArmorBlocked && Draw.bArmorBlocked && Draw.Rounds.IsEmpty()); ++I)
        Draw.Advance(.25);
    TestTrue(TEXT("Two blocked batteries cease fire with both hulls intact"), Draw.bEnemyArmorBlocked && Draw.bArmorBlocked &&
        Draw.Rounds.IsEmpty() && Draw.EnemyHull == 100 && Draw.Hull == 100);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaLiveFittingTest, "Aquila.Combat.LiveFitting",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaLiveFittingTest::RunTest(const FString&)
{
    auto Strip = [](FAquilaShipLoadout& Ship) { Ship.ClearFitting(); };
    for (const bool bEnemy : {false, true})
    {
        const int32 Side = bEnemy ? 1 : 0;
        FAquilaCampaign Duel;
        FAquilaDuelLoadout Fitting;
        Fitting.ForShip(bEnemy).bReserveCapacitor = true;
        Duel.BeginDuel(Fitting);
        const auto& Ship = Duel.Loadout.ForShip(bEnemy);
        const int32 Bodies = Ship.Rack->Stock.Num();
        const int32 OpponentGuns = Duel.Loadout.ForShip(!bEnemy).Rack->Installed();
        double& Charge = bEnemy ? Duel.EnemyShield : Duel.Shield;
        double& Hull = bEnemy ? Duel.EnemyHull : Duel.Hull;
        Charge = 42.;
        Hull = 73.;
        Duel.DuelStats->bReserveSpent[Side] = true;
        Duel.Guns[Side].Clock[0] = 0.;
        Duel.FireBattery(bEnemy, FVector::ZeroVector, 35000., .01);
        if (!TestEqual(TEXT("One fitted gun fires before clearing"), Duel.Rounds.Num(), 1)) return false;
        const auto Shot = Duel.Rounds[0];
        Duel.Guns[1-Side].Blocked[0] = true;
        Duel.Guns[1-Side].Clock[0] = 1.25;
        const auto Generation = Duel.EncounterGeneration;
        Duel.EditDuelShip(bEnemy, Strip);
        Duel.EditDuelShip(bEnemy, Strip);
        TestTrue(TEXT("Repeated Clear empties the chosen ship and its live shield"), Ship.Rack->Stock.Num()==Bodies &&
            Ship.Rack->Installed()==0 && Ship.Armor==EAquilaArmor::BareHull && !Ship.bShield && !Ship.bReserveCapacitor && Charge==0.);
        TestTrue(TEXT("Opponent retries the new defenses with its existing guns and cooldown"), !Duel.Guns[1-Side].Blocked[0] &&
            Duel.Guns[1-Side].Clock[0]==1.25 && Duel.Loadout.ForShip(!bEnemy).Rack->Installed()==OpponentGuns);
        Duel.FireBattery(bEnemy, FVector::ZeroVector, 35000., 3.);
        TestTrue(TEXT("Clearing preserves launched rounds and the wounded hull"), Duel.Rounds.Num()==1 && Duel.Rounds[0].Id==Shot.Id &&
            Duel.Rounds[0].Position==Shot.Position && Duel.Rounds[0].Ammo==Shot.Ammo && Hull==73. && Duel.EncounterGeneration==Generation);
        Duel.EditDuelShip(bEnemy, [](FAquilaShipLoadout& Fit) {
            Fit.Rack->Fit(29, EAquilaWeapon::KineticCannon);
            Fit.bShield = Fit.bReserveCapacitor = true;
        });
        TestTrue(TEXT("Returned gear can fire again without restoring the spent reserve"), Charge==100. && Hull==73. &&
            Duel.DuelStats->bReserveSpent[Side] && Ship.Rack->Installed()==1 && Duel.Guns[Side].Burst[0]==0);
        Duel.FireBattery(bEnemy, FVector::ZeroVector, 35000., 3.);
        TestTrue(TEXT("The first new round uses the refitted cannon"), Duel.Rounds.Num()==2 && Duel.Rounds.Last().Weapon==EAquilaWeapon::KineticCannon);
        Hull = 0.;
        Duel.EditDuelShip(bEnemy, Strip);
        Duel.EditDuelShip(bEnemy, [](FAquilaShipLoadout& Fit) { Fit.bShield = true; });
        TestTrue(TEXT("Fitting cannot revive a destroyed ship"), Hull==0. && Charge==0. && Duel.EncounterGeneration==Generation);
    }
    FAquilaCampaign Match;
    const FString Saved = FAquilaSave::Encode(Match);
    Match.EditDuelShip(false, Strip);
    TestTrue(TEXT("Arena fitting does not alter a match"), Match.Loadout.Resolute.bShield && !Match.Loadout.Resolute.Rack &&
        FAquilaSave::Encode(Match)==Saved);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaMountTest, "Aquila.Combat.MountInventoryAndInterception",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaMountTest::RunTest(const FString&)
{
    FAquilaDuelLoadout Cleared;
    for (auto* Ship : {&Cleared.Resolute, &Cleared.Sentinel})
    {
        const int32 Owned = Ship->StarterCount();
        Ship->bPiercing = Ship->bReserveCapacitor = true;
        Ship->ClearFitting();
        Ship->ClearFitting();
        TestTrue(TEXT("Clearing implicit starter gear twice preserves exactly the owned bodies"), Ship->Rack &&
            Ship->Rack->Stock.Num()==Owned && Ship->Rack->Installed()==0 && !Ship->bShield && !Ship->bReserveCapacitor &&
            !Ship->bPiercing && Ship->Armor==EAquilaArmor::BareHull);
    }
    auto& Spares = Cleared.Resolute.Rack->Stock;
    Spares[0] = {4, EAquilaWeapon::KineticCannon, EAquilaAmmo::Phase, true};
    Spares[1] = {INDEX_NONE, EAquilaWeapon::PlasmaCaster, EAquilaAmmo::Piercing, true};
    Cleared.Resolute.ClearFitting();
    for (const auto& Body : Spares)
        TestTrue(TEXT("Mounted and spare bodies both lose their loadout"), Body.Mount==INDEX_NONE &&
            Body.Weapon==EAquilaWeapon::TwinTurrets && Body.Ammo==EAquilaAmmo::Standard && !Body.bIntercept);
    FAquilaCampaign Empty;
    Empty.BeginDuel(Cleared);
    Empty.Advance(.25);
    TestTrue(TEXT("Empty explicit racks stay silent with two bare hulls"), Empty.Rounds.IsEmpty() &&
        Empty.bArmorBlocked && Empty.bEnemyArmorBlocked && Empty.Shield==0 && Empty.EnemyShield==0 &&
        Empty.Hull==100 && Empty.EnemyHull==100);
    FAquilaDuelLoadout Fit;
    Fit.Resolute.PrepareRack();
    Fit.Sentinel.PrepareRack();
    auto& Rack = Fit.Resolute.Rack.GetValue();
    TestTrue(TEXT("Two owned turrets cannot fill a third mount"), Rack.Installed() == 2 && !Rack.Fit(29, EAquilaWeapon::PlasmaCaster));
    Rack.Stock[0].bIntercept = true;
    Rack.Stock[0].Ammo = EAquilaAmmo::Piercing;
    TestTrue(TEXT("Moving retains the same turret and enhancement"), Rack.Place(0,29) && Rack.At(6) == INDEX_NONE &&
        Rack.At(29) == 0 && Rack.Stock[0].bIntercept && Rack.Stock[0].Ammo == EAquilaAmmo::Piercing && !Rack.Place(1,29));
    TestTrue(TEXT("Returning a turret retains it in inventory"), Rack.Place(0,INDEX_NONE) && Rack.Installed() == 1 &&
        Rack.Fit(9,EAquilaWeapon::PulseRepeater) && Rack.Stock[0].bIntercept);
    Rack.Stock.SetNum(30);
    for (int32 I = 0; I < 30; ++I) TestTrue(TEXT("Every physical mount accepts a turret"), Rack.Fit(I,EAquilaWeapon::TwinTurrets));
    TestTrue(TEXT("Thirty unique mounts and no spare"), Rack.Installed() == 30 && Rack.At(INDEX_NONE) == INDEX_NONE);
    FAquilaCampaign Full;
    Full.BeginDuel(Fit);
    for (auto& Clock : Full.Guns[0].Clock) Clock = 0;
    Full.FireBattery(false, FVector::ZeroVector, 35000, .01);
    TSet<int32> GunIds;
    for (const auto& R : Full.Rounds)
    {
        GunIds.Add(R.Turret);
        TestTrue(TEXT("Launch comes from the installed physical mount"),
            R.Position.Equals(Rack.Stock[R.Turret].Muzzle(false)*10.,.01));
    }
    TestEqual(TEXT("All 30 guns launch"), GunIds.Num(), 30);
    for (bool bEnemy : {false, true})
    for (const auto Defense : {EAquilaWeapon::TwinTurrets, EAquilaWeapon::KineticCannon, EAquilaWeapon::PlasmaCaster, EAquilaWeapon::PulseRepeater})
    for (const auto Threat : {EAquilaWeapon::KineticCannon, EAquilaWeapon::PlasmaCaster})
    {
        FAquilaDuelLoadout Setup;
        Setup.Resolute.PrepareRack();
        Setup.Sentinel.PrepareRack();
        auto& Def = bEnemy ? Setup.Sentinel : Setup.Resolute;
        auto& Att = bEnemy ? Setup.Resolute : Setup.Sentinel;
        Def.Rack->Stock.SetNum(1);
        Def.Rack->Stock[0] = {6, Defense, EAquilaAmmo::Standard, true};
        Att.Weapon = Threat;
        auto Duel = [&](bool bChip, int32 Fps) {
            Def.Rack->Stock[0].bIntercept = bChip;
            FAquilaCampaign C;
            C.BeginDuel(Setup);
            C.Order = EAquilaOrder::Hold;
            for (auto& Gun : C.Guns[!bEnemy].Clock) Gun = 100;
            const FVector Ship = bEnemy ? C.EnemyPosition : C.ShipPosition;
            auto& R = C.Rounds.AddDefaulted_GetRef();
            const FVector ToOpponent = bEnemy ? -C.DuelStats->Axis : C.DuelStats->Axis;
            R.Position = Ship + ToOpponent * 10000;
            R.Velocity = ToOpponent * -7000;
            R.Id = ++C.DuelStats->NextRoundId;
            R.bEnemy = !bEnemy;
            R.Weapon = Threat;
            R.Turret = 0;
            C.Guns[bEnemy].Clock[0] = 0;
            C.Advance(1. / Fps);
            if (bChip) TestTrue(TEXT("Chip sends a separate visible shot before contact"), C.DuelStats->ScreenShots.Num()==1 && C.Rounds.Num()==1);
            for (int32 I=1; I<3*Fps; ++I) C.Advance(1. / Fps);
            return C;
        };
        const auto Bare = Duel(false,60);
        const auto Guard = Duel(true,30);
        const auto Fast = Duel(true,120);
        TestTrue(TEXT("Either chip protects either hull against both payload families at contact"),
            Guard.DuelStats->Shots[bEnemy].Intercepted==1 && Fast.DuelStats->Shots[bEnemy].Intercepted==1 &&
            (bEnemy ? Guard.EnemyShield : Guard.Shield)==100 && (bEnemy ? Bare.EnemyShield : Bare.Shield)<100);
    }
    FAquilaDuelLoadout Mixed;
    Mixed.Resolute.PrepareRack();
    Mixed.Sentinel.PrepareRack();
    Mixed.Sentinel.bShield = false;
    Mixed.Resolute.Rack->Stock[1].Weapon = EAquilaWeapon::PlasmaCaster;
    FAquilaCampaign Combined;
    Combined.BeginDuel(Mixed);
    for (int32 Frame = 0; Frame < 60*60; ++Frame) Combined.Advance(1./60);
    TestTrue(TEXT("One blocked mount cannot silence a compatible neighbor"), Combined.Guns[0].Blocked[0] &&
        !Combined.Guns[0].Blocked[1] && !Combined.bArmorBlocked && Combined.EnemyHull < 100 && Combined.DuelStats->Shots[0].Blocked > 0);
    FAquilaCampaign Late;
    Mixed.Resolute.Rack->Stock[0].bIntercept = Mixed.Resolute.Rack->Stock[1].bIntercept = true;
    Late.BeginDuel(Mixed);
    Late.Guns[0].Clock[0] = Late.Guns[0].Clock[1] = 0;
    auto& Close = Late.Rounds.AddDefaulted_GetRef();
    Close.Id = ++Late.DuelStats->NextRoundId;
    Close.bEnemy = true;
    Close.Position = FVector(Late.ShieldRadius + 260, 0, 0);
    Close.Velocity = FVector(-8500, 0, 0);
    Late.Advance(1./120);
    TestEqual(TEXT("Two chips cannot claim one threat twice"), Late.DuelStats->ScreenShots.Num(), 1);
    Late.Advance(.2);
    TestTrue(TEXT("An incoming shot arriving first still hits exactly once"), Late.Shield < 100 && Late.DuelStats->Shots[1].Hits == 1 &&
        Late.DuelStats->Shots[0].Intercepted == 0 && Late.DuelStats->ScreenShots.IsEmpty());
    FAquilaCampaign Quiet;
    Fit.Resolute.bWeave = Fit.Sentinel.bWeave = true;
    Quiet.BeginDuel(Fit);
    TestTrue(TEXT("Inactive legacy fitting flags are discarded"), !Quiet.Loadout.Resolute.bWeave && !Quiet.Loadout.Sentinel.bWeave);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaImpactSurfaceTest, "Aquila.Combat.ImpactSurfacesAndDestruction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaImpactSurfaceTest::RunTest(const FString&)
{
    FVector Entry;
    TestTrue(TEXT("Swept entry on the near face"), FAquilaCampaign::SegmentHitsSphere(
        FVector(-10,3,0), FVector(10,3,0), FVector::ZeroVector, 5, &Entry) && Entry.Equals(FVector(-4,3,0)));
    TestTrue(TEXT("Tangent has one contact"), FAquilaCampaign::SegmentHitsSphere(
        FVector(-10,5,0), FVector(10,5,0), FVector::ZeroVector, 5, &Entry) && Entry.Equals(FVector(0,5,0)));
    TestFalse(TEXT("Near miss remains a miss"), FAquilaCampaign::SegmentHitsSphere(
        FVector(-10,5.1,0), FVector(10,5.1,0), FVector::ZeroVector, 5, &Entry));
    TestTrue(TEXT("Starting inside still registers"), FAquilaCampaign::SegmentHitsSphere(
        FVector(1,0,0), FVector(2,0,0), FVector::ZeroVector, 5, &Entry) && Entry.Equals(FVector(1,0,0)));

    for (const bool bEnemyTarget : {true, false})
    {
        FAquilaCampaign C;
        C.StartPractice();
        C.bHoldFire = true;
        C.Guns[1].Clock[0] = 10;
        C.Heading = UE_PI / 3;
        double& Shield = bEnemyTarget ? C.EnemyShield : C.Shield;
        double& Hull = bEnemyTarget ? C.EnemyHull : C.Hull;
        auto Fire = [&](bool bPiercing)
        {
            auto& Round = C.Rounds.AddDefaulted_GetRef();
            Round.bEnemy = !bEnemyTarget;
            Round.Ammo = bPiercing ? EAquilaAmmo::Piercing : EAquilaAmmo::Standard;
            const double Radius = Shield > 0 ? C.ShieldRadius : C.HullRadius;
            Round.Position = (bEnemyTarget ? C.EnemyPosition : C.ShipPosition) - FVector(Radius + 12, 0, 0);
            Round.Velocity = FVector(8500,0,0);
            C.Advance(1./120);
        };
        Fire(false);
        if (!TestEqual(TEXT("One hit emitted"), C.Impacts.Num(), 1)) return false;
        const FRotator Facing(0, bEnemyTarget ? 180. : 60., 0);
        const auto& First = C.Impacts.Last();
        TestTrue(TEXT("Exact hit side follows the struck ship's heading"), First.bTargetEnemy == bEnemyTarget &&
            Facing.RotateVector(First.LocalContact).Equals(FVector(-C.ShieldRadius,0,0), 0.001) &&
            Facing.RotateVector(First.LocalDirection).Equals(FVector::ForwardVector, 0.001));
        TestTrue(TEXT("Full absorption emits only a shield strike"), First.bShield && !First.bHull && !First.bBlocked && !First.bDestroyed);
        if (bEnemyTarget)
        {
            Shield = 0;
            Fire(false);
            TestTrue(TEXT("Armor glances without a hull-damage flare"), C.Impacts.Last().bBlocked && !C.Impacts.Last().bHull);
        }
        Shield = 1;
        Hull = 1;
        Fire(true);
        const auto& Lethal = C.Impacts.Last();
        TestTrue(TEXT("One penetrating round can light shield and hull and destroy the ship"),
            Lethal.bShield && Lethal.bHull && Lethal.bDestroyed && !Lethal.bBlocked);
        const uint64 Last = Lethal.Serial;
        Fire(true);
        TestEqual(TEXT("In-flight rounds cannot explode a wreck twice"), C.LastImpactSerial, Last);
        FAquilaCampaign Restored;
        C.Sequence = 1;
        C.RevisionId = FGuid::NewGuid().ToString();
        FString Error;
        TestTrue(TEXT("Restoring a destroyed ship does not replay its explosion"),
            FAquilaSave::Decode(FAquilaSave::Encode(C), Restored, Error) && Restored.Impacts.IsEmpty() && Restored.LastImpactSerial == 0);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaTravelTest, "Aquila.Combat.TravelAndArmor", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaTravelTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign C;
    C.StartPractice();
    for (int32 I=0; I<120; ++I) C.Advance(1.0/120);
    TestEqual(TEXT("No instant damage"), C.EnemyShield, 100.0);
    TestTrue(TEXT("Visible rounds in flight"), C.Rounds.Num() > 0);
    for (int32 I=0; I<1800; ++I) C.Advance(1.0/120);
    TestEqual(TEXT("Standard blocked by armor"), C.EnemyHull, 100.0);
    TestTrue(TEXT("Armor explanation raised"), C.bArmorBlocked);
    C.SetPiercing(true);
    C.Shield=100; C.Hull=100;
    for (int32 I=0; I<2400; ++I) C.Advance(1.0/120);
    TestEqual(TEXT("Piercing defeats armor"), C.EnemyHull, 0.0);
    TestEqual(TEXT("Victory counted once"), C.Victories, 1);
    TestTrue(TEXT("Round pool bounded"), C.Rounds.Num() <= FAquilaCampaign::MaxRounds);
    for (bool Piercing : {false,true})
    {
        FAquilaCampaign Overflow;
        Overflow.StartPractice(); Overflow.bHoldFire=true; Overflow.EnemyShield=2;
        FAquilaRound Shot;
        Shot.Position=Overflow.EnemyPosition-FVector(4000,0,0);
        Shot.Velocity=FVector(8500,0,0); Shot.Ammo = Piercing ? EAquilaAmmo::Piercing : EAquilaAmmo::Standard;
        Overflow.Rounds.Add(Shot);
        Overflow.Advance(0.2);
        TestEqual(TEXT("Shield overflow checks armor"),Overflow.EnemyHull,Piercing?95.5:100.0);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaAmmoSwitchTest, "Aquila.Combat.SwitchWithRoundsInFlight", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaAmmoSwitchTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign C;
    C.StartPractice(); C.EnemyShield=0; C.bHoldFire=true;
    FAquilaRound OldRound;
    OldRound.Position=C.EnemyPosition-FVector(4000,0,0);
    OldRound.Velocity=FVector(8500,0,0);
    C.Rounds.Add(OldRound);
    C.SetPiercing(true);
    C.Advance(0.25);
    TestEqual(TEXT("Old standard shot still cannot damage armor"),C.EnemyHull,100.0);
    TestTrue(TEXT("Old shot still shows blocked impact"),C.Impacts.ContainsByPredicate([](const FAquilaImpact& I){return I.bBlocked;}));
    TestFalse(TEXT("Old impact does not block selected piercing ammunition"),C.bArmorBlocked);
    C.bHoldFire=false;
    for(int I=0; I<1200; ++I) C.Advance(1.0/120);
    TestTrue(TEXT("Automatic piercing fire continues and damages hull"),C.EnemyHull<100);

    // Earlier builds could save this stale combination. It must not stop restored combat.
    FAquilaCampaign Restored;
    Restored.StartPractice(); Restored.SetPiercing(true); Restored.bArmorBlocked=true;
    TestFalse(TEXT("Restored piercing loadout has no incorrect refit instruction"),Restored.CombatStatus().Contains(TEXT("FIT PIERCING")));
    for(int I=0; I<120; ++I) Restored.Advance(1.0/120);
    TestTrue(TEXT("Restored piercing loadout launches new rounds"),Restored.Rounds.ContainsByPredicate([](const FAquilaRound& R){return !R.bEnemy && R.Ammo == EAquilaAmmo::Piercing;}));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaTimingTest, "Aquila.Combat.FrameRateAndHold", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaTimingTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign A,B;
    A.StartPractice(); B.StartPractice();
    for(int I=0; I<300; ++I) A.Advance(1.0/30);
    for(int I=0; I<600; ++I) B.Advance(1.0/60);
    TestEqual(TEXT("30/60 Hz shield parity"), A.EnemyShield,B.EnemyShield);
    TestEqual(TEXT("30/60 Hz projectile parity"), A.Rounds.Num(),B.Rounds.Num());
    FAquilaCampaign Held;
    Held.StartPractice();
    for(int I=0; I<120; ++I) Held.Advance(1.0/120);
    Held.bHoldFire=true;
    for(int I=0; I<600; ++I) Held.Advance(1.0/120);
    TestTrue(TEXT("Hold does not erase launched shots"),Held.EnemyShield<100);
    TestFalse(TEXT("No fresh friendly rounds during hold"), Held.Rounds.ContainsByPredicate([](const FAquilaRound& R){ return !R.bEnemy && R.Age<4; }));
    TestTrue(TEXT("Swept collision prevents tunneling"),FAquilaCampaign::SegmentHitsSphere(FVector(-10000,0,0),FVector(10000,0,0),FVector::ZeroVector,10));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaNavigationTest, "Aquila.Navigation.ClearanceAndArrival", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaNavigationTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign C;
    C.Order=EAquilaOrder::Approach;
    for(int I=0; I<3600; ++I) C.Advance(1.0/60);
    TestTrue(TEXT("Ship approaches requested orbit"), C.ShipPosition.X > 85000);
    TestTrue(TEXT("Planet clearance preserved"), FVector::Distance(C.ShipPosition,FVector(180000,0,0)) >=80000);
    TestTrue(TEXT("Helm holds on arrival"), C.Order==EAquilaOrder::Hold);
    const FVector P=C.ShipPosition;
    C.Advance(1);
    TestEqual(TEXT("Hold preserves position"),C.ShipPosition,P);
    C.ShipPosition=FVector(-100000,0,0); C.EnemyPosition=FVector::ZeroVector; C.Order=EAquilaOrder::Withdraw;
    C.Advance(0.1);
    TestEqual(TEXT("Withdrawal never moves toward an already distant enemy"),C.ShipPosition,FVector(-100000,0,0));
    C.ShipPosition=FVector(90000,0,0); C.StartPractice();
    TestTrue(TEXT("Practice enemy remains outside planet clearance"),FVector::Distance(C.EnemyPosition,FVector(180000,0,0))>=80000);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaCodecTest, "Aquila.Save.RoundTripMigrationAndRejection", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaCodecTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign C,Loaded;
    C.StartPractice(); C.Order=EAquilaOrder::CombatRange; C.SetPiercing(true);
    for(int I=0; I<360; ++I) C.Advance(1.0/120);
    C.Sequence=1; C.RevisionId=FGuid::NewGuid().ToString();
    FString Error;
    const FString Json=FAquilaSave::Encode(C);
    TestTrue(TEXT("Portable JSON round trip"),FAquilaSave::Decode(Json,Loaded,Error));
    TestEqual(TEXT("World identity"),Loaded.WorldSeed,C.WorldSeed);
    TestEqual(TEXT("Travel position"),Loaded.ShipPosition,C.ShipPosition);
    TestEqual(TEXT("In-flight rounds"),Loaded.Rounds.Num(),C.Rounds.Num());
    for(int I=0; I<360; ++I) {C.Advance(1.0/120);Loaded.Advance(1.0/120);}
    TestEqual(TEXT("Continued combat matches after restore"),Loaded.EnemyShield,C.EnemyShield);
    TestEqual(TEXT("Continued health matches after restore"),Loaded.Hull,C.Hull);
    FString Future=Json.Replace(TEXT("\"schemaVersion\": 3"),TEXT("\"schemaVersion\": 99"));
    const FString Revision=Loaded.RevisionId;
    TestFalse(TEXT("Newer schema refused"),FAquilaSave::Decode(Future,Loaded,Error));
    TestEqual(TEXT("Failure leaves live state intact"),Loaded.RevisionId,Revision);
    TestFalse(TEXT("Truncated file refused"),FAquilaSave::Decode(Json.Left(Json.Len()/2),Loaded,Error));
    FString Old=Json.Replace(TEXT("\"schemaVersion\": 3"),TEXT("\"schemaVersion\": 1"));
    TestTrue(TEXT("Version 1 migrates"),FAquilaSave::Decode(Old,Loaded,Error));
    TestEqual(TEXT("Version 1 migration begins with empty rounds"),Loaded.Rounds.Num(),0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaJournalTest, "Aquila.Save.InterruptionAndConflict", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaJournalTest::RunTest(const FString& Parameters)
{
    // Unique isolated test folders are retained for evidence; never touch a player's journal.
    const FString Dir=FPaths::ProjectSavedDir()/TEXT("Automation")/FGuid::NewGuid().ToString();
    FAquilaCampaign C,Loaded;
    FString Status; bool Writable;
    TestTrue(TEXT("First immutable save"),FAquilaSave::Commit(Dir,C,Status));
    FAquilaCampaign Branch=C;
    C.ShipPosition=FVector(1400,2800,350);
    TestTrue(TEXT("Second immutable save"),FAquilaSave::Commit(Dir,C,Status));
    FFileHelper::SaveStringToFile(TEXT("interrupted write"),*(Dir/TEXT("unfinished.json.pending")));
    TestTrue(TEXT("Read complete revision after interruption"),FAquilaSave::LoadJournal(Dir,Loaded,Status,Writable));
    TestTrue(TEXT("Pending write does not destroy earlier save"),Writable);
    TestEqual(TEXT("Latest state restored"),Loaded.ShipPosition,C.ShipPosition);
    Branch.ShipPosition=FVector(-100,0,0);
    TestTrue(TEXT("Divergent branch is preserved"),FAquilaSave::Commit(Dir,Branch,Status));
    TestTrue(TEXT("Conflict retains a readable campaign"),FAquilaSave::LoadJournal(Dir,Loaded,Status,Writable));
    TestFalse(TEXT("Conflict blocks automatic writes"),Writable);
    TArray<FString> Files;
    IFileManager::Get().FindFiles(Files,*(Dir/TEXT("*.json")),true,false);
    TestEqual(TEXT("All three revisions remain"),Files.Num(),3);
    return true;
}
#endif
