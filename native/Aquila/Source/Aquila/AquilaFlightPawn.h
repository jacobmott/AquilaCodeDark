#pragma once
#include "GameFramework/Pawn.h"
#include "InputCoreTypes.h"
#include "AquilaWeapons.h"
#include "AquilaDefenses.h"
#include "AquilaFiringPreview.h"
#include "AquilaFlightPawn.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class UStaticMesh;
class UInstancedStaticMeshComponent;
class UMaterialInstanceDynamic;
class UMaterialInterface;
struct FAquilaCampaign;
struct FAquilaRound;
struct FAquilaDuelLoadout;
struct FAquilaShipLoadout;
class SCanvas;
class SWidget;
class SAquilaMouseLook;
class SAquilaFiringRibbon;
class AAquilaGameMode;
class AAquilaBoardingBay;
class AAquilaBoardingWalker;

enum class EAquilaCameraTarget : uint8 { Free, Resolute, Sentinel };
enum class EAquilaBurst : uint8 { Shield, Armor, Hull, Destruction };

struct FAquilaVisualBurst
{
    EAquilaBurst Kind;
    bool bEnemy;
    FVector Point; // Render centimeters in the struck ship's frame.
    FVector Normal;
    uint64 Serial;
    float Age = 0; // Presentation seconds, independent of duel speed.
    FVector MeshPoint = FVector::ZeroVector;
    FVector MeshNormal = FVector::ZeroVector;
};

struct FAquilaShieldEnvelope
{
    FVector Center = FVector::ZeroVector;
    FVector Radii = FVector::OneVector;
    static FAquilaShieldEnvelope Fit(TConstArrayView<FBox> Parts);
    FVector Contact(FVector Point, FVector Direction, FVector& Normal) const;
};

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
    void LeaveDuelBridge();
    void SwitchDuelShip();
