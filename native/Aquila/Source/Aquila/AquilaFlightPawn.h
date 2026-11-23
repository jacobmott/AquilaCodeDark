#pragma once
#include "GameFramework/Pawn.h"
#include "InputCoreTypes.h"
#include "AquilaFlightPawn.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class UInstancedStaticMeshComponent;
class UMaterialInstanceDynamic;
class SWidget;
class SAquilaMouseLook;
class AAquilaGameMode;

enum class EAquilaCameraTarget : uint8 { Free, Resolute, Sentinel };

UCLASS()
class AAquilaFlightPawn : public APawn
{
    GENERATED_BODY()
public:
    AAquilaFlightPawn();
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    void ToggleBridge();
private:
    static constexpr double ExteriorScale = 0.1;
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
    TSharedPtr<SAquilaMouseLook> MouseLook;
    TArray<TSharedPtr<SWidget>> TouchUIRegions;
    bool bTouchLayout = false;
    bool bCommandsVisible = false;
    int32 CommandTab = 2;
    bool bBridge = false;
    EAquilaCameraTarget CameraTarget = EAquilaCameraTarget::Free;
    bool bObserverPlaced = false;
    FVector ObserverFocus = FVector::ZeroVector; // Simulation meters, independent of the helm.
    double OrbitDistance = 4800; // Render centimeters.
    float RecenterDelay = 0;
    float OrbitYaw = 32;
    float OrbitPitch = 38;
    float BridgeYaw = 0;
    float BridgePitch = 0;
    FVector Eye = FVector(-650, 0, 175);
    FVector2D LastTouch = FVector2D::ZeroVector;
    FVector2D TouchStart = FVector2D::ZeroVector;
    bool bTouching = false;
    bool bTouchDragged = false;
    bool bTouchRotate = false;
    FVector2D TouchWalk = FVector2D::ZeroVector;
    double AliveTime = 0;
    bool bCaptured = false;
    AAquilaGameMode* Game() const;
    bool CommandEnabled(int32 Id) const;
    void RunControl(int32 Id);
    void ToggleCommands();
    void BeginCameraTouch(ETouchIndex::Type Finger, FVector Position);
    void EndCameraTouch(ETouchIndex::Type Finger, FVector Position);
    void UpdateTouchCamera(APlayerController* PC);
    void UpdateCursor();
    void DragDesktopCamera(FVector2D Delta, FKey Button);
    void PickShip(FVector2D ViewUV);
    void Follow(EAquilaCameraTarget Target);
    void ReleaseFollow();
    void PanObserver(FVector Direction, double RenderDistance);
    void PanByPixels(FVector2D Delta);
    void ZoomObserver(float Steps);
    float TargetHeading() const;
    void UpdateCamera(float Seconds);
    UStaticMeshComponent* Shape(USceneComponent* Parent, const TCHAR* MeshPath, FVector Position, FVector Size, int32 Material);
    UInstancedStaticMeshComponent* Instances(int32 Material);
    void BuildShip(USceneComponent* Parent, bool bEnemy);
    void BuildRoom();
    void BuildOverlay();
    void BuildTouchOverlay();
    void RenderCampaign();
};
