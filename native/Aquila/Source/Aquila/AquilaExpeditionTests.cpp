#include "AquilaSave.h"
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#if WITH_DEV_AUTOMATION_TESTS
namespace
{
void Simulate(FAquilaCampaign& C, double Seconds, int32 Hz = 60)
{
    for (int32 I=0; I<FMath::RoundToInt(Seconds*Hz); ++I) C.Advance(1.0/Hz);
}
bool Until(FAquilaCampaign& C, EAquilaExpedition Stage, double Limit = 900, int32 Hz = 60)
{
    for (int32 I=0; I<Limit*Hz; ++I)
    {
        if (C.Expedition == Stage) return true;
        if (C.Expedition == EAquilaExpedition::Failed) return false;
        C.Advance(1.0/Hz);
    }
    return C.Expedition == Stage;
}
bool CompleteExpedition(FAquilaCampaign& C, int32 Hz = 60)
{
    if (!C.StartExpedition() || !Until(C,EAquilaExpedition::Battle,900,Hz)) return false;
    // The player's tactical choice, followed by automatic acquisition and projectile combat.
    C.SetPiercing(true);
    if (!Until(C,EAquilaExpedition::Salvage,900,Hz)) return false;
    C.ExpeditionAction();
    return Until(C,EAquilaExpedition::Complete,900,Hz);
}
bool Restore(const FAquilaCampaign& C, FAquilaCampaign& Out)
{
    FAquilaCampaign Saved = C;
    Saved.Sequence = 1;
    Saved.RevisionId = FGuid::NewGuid().ToString();
    FString Error;
    return FAquilaSave::Decode(FAquilaSave::Encode(Saved),Out,Error);
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaExpeditionLoopTest,"Aquila.Expedition.CompleteLoopAndUpgrade",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaExpeditionLoopTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign C;
    TestTrue(TEXT("Complete actual travel, survey, projectile battle and recovery"),CompleteExpedition(C));
    TestEqual(TEXT("One completed expedition"),C.CompletedExpeditions,1);
    TestEqual(TEXT("One salvage reward"),C.Salvage,120);
    TestTrue(TEXT("Ship survived automatic battle"),C.Hull > 0);
    AddInfo(FString::Printf(TEXT("First expedition simulation: %.2f seconds (%.2f minutes)."),C.ExpeditionSeconds,C.ExpeditionSeconds/60));
    TestTrue(TEXT("First loop stays within a bounded introductory session"),C.ExpeditionSeconds > 420 && C.ExpeditionSeconds < 900);
    Simulate(C,60);
    TestEqual(TEXT("Waiting after victory never pays twice"),C.Salvage,120);
    TestTrue(TEXT("Earned salvage buys the port upgrade"),C.UpgradePortTurret());
    TestEqual(TEXT("Upgrade charged exactly once"),C.Salvage,20);
    TestFalse(TEXT("Repeated refit cannot charge twice"),C.UpgradePortTurret());
    TestEqual(TEXT("Only the port turret advances"),C.PortTurretLevel,1);
    TestTrue(TEXT("A second expedition retains gains"),C.StartExpedition());
    TestEqual(TEXT("Next signal has a new identity"),C.ExpeditionNumber,2);
    TestEqual(TEXT("Next signal retains salvage"),C.Salvage,20);
    TestEqual(TEXT("Next signal retains upgrade"),C.PortTurretLevel,1);
    TestFalse(TEXT("Double start cannot skip an active expedition"),C.StartExpedition());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaExpeditionTacticsTest,"Aquila.Expedition.CommandsFailureAndPracticeIsolation",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaExpeditionTacticsTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign C;
    TestFalse(TEXT("Upgrade requires earned salvage"),C.UpgradePortTurret());
    C.StartExpedition();
    TestFalse(TEXT("Practice cannot reset an active mission"),C.StartPractice());
    C.Order=EAquilaOrder::Hold;
    const FVector Position=C.ShipPosition;
    Simulate(C,10);
    TestEqual(TEXT("Hold overrides the expedition route"),C.ShipPosition,Position);
    TestTrue(TEXT("Mission control resumes travel"),C.ExpeditionAction());
    TestTrue(TEXT("Arrive at survey site"),Until(C,EAquilaExpedition::Survey));
    Simulate(C,10);
    const double Progress=C.SurveySeconds;
    C.ShipPosition += FVector(0,20000,0);
    Simulate(C,5);
    TestEqual(TEXT("Survey pauses outside its radius"),C.SurveySeconds,Progress);
    C.ExpeditionAction();
    TestTrue(TEXT("Resume survey and discover threat"),Until(C,EAquilaExpedition::Battle));
    Simulate(C,1);
    TestEqual(TEXT("Expedition also has projectile travel time"),C.EnemyShield,100.0);
    for(int32 I=0; I<300*60 && !C.bArmorBlocked; ++I) C.Advance(1.0/60);
    TestTrue(TEXT("Standard rounds visibly identify incompatible armor"),C.bArmorBlocked);
    TestEqual(TEXT("Wrong ammunition cannot damage armor"),C.EnemyHull,100.0);
    C.SetPiercing(true);
    C.Order=EAquilaOrder::Withdraw;
    Simulate(C,25);
    TestTrue(TEXT("Withdrawal takes the ship outside weapon range"),FVector::Distance(C.ShipPosition,C.EnemyPosition)>65000);
    C.ExpeditionAction();
    TestTrue(TEXT("Reengagement still completes battle"),Until(C,EAquilaExpedition::Salvage));
    TestEqual(TEXT("Victory alone does not pay recovery reward"),C.Salvage,0);

    FAquilaCampaign Failed;
    Failed.StartExpedition();
    Failed.Hull=0;
    Failed.Advance(1.0/60);
    TestTrue(TEXT("Loss ends the mission"),Failed.Expedition==EAquilaExpedition::Failed);
    TestEqual(TEXT("Loss gives no reward"),Failed.Salvage,0);
    const FVector Signal=Failed.SignalPosition;
    TestTrue(TEXT("Retry is available"),Failed.ExpeditionAction());
    TestEqual(TEXT("Retry keeps the same signal identity"),Failed.ExpeditionNumber,1);
    TestEqual(TEXT("Retry keeps its destination"),Failed.SignalPosition,Signal);
    TestEqual(TEXT("Retry restores the ship"),Failed.Hull,100.0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaExpeditionSaveTest,"Aquila.Expedition.ResumeAndRewardTransactions",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaExpeditionSaveTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign C,Loaded;
    C.StartExpedition();
    for(EAquilaExpedition Phase : {EAquilaExpedition::Travel,EAquilaExpedition::Survey,EAquilaExpedition::Battle,EAquilaExpedition::Salvage})
    {
        if(Phase==EAquilaExpedition::Salvage) C.SetPiercing(true);
        TestTrue(TEXT("Reach save checkpoint"),Until(C,Phase));
        Simulate(C,3);
        TestTrue(TEXT("Restore each mission phase"),Restore(C,Loaded));
        Simulate(C,10); Simulate(Loaded,10);
        TestEqual(TEXT("Restored phase matches continued play"),static_cast<int32>(Loaded.Expedition),static_cast<int32>(C.Expedition));
        TestEqual(TEXT("Restored route matches"),Loaded.ShipPosition,C.ShipPosition);
        TestEqual(TEXT("Restored firing clocks and damage match"),Loaded.EnemyHull,C.EnemyHull);
        TestEqual(TEXT("Restored survey matches"),Loaded.SurveySeconds,C.SurveySeconds);
    }
    C.ExpeditionAction();
    TestTrue(TEXT("Complete recovery"),Until(C,EAquilaExpedition::Complete));
    const FString Dir=FPaths::ProjectSavedDir()/TEXT("Automation")/FGuid::NewGuid().ToString();
    FString Status; bool Writable=false;
    TestTrue(TEXT("Reward and completion committed together"),FAquilaSave::Commit(Dir,C,Status));
    TestTrue(TEXT("Restore committed reward"),FAquilaSave::LoadJournal(Dir,Loaded,Status,Writable));
    TestTrue(TEXT("Clean journal remains writable"),Writable);
    Simulate(Loaded,120);
    TestEqual(TEXT("Reloading cannot duplicate salvage"),Loaded.Salvage,120);
    Loaded.UpgradePortTurret();
    TestTrue(TEXT("Debit and upgrade committed together"),FAquilaSave::Commit(Dir,Loaded,Status));
    TestTrue(TEXT("Restore refitted ship"),FAquilaSave::LoadJournal(Dir,C,Status,Writable));
    TestEqual(TEXT("Refit retained"),C.PortTurretLevel,1);
    TestEqual(TEXT("Debit retained"),C.Salvage,20);
    TestFalse(TEXT("Restored refit cannot charge twice"),C.UpgradePortTurret());
    TestTrue(TEXT("Practice remains available after expedition"),C.StartPractice());
    TestEqual(TEXT("Practice retains salvage"),C.Salvage,20);
    TestEqual(TEXT("Practice retains completed expedition"),C.CompletedExpeditions,1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaExpeditionDamageTest,"Aquila.Expedition.UpgradeImpactAndFrameParity",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaExpeditionDamageTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign Base,Upgraded;
    Upgraded.Salvage=120;
    Upgraded.UpgradePortTurret();
    Base.StartPractice(); Upgraded.StartPractice();
    Simulate(Base,7); Simulate(Upgraded,7);
    TestTrue(TEXT("Earned upgrade changes actual projectile impact"),Upgraded.EnemyShield < Base.EnemyShield);
    TestTrue(TEXT("Starboard still launches baseline rounds"),Upgraded.Rounds.ContainsByPredicate([](const FAquilaRound& R){return !R.bEnemy && R.DamageScale==1;}));
    TestTrue(TEXT("Port launches upgraded rounds"),Upgraded.Rounds.ContainsByPredicate([](const FAquilaRound& R){return !R.bEnemy && R.DamageScale==1.5;}));
    FAquilaCampaign A,B;
    TestTrue(TEXT("Thirty Hz mission completes"),CompleteExpedition(A,30));
    TestTrue(TEXT("Sixty Hz mission completes"),CompleteExpedition(B,60));
    TestEqual(TEXT("Frame rate never changes rewards"),A.Salvage,B.Salvage);
    TestTrue(TEXT("Mission pacing agrees within one input frame"),FMath::Abs(A.ExpeditionSeconds-B.ExpeditionSeconds)<0.1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaExpeditionMigrationTest,"Aquila.Expedition.LegacyMigrationAndValidation",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaExpeditionMigrationTest::RunTest(const FString& Parameters)
{
    FAquilaCampaign C,Loaded;
    C.StartPractice(); Simulate(C,2); C.Sequence=1; C.RevisionId=FGuid::NewGuid().ToString();
    TSharedPtr<FJsonObject> J;
    FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(FAquilaSave::Encode(C)),J);
    J->SetNumberField(TEXT("schemaVersion"),2);
    for(const TCHAR* Key : {TEXT("expedition"),TEXT("expeditionNumber"),TEXT("completedExpeditions"),TEXT("salvage"),TEXT("portTurretLevel"),
        TEXT("signalPosition"),TEXT("expeditionSeconds"),TEXT("surveySeconds"),TEXT("recoverySeconds")}) J->RemoveField(Key);
    for(const auto& Shot:J->GetArrayField(TEXT("rounds"))) Shot->AsObject()->RemoveField(TEXT("damageScale"));
    FString Legacy,Error;
    FJsonSerializer::Serialize(J.ToSharedRef(),TJsonWriterFactory<>::Create(&Legacy));
    TestTrue(TEXT("Actual version-two field set migrates"),FAquilaSave::Decode(Legacy,Loaded,Error));
    TestEqual(TEXT("Old combat retains flying rounds"),Loaded.Rounds.Num(),C.Rounds.Num());
    TestTrue(TEXT("Migration invents no mission completion"),Loaded.Expedition==EAquilaExpedition::Available);
    TestEqual(TEXT("Migration invents no salvage"),Loaded.Salvage,0);
    for(const auto& R:Loaded.Rounds) TestEqual(TEXT("Old rounds retain original damage"),R.DamageScale,1.0);
    C.StartExpedition(); C.Sequence=1; C.RevisionId=FGuid::NewGuid().ToString();
    FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(FAquilaSave::Encode(C)),J);
    J->SetNumberField(TEXT("salvage"),-1);
    FString Invalid;
    FJsonSerializer::Serialize(J.ToSharedRef(),TJsonWriterFactory<>::Create(&Invalid));
    const FString Revision=Loaded.RevisionId;
    TestFalse(TEXT("Negative reward balance refused"),FAquilaSave::Decode(Invalid,Loaded,Error));
    TestEqual(TEXT("Invalid progression never mutates live state"),Loaded.RevisionId,Revision);
    return true;
}
#endif