private:
    friend class FAquilaCombatFeedbackFrames;
    friend class FAquilaSimulatorTest;
    friend class FAquilaObserverTest;
    friend class FAquilaDuelBridgeWalk;
    friend class FAquilaDuelShipJump;
    static constexpr double ExteriorScale = 0.1;
    static constexpr double HullShieldScale = 1.08;
    UPROPERTY() TObjectPtr<UCameraComponent> Camera;
    UPROPERTY() TObjectPtr<USceneComponent> ShipRoot;
    UPROPERTY() TObjectPtr<USceneComponent> EnemyRoot;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> HullBodies[2];
    UPROPERTY() TObjectPtr<USceneComponent> RoomRoot;
    UPROPERTY() TObjectPtr<AAquilaBoardingBay> DuelDecks[2];
    UPROPERTY() TObjectPtr<AAquilaBoardingWalker> DuelVisitor;
    TOptional<FTransform> DuelFooting[2];
    bool bOnGlacier = false;
    FTransform ExteriorTransform;
    uint64 LastDuelViewFrame = MAX_uint64;
    bool bStartAboard = false;
    EMouseCaptureMode ExteriorCapture = EMouseCaptureMode::NoCapture;
    void EnterDuelBridge();
    void VisitDuelShip(bool bGlacier);
    void StartDuelAboard();
    void RenderDuelBridge(float Seconds);
    UPROPERTY() TObjectPtr<USceneComponent> SignalRoot;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> PortUpgradeGlow;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Planet;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> FriendlyRounds;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> HostileRounds;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> DefensiveBolts;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> HotFragments;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> BurstSprites;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> ShieldShells[2];
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> ShieldMaterials[2];
    UPROPERTY() TObjectPtr<USceneComponent> HullShieldRoots[2];
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> HullShieldMaterials[2];
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> Stars;
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> Materials;
    TSharedPtr<SWidget> Overlay;
    TSharedPtr<SAquilaMouseLook> MouseLook;
    TArray<TSharedPtr<SWidget>> TouchUIRegions;
    bool bTouchLayout = false;
    bool bCommandsVisible = false;
    int32 CommandTab = 2;
    bool bBridge = false;
    bool bDuelMeshShield = true;
    EAquilaCameraTarget CameraTarget = EAquilaCameraTarget::Free;
    bool bObserverPlaced = false;
    FVector ObserverOrigin = FVector::ZeroVector; // Simulation meters: the free eye, or the center of the followed ship.
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
    TArray<FBox> HullParts[2];
    int32 BaseHullParts[2] = {};
    TArray<uint8> MountedStyles[2];
    UPROPERTY() TObjectPtr<UStaticMesh> TurretMeshes[4];
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> TurretLights[4];
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> TurretPlate;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> TurretFrame;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> TurretEdge;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> ShipPlating[8];
    UPROPERTY() TObjectPtr<UStaticMeshComponent> TurretPreviews[4];
    UPROPERTY() TObjectPtr<UStaticMeshComponent> AmmoPreviews[3];
    UPROPERTY() TObjectPtr<UStaticMesh> DefenseMeshes[6];
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> DefensePlates[6];
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> DefenseEdges[6];
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> DefenseLights[6];
    UPROPERTY() TObjectPtr<UStaticMeshComponent> DefensePreviews[6];
    UPROPERTY() TObjectPtr<UStaticMeshComponent> MountedReserve[2];
    int32 MountedDefense[2] = {-1,-1};
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> FittingStars;
    TSharedPtr<SCanvas> MountCanvas;
    TOptional<EAquilaWeapon> PlacementWeapon;
    TOptional<EAquilaWeapon> HoveredWeapon;
    TOptional<EAquilaAmmo> PlacementAmmo;
    bool bInspectPort = false;
    TOptional<FAquilaFiringPreview> FiringPreview;
    TSharedPtr<SAquilaFiringRibbon> FiringRibbon;
    UPROPERTY() TObjectPtr<USceneComponent> PreviewTarget;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> PreviewShield;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> PreviewShieldMaterial;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> PreviewTracers;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> PreviewShotMaterial;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> PreviewSparks;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> PreviewDeflections;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> PreviewCounters;
    bool bFitEnemy = false;
    bool bMoveHere = false;
    int32 SelectedPart = 6;
    static constexpr int32 ArmorPart = -2, ShieldPart = -3;
    FAquilaShieldEnvelope ShieldEnvelopes[2];
    TArray<FAquilaVisualBurst> Bursts;
    uint64 SeenImpact = 0;
    uint32 SeenEncounter = 0;
    FString VisualCampaignId;
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
    void FrameFreeView(FVector Point);
    void MoveObserver(FVector WorldDirection, double RenderDistance);
    void PanByPixels(FVector2D Delta);
    void ZoomObserver(float Steps);
    FRotator ObserverRotation() const;
    void UpdateCamera(float Seconds);
    UStaticMeshComponent* Shape(USceneComponent* Parent, const TCHAR* MeshPath, FVector Position, FVector Size, int32 Material);
    UInstancedStaticMeshComponent* Instances(UMaterialInterface* Material, const TCHAR* MeshPath);
    UStaticMeshComponent* BuildShip(USceneComponent* Parent, bool bEnemy);
    void LoadTurretModels(UMaterialInterface* Glow);
    UStaticMeshComponent* TurretModel(USceneComponent* Parent, EAquilaWeapon Weapon, bool bChip);
    UStaticMeshComponent* DefenseModel(USceneComponent* Parent, EAquilaDefense Type, bool bGlacier);
    UMaterialInterface* ArmorFinish(bool bGlacier, EAquilaArmor Armor) const;
    UStaticMeshComponent* AmmoModel(const AquilaAmmunition::FModel& Style);
    UStaticMeshComponent* FittingMesh(USceneComponent* Parent, UStaticMesh* Mesh, UMaterialInterface* Plate,
        UMaterialInterface* Edge, UMaterialInterface* Light, UMaterialInterface* Chip);
    void FitDefenseModels(int32 Side, const FAquilaShipLoadout& Fit);
    bool DefenseGalleryVisible(EAquilaDefense Type) const;
    static FVector2D DefensePreviewUV(EAquilaDefense Type);
    static FVector2D AmmoPreviewUV(EAquilaAmmo Type);
    TOptional<EAquilaWeapon> AmmoFittingWeapon() const;
    void UpdateTurretPreviews(bool bFitting, float Seconds);
    const FAquilaTurret* FittedTurret() const;
    TOptional<EAquilaWeapon> InspectedWeapon() const;
    FAquilaTurret SampleGun(EAquilaWeapon Weapon) const;
    void ClearFiringSelection();
    void UpdateFiringSelection(bool bFitting, float Seconds);
    void BuildPreviewTarget();
    void RenderFiringPreview(FVector Target, double TargetSpan);
    void AddFiringReadout();
    static FVector2D PreviewTargetUV() { return FVector2D(.5,.18); }
    static FVector2D TurretPreviewUV(EAquilaWeapon Weapon);
    void RefreshHardpoints(const FAquilaDuelLoadout& Fittings);
    const FAquilaShipLoadout* EditingShip() const;
    void EditFitting(TFunctionRef<void(FAquilaShipLoadout&)> Edit);
    void EditFittedTurret(TFunctionRef<void(FAquilaTurret&)> Edit);
    FVector2D FittingPin(int32 Part) const;
    void SelectPart(int32 Part);
    TSharedRef<SWidget> BuildFittingView();
    void BuildRoom();
    void BuildMatchScene();
    void ResetSessionView();
    TSharedRef<SWidget> BuildModeMenu();
    TSharedRef<SWidget> BuildDuelPanel();
    void BuildOverlay();
    void BuildTouchOverlay();
    void RenderCampaign(float Seconds = 0);
    void BuildCombatVisuals();
    void BuildHullShields();
    void SetDuelMeshShield(bool bMesh);
    void RenderShieldShells(const FAquilaCampaign& Campaign, const FVector* Centers, const FQuat* Rotations, bool bMeshShield,
        bool bFitting = false);
    void RenderCombatVisuals(const FAquilaCampaign& Campaign, const FVector& Origin, float Seconds, bool bMeshShield);
    FVector HullContact(int32 Side, FVector RayPoint, FVector Direction, FVector& Normal, bool bShield=false, bool bHullOnly=false) const;
    FVector RenderRoundPosition(const FAquilaCampaign& Campaign, const FAquilaRound& Round, const FVector& Origin, bool bMeshShield) const;
};
