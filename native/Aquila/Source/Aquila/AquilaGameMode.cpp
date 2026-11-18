#include "AquilaGameMode.h"
#include "AquilaFlightPawn.h"
#include "AquilaSave.h"
#include "Misc/CoreDelegates.h"
#include "Misc/Paths.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

AAquilaGameMode::AAquilaGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    DefaultPawnClass = AAquilaFlightPawn::StaticClass();
}

void AAquilaGameMode::BeginPlay()
{
    SaveDirectory = FPaths::ProjectSavedDir() / TEXT("Campaign");
    FString TestDirectory;
    if (FParse::Value(FCommandLine::Get(), TEXT("AquilaSaveDir="), TestDirectory)) SaveDirectory = TestDirectory;
    FAquilaSave::LoadJournal(SaveDirectory, Campaign, SaveStatus, bWritable);
    // Super starts pawn play, whose development scene may issue a command and save immediately.
    Super::BeginPlay();
    BackgroundHandle = FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &AAquilaGameMode::Background);
    ForegroundHandle = FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &AAquilaGameMode::Foreground);
    DeactivateHandle = FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &AAquilaGameMode::Background);
    UE_LOG(LogTemp, Display, TEXT("Aquila native campaign started. Sequence %d, %s"), Campaign.Sequence, *SaveStatus);
}

void AAquilaGameMode::Tick(float Seconds)
{
    Super::Tick(Seconds);
    if (bPaused || !bWritable) return;
    const EAquilaExpedition PreviousStage = Campaign.Expedition;
    Campaign.Advance(Seconds);
    SinceSave += Seconds;
    if (SinceSave >= 30 || Campaign.Expedition != PreviousStage) Save();
}

void AAquilaGameMode::Save()
{
    if (!bWritable || SaveDirectory.IsEmpty()) return;
    if (FAquilaSave::Commit(SaveDirectory, Campaign, SaveStatus)) SinceSave = 0;
    UE_LOG(LogTemp, Display, TEXT("Aquila save: %s; revision %d"), *SaveStatus, Campaign.Sequence);
}

void AAquilaGameMode::Background() { bPaused = true; Save(); }
void AAquilaGameMode::Foreground() { bPaused = true; }

void AAquilaGameMode::Command(int32 Id)
{
    if (Id == 8) { bPaused = !bPaused; Save(); return; }
    if (Id == 9) { Save(); return; }
    if (!bWritable) return;
    if (Id == 0) Campaign.Order = EAquilaOrder::Hold;
    if (Id == 1) Campaign.Order = EAquilaOrder::Approach;
    if (Id == 2) Campaign.Order = EAquilaOrder::CombatRange;
    if (Id == 3) Campaign.Order = EAquilaOrder::Withdraw;
    if (Id == 4) Campaign.StartPractice();
    if (Id == 5) Campaign.bHoldFire = !Campaign.bHoldFire;
    if (Id == 6) Campaign.SetPiercing(!Campaign.bPiercing);
    if (Id == 11) Campaign.ExpeditionAction();
    if (Id == 12) Campaign.UpgradePortTurret();
    Save();
}

void AAquilaGameMode::EndPlay(const EEndPlayReason::Type Reason)
{
    Save();
    FCoreDelegates::ApplicationWillEnterBackgroundDelegate.Remove(BackgroundHandle);
    FCoreDelegates::ApplicationHasEnteredForegroundDelegate.Remove(ForegroundHandle);
    FCoreDelegates::ApplicationWillDeactivateDelegate.Remove(DeactivateHandle);
    Super::EndPlay(Reason);
}
