#include "AquilaSave.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#if WITH_DEV_AUTOMATION_TESTS
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
        Shot.Velocity=FVector(8500,0,0); Shot.bPiercing=Piercing;
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
    TestTrue(TEXT("Restored piercing loadout launches new rounds"),Restored.Rounds.ContainsByPredicate([](const FAquilaRound& R){return !R.bEnemy && R.bPiercing;}));
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
