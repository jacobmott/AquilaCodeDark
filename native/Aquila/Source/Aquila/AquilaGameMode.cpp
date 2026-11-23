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

void AAquilaGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    SaveDirectory = FPaths::ProjectSavedDir() / TEXT("Campaign");
    FString TestDirectory;
    const TCHAR* Args = FCommandLine::Get();
    const bool bHasDirectory = FParse::Value(Args, TEXT("AquilaSaveDir="), TestDirectory);
    if (bHasDirectory) SaveDirectory = TestDirectory;
    FString Mode, Demo;
    FParse::Value(Args, TEXT("AquilaMode="), Mode);
    const bool bDesktop = PLATFORM_WINDOWS && !FParse::Param(Args, TEXT("AquilaTouchUI"));
    if (bDesktop && Mode.Equals(TEXT("Simulator"), ESearchCase::IgnoreCase))
        StartDuel(false);
    else if (!bDesktop || Mode.Equals(TEXT("Match"), ESearchCase::IgnoreCase)
        || (Mode.IsEmpty() && (bHasDirectory || FParse::Value(Args, TEXT("AquilaDemo="), Demo))))
        PlayMatch();
}

void AAquilaGameMode::BeginPlay()
{
    Super::BeginPlay();
    BackgroundHandle = FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &AAquilaGameMode::Background);
    ForegroundHandle = FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &AAquilaGameMode::Foreground);
    DeactivateHandle = FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &AAquilaGameMode::Background);
}

void AAquilaGameMode::PlayMatch()
{
    if (ActiveSession != EAquilaSession::Menu) return;
    Campaign = FAquilaCampaign();
    FAquilaSave::LoadJournal(SaveDirectory, Campaign, SaveStatus, bWritable);
    ActiveSession = EAquilaSession::Match;
    SinceSave = 0;
    bPaused = false;
    UE_LOG(LogTemp, Display, TEXT("Aquila native campaign started. Sequence %d, %s"), Campaign.Sequence, *SaveStatus);
}

void AAquilaGameMode::StartDuel(bool bPiercing)
{
    if (ActiveSession == EAquilaSession::Match) return;
    ActiveSession = EAquilaSession::Simulator;
    Campaign = FAquilaCampaign();
    Campaign.SetPiercing(bPiercing);
    Campaign.BeginExpeditionBattle();
    bPaused = false;
    UE_LOG(LogTemp, Display, TEXT("Aquila simulator duel: %s"), bPiercing ? TEXT("piercing") : TEXT("standard"));
}

bool AAquilaGameMode::ReturnToMenu()
{
    if (ActiveSession == EAquilaSession::Match && bWritable && !Save()) return false;
    ActiveSession = EAquilaSession::Menu;
    Campaign = FAquilaCampaign();
    SaveStatus.Reset();
    bPaused = false;
    return true;
}

void AAquilaGameMode::Tick(float Seconds)
{
    Super::Tick(Seconds);
    if (bPaused || ActiveSession == EAquilaSession::Menu) return;
    if (ActiveSession == EAquilaSession::Simulator)
    {
        const int32 Steps = bFastDuel ? 8 : 1;
        for (int32 I = 0; I < Steps; ++I)
        {
            // A ship can be disabled while its last burst is still in flight.
            if (Campaign.Expedition != EAquilaExpedition::Battle && Campaign.Rounds.IsEmpty() && Campaign.Impacts.IsEmpty()) break;
            Campaign.Advance(Seconds);
        }
        return;
    }
    if (!bWritable) return;
    const EAquilaExpedition PreviousStage = Campaign.Expedition;
    Campaign.Advance(Seconds);
    SinceSave += Seconds;
    if (SinceSave >= 30 || Campaign.Expedition != PreviousStage) Save();
}

bool AAquilaGameMode::Save()
{
    if (ActiveSession != EAquilaSession::Match || !bWritable || SaveDirectory.IsEmpty()) return false;
    const bool bSaved = FAquilaSave::Commit(SaveDirectory, Campaign, SaveStatus);
    if (bSaved) SinceSave = 0;
    UE_LOG(LogTemp, Display, TEXT("Aquila save: %s; revision %d"), *SaveStatus, Campaign.Sequence);
    return bSaved;
}

void AAquilaGameMode::Background() { bPaused = true; Save(); }
void AAquilaGameMode::Foreground() { bPaused = true; }

void AAquilaGameMode::Command(int32 Id)
{
    if (ActiveSession == EAquilaSession::Menu) return;
    if (Id == 8) { bPaused = !bPaused; Save(); return; }
    if (ActiveSession != EAquilaSession::Match) return;
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
