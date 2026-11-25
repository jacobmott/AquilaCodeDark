#pragma once
#include "GameFramework/GameModeBase.h"
#include "AquilaSimulation.h"
#include "AquilaGameMode.generated.h"

enum class EAquilaSession : uint8 { Menu, Match, Simulator, Boarding };

UCLASS()
class AAquilaGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AAquilaGameMode();
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    bool Save();
    void PlayMatch();
    void TravelToBoarding(bool bEnter);
    void StartDuel(FAquilaDuelLoadout Loadout = {});
    bool ReturnToMenu();
    EAquilaSession Session() const { return ActiveSession; }
    void Background();
    void Foreground();
    void Command(int32 CommandId);
    FAquilaCampaign Campaign;
    bool bFittingOpen = false;
    FString SaveStatus;
    bool bPaused = false;
    bool bWritable = true;
    bool bFastDuel = true;
private:
    EAquilaSession ActiveSession = EAquilaSession::Menu;
    bool bPauseOnShieldHit = false;
    FString SaveDirectory;
    double SinceSave = 0;
    FDelegateHandle BackgroundHandle, ForegroundHandle, DeactivateHandle;
};
