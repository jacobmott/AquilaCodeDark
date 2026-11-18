#pragma once
#include "GameFramework/GameModeBase.h"
#include "AquilaSimulation.h"
#include "AquilaGameMode.generated.h"

UCLASS()
class AAquilaGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AAquilaGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    void Save();
    void Background();
    void Foreground();
    void Command(int32 CommandId);
    FAquilaCampaign Campaign;
    FString SaveStatus;
    bool bPaused = false;
    bool bWritable = true;
private:
    FString SaveDirectory;
    double SinceSave = 0;
    FDelegateHandle BackgroundHandle, ForegroundHandle, DeactivateHandle;
};
