#pragma once
#include "GameFramework/Pawn.h"
#include "AquilaFlightPawn.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class UInstancedStaticMeshComponent;
class UMaterialInstanceDynamic;
class SWidget;
class AAquilaGameMode;

UCLASS()
class AAquilaFlightPawn : public APawn
{
    GENERATED_BODY()
public:
    AAquilaFlightPawn();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    void ToggleBridge();
private:
    UPROPERTY() TObjectPtr<UCameraComponent> Camera;
    UPROPERTY() TObjectPtr<USceneComponent> ShipRoot;
    UPROPERTY() TObjectPtr<USceneComponent> EnemyRoot;
    UPROPERTY() TObjectPtr<USceneComponent> RoomRoot;
    UPROPERTY() TObjectPtr<USceneComponent> SignalRoot;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> PortUpgradeGlow;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Planet;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> FriendlyRounds;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> HostileRounds;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> Impacts;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> Stars;
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> Materials;
    TSharedPtr<SWidget> Overlay;
    TArray<TSharedPtr<SWidget>> TouchUIRegions;
    bool bTouchLayout = false;
    bool bCommandsVisible = true;
    int32 CommandTab = 2;
    bool bBridge = false;
    bool bDragging = false;
    float OrbitYaw = 20;
    float OrbitPitch = 23;
    float BridgeYaw = 0;
    float BridgePitch = 0;
    FVector Eye = FVector(-650, 0, 175);
    FVector2D LastTouch = FVector2D::ZeroVector;
    bool bTouching = false;
    FVector2D TouchWalk = FVector2D::ZeroVector;
    double AliveTime = 0;
    bool bCaptured = false;
    AAquilaGameMode* Game() const;
    bool CommandEnabled(int32 Id) const;
    UStaticMeshComponent* Shape(USceneComponent* Parent, const TCHAR* MeshPath, FVector Position, FVector Size, int32 Material);
    UInstancedStaticMeshComponent* Instances(int32 Material);
    void BuildShip(USceneComponent* Parent, bool bEnemy);
    void BuildRoom();
    void BuildOverlay();
    void BuildTouchOverlay();
    void RenderCampaign();
};
