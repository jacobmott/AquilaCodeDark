#include "AquilaFlightPawn.h"
#include "AquilaBoarding.h"
#include "AquilaBoardingBay.h"
#include "AquilaGameMode.h"
#include "SAquilaMouseLook.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ContentStreaming.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SDPIScaler.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Layout/SSafeZone.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UnrealClient.h"

namespace
{
const TCHAR* Cube = TEXT("/Engine/BasicShapes/Cube.Cube");
const TCHAR* Sphere = TEXT("/Engine/BasicShapes/Sphere.Sphere");
const FLinearColor Ink(0.78f, 0.9f, 1.f, 1.f);
}

AAquilaFlightPawn::AAquilaFlightPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Origin"));
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(RootComponent);
    Camera->FieldOfView = 65;
    ShipRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Resolute"));
    ShipRoot->SetupAttachment(RootComponent);
    EnemyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Sentinel"));
    EnemyRoot->SetupAttachment(RootComponent);
    RoomRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Bridge"));
    RoomRoot->SetupAttachment(RootComponent);
    SignalRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ResearchSignal"));
    SignalRoot->SetupAttachment(RootComponent);
}

AAquilaGameMode* AAquilaFlightPawn::Game() const { return GetWorld()->GetAuthGameMode<AAquilaGameMode>(); }

bool AAquilaFlightPawn::CommandEnabled(int32 Id) const
{
    auto* G = Game();
    if (!G || G->Session() != EAquilaSession::Match) return false;
    if (Id == 7 || Id == 8 || Id == 10) return true;
    if (Id >= 13 && Id <= 16) return !bBridge;
    if (!G->bWritable) return false;
    if (Id == 4) return !G->Campaign.ExpeditionActive();
    if (Id == 11) return G->Campaign.CanExpeditionAction();
    if (Id == 12) return G->Campaign.CanUpgradePortTurret();
    return true;
}

void AAquilaFlightPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindKey(EKeys::H, IE_Pressed, this, &AAquilaFlightPawn::ToggleCommands);
    PlayerInputComponent->BindTouch(IE_Pressed, this, &AAquilaFlightPawn::BeginCameraTouch).bConsumeInput = false;
    PlayerInputComponent->BindTouch(IE_Released, this, &AAquilaFlightPawn::EndCameraTouch).bConsumeInput = false;
}

void AAquilaFlightPawn::ToggleCommands()
{
    if (!Game() || Game()->Session() != EAquilaSession::Match) return;
    bCommandsVisible = !bCommandsVisible;
    TouchWalk = FVector2D::ZeroVector;
    bTouching = false;
    UpdateCursor();
    if (!bCommandsVisible && FSlateApplication::IsInitialized())
        FSlateApplication::Get().SetUserFocusToGameViewport(0);
}

void AAquilaFlightPawn::UpdateCursor()
{
    if (auto* PC = Cast<APlayerController>(GetController()))
        PC->bShowMouseCursor = (Game() && Game()->Session() != EAquilaSession::Match) || bCommandsVisible || (!bBridge && !bTouchLayout);
}

void AAquilaFlightPawn::RunControl(int32 Id)
{
    if (Id == 7) ToggleBridge();
    else if (Id == 10) ToggleCommands();
    else if (Id == 13) Follow(EAquilaCameraTarget::Resolute);
    else if (Id == 14)
    {
        if (CameraTarget != EAquilaCameraTarget::Free) ReleaseFollow();
        else bTouchRotate = !bTouchRotate;
    }
    else if (Id == 15 || Id == 16) ZoomObserver(Id == 15 ? 1.f : -1.f);
    else if (auto* G = Game()) G->Command(Id);
}

UStaticMeshComponent* AAquilaFlightPawn::Shape(USceneComponent* Parent, const TCHAR* MeshPath, FVector Position, FVector Size, int32 Material)
{
    auto* Mesh = NewObject<UStaticMeshComponent>(this);
    Mesh->SetupAttachment(Parent);
    Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, MeshPath));
    Mesh->SetMaterial(0,Parent==RoomRoot && Material<2 ?
        AquilaHulls::Surface(false,Material==0 ? TEXT("Island") : TEXT("Hardware")) : Materials[Material].Get());
    if (Parent == ShipRoot || Parent == EnemyRoot)
    {
        HullParts[Parent == EnemyRoot ? 1 : 0].Emplace(Position - Size * 0.5, Position + Size * 0.5);
        Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
        Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
        Mesh->SetGenerateOverlapEvents(false);
        Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    else Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCastShadow(false);
    Mesh->RegisterComponent();
    Mesh->SetRelativeLocation(Position);
    Mesh->SetRelativeScale3D(Size / 100);
    return Mesh;
}

UInstancedStaticMeshComponent* AAquilaFlightPawn::Instances(UMaterialInterface* Material, const TCHAR* MeshPath)
{
    auto* Mesh = NewObject<UInstancedStaticMeshComponent>(this);
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, MeshPath));
    Mesh->SetMaterial(0, Material);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCastShadow(false);
    Mesh->RegisterComponent();
    return Mesh;
}

UStaticMeshComponent* AAquilaFlightPawn::BuildShip(USceneComponent* Parent, bool bEnemy)
{
    const bool bFlight=Parent==ShipRoot || Parent==EnemyRoot;
    auto* Hull=NewObject<UStaticMeshComponent>(this);
    Hull->SetupAttachment(Parent);
    Hull->SetStaticMesh(AquilaHulls::LoadMesh(bEnemy));
    Hull->SetRelativeTransform(AquilaHulls::Layout(bEnemy).MeshToShip);
    Hull->SetGenerateOverlapEvents(false);
    Hull->SetCollisionResponseToAllChannels(ECR_Ignore);
    Hull->SetCollisionResponseToChannel(ECC_Visibility,ECR_Overlap);
    Hull->SetCollisionEnabled(bFlight ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
    Hull->SetCastShadow(false);
    Hull->RegisterComponent();
    if (bFlight)
    {
        HullBodies[int32(bEnemy)]=Hull;
        HullParts[int32(bEnemy)].Add(Hull->GetStaticMesh()->GetBoundingBox().TransformBy(Hull->GetRelativeTransform()));
    }
    return Hull;
}

void AAquilaFlightPawn::BuildRoom()
{
    Shape(RoomRoot, Cube, FVector(0, 0, -20), FVector(2400, 1700, 40), 1);
    Shape(RoomRoot, Cube, FVector(-1100, 0, 280), FVector(30, 1700, 560), 0);
    for (int32 Side : {-1, 1})
    {
        Shape(RoomRoot, Cube, FVector(0, Side * 850, 280), FVector(2400, 35, 560), 0);
        Shape(RoomRoot, Cube, FVector(0, Side * 800, 8), FVector(2150, 8, 8), 2);
        Shape(RoomRoot, Cube, FVector(970, Side * 780, 280), FVector(40, 80, 560), 1);
        Shape(RoomRoot, Cube, FVector(250, Side * 460, 60), FVector(380, 160, 120), 1);
        Shape(RoomRoot, Cube, FVector(250, Side * 460, 123), FVector(340, 140, 4), 2);
    }
    Shape(RoomRoot, Cube, FVector(950, 0, 35), FVector(50, 1700, 70), 1);
    Shape(RoomRoot, Cube, FVector(950, 0, 520), FVector(50, 1700, 60), 1);
    Shape(RoomRoot, Cube, FVector(-400, 0, 50), FVector(200, 180, 100), 0);
}

void AAquilaFlightPawn::RefreshHardpoints(const FAquilaDuelLoadout& Fittings)
{
    for (int32 Side = 0; Side < 2; ++Side)
    {
        const auto& Fit = Fittings.ForShip(Side != 0);
        uint8 Desired[FAquilaTurret::MountCount] = {};
        const int32 Count = Fit.Rack ? Fit.Rack->Stock.Num() : Fit.StarterCount();
        for (int32 I = 0; I < Count; ++I)
        {
            const auto T = Fit.Rack ? Fit.Rack->Stock[I] : Fit.Starter(I);
            if (T.Mount != INDEX_NONE) Desired[T.Mount] = 1 + uint8(T.Weapon) + (T.bIntercept ? 4 : 0);
        }
        auto& Current = MountedStyles[Side];
        const int32 Defense = int32(Fit.Armor) | (Fit.bShield ? 4 : 0) | (Fit.bReserveCapacitor ? 8 : 0);
        if (MountedDefense[Side] == Defense && Current.Num() == UE_ARRAY_COUNT(Desired) &&
            !FMemory::Memcmp(Current.GetData(), Desired, sizeof(Desired))) continue;
        MountedDefense[Side] = Defense;
        Current.Reset();
        Current.Append(Desired, UE_ARRAY_COUNT(Desired));
        auto* Root = (Side ? EnemyRoot : ShipRoot).Get();
        const auto Old = Root->GetAttachChildren();
        for (USceneComponent* Part : Old)
            if (Part->ComponentHasTag(TEXT("FittedTurret")) || Part->ComponentHasTag(TEXT("FittedDefense"))) Part->DestroyComponent();
        HullParts[Side].SetNum(BaseHullParts[Side]);
        FitDefenseModels(Side, Fit);
        for (int32 I = 0; I < Count; ++I)
        {
            const auto T = Fit.Rack ? Fit.Rack->Stock[I] : Fit.Starter(I);
            if (T.Mount == INDEX_NONE) continue;
            auto* Model = TurretModel(Root, T.Weapon, T.bIntercept);
            Model->SetRelativeTransform(T.MountedPose(Side!=0));
            Model->ComponentTags.Add(TEXT("FittedTurret"));
            Model->ComponentTags.Add(AquilaHulls::GunSocket(T.Mount));
            HullParts[Side].Add(Model->GetStaticMesh()->GetBoundingBox().TransformBy(Model->GetRelativeTransform()));
        }
        if (ShieldShells[Side])
        {
            const auto Shell = FAquilaShieldEnvelope::Fit(HullParts[Side]);
            ShieldEnvelopes[Side] = Shell;
            ShieldShells[Side]->SetRelativeScale3D(Shell.Radii / 50.);
            const FVector Axes = Shell.Radii / Shell.Radii.GetMax();
            ShieldMaterials[Side]->SetVectorParameterValue(TEXT("Axes"), FLinearColor(Axes.X, Axes.Y, Axes.Z, 1));
            if (HullShieldMaterials[Side]) HullShieldMaterials[Side]->SetScalarParameterValue(TEXT("Reach"), Shell.Radii.GetMax());
        }
    }
}

void AAquilaFlightPawn::BeginPlay()
{
    Super::BeginPlay();
    if (Game()) AddTickPrerequisiteActor(Game());
    UMaterialInterface* Hull = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Foundation/M_Hull.M_Hull"));
    UMaterialInterface* Glow = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Foundation/M_Glow.M_Glow"));
    if (!Hull || !Glow)
    {
        UE_LOG(LogTemp, Error, TEXT("Aquila content missing. Run scripts/native.ps1 -Action Prepare after Build."));
        Hull = Glow = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    }
    const FLinearColor Colors[] = {FLinearColor(0.24f,0.32f,0.4f), FLinearColor(0.06f,0.1f,0.15f), FLinearColor(0.04f,0.65f,0.85f),
        FLinearColor(1,0.63f,0.16f), FLinearColor(1,0.13f,0.25f), FLinearColor(2.8f,0.72f,0.13f), FLinearColor(0.035f,0.12f,0.21f), FLinearColor(0.38f,0.56f,0.8f)};
    for (int32 I = 0; I < 8; ++I)
    {
        auto* M = UMaterialInstanceDynamic::Create((I >= 2 && I != 6) ? Glow : Hull, this);
        M->SetVectorParameterValue(TEXT("Color"), Colors[I]);
        M->SetScalarParameterValue(TEXT("Glow"), I == 7 ? 1.0 : 2.0);
        Materials.Add(M);
    }
    LoadTurretModels(Glow);
    BuildShip(ShipRoot, false);
    BuildShip(EnemyRoot, true);
    const FTransform Upgrade=FAquilaTurret{6,EAquilaWeapon::TwinTurrets}.MountedPose(false);
    PortUpgradeGlow = Shape(ShipRoot,Cube,Upgrade.TransformPosition(FVector(-23,0,26)),FVector(4,6,1),2);
    PortUpgradeGlow->SetRelativeRotation(Upgrade.GetRotation());
    for (int32 I = 0; I < 2; ++I) BaseHullParts[I] = HullParts[I].Num();
    RefreshHardpoints(Game()->Campaign.Loadout);
    FriendlyRounds = Instances(Materials[3], Sphere);
    HostileRounds = Instances(Materials[4], Sphere);
    DefensiveBolts = Instances(Materials[2], Sphere);
    BuildCombatVisuals();
    Stars = Instances(Materials[7], Sphere);
    FRandomStream Random(7142);
    for (int32 I = 0; I < 360; ++I)
    {
        const FVector Location = Random.VRand() * 65000;
        Stars->AddInstance(FTransform(FRotator::ZeroRotator, Location, FVector(Random.FRandRange(0.6,1.8))));
    }
    for (int32 I = 0; I < 2; ++I)
    {
        auto* Light = GetWorld()->SpawnActor<ADirectionalLight>();
        Light->GetLightComponent()->SetMobility(EComponentMobility::Movable);
        Light->SetActorRotation(I == 0 ? FRotator(-35,-35,0) : FRotator(-20,145,0));
        Light->GetLightComponent()->SetIntensity(I == 0 ? 4 : 1.5);
        Light->GetLightComponent()->SetLightColor(I == 0 ? FLinearColor(0.78f,0.87f,1) : FLinearColor(0.25f,0.45f,1));
        Light->GetLightComponent()->SetCastShadows(false);
        Light->FindComponentByClass<UDirectionalLightComponent>()->SetForwardShadingPriority(I == 0 ? 1 : 0);
    }
    if (auto* PC = Cast<APlayerController>(GetController()))
    {
        PC->bEnableTouchEvents = true;
        FInputModeGameAndUI Input;
        Input.SetHideCursorDuringCapture(false);
        Input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Input);
    }
    BuildOverlay();
    bStartAboard=Game() && Game()->Session()==EAquilaSession::Simulator && FParse::Param(FCommandLine::Get(),TEXT("AquilaDuelAboard"));
    FString Demo;
    if (FParse::Value(FCommandLine::Get(), TEXT("AquilaDemo="), Demo))
    {
        bBridge = Demo == TEXT("bridge");
        if (Demo == TEXT("combat") && Game()) Game()->Command(4);
    }
    UpdateCursor();
    UpdateCamera(0);
    RenderCampaign();
}

void AAquilaFlightPawn::ToggleBridge()
{
    if (Game() && Game()->Session()==EAquilaSession::Simulator)
    {
        EnterDuelBridge();
        return;
    }
    if (!Game() || Game()->Session() != EAquilaSession::Match) return;
    TouchWalk = FVector2D::ZeroVector;
    bTouching = false;
    bBridge = !bBridge;
    UpdateCursor();
}

void AAquilaFlightPawn::BuildMatchScene()
{
    BuildRoom();
    Shape(SignalRoot,Sphere,FVector::ZeroVector,FVector(120),2);
    Shape(SignalRoot,Cube,FVector::ZeroVector,FVector(440,25,25),1);
    Shape(SignalRoot,Cube,FVector::ZeroVector,FVector(25,440,25),1);
    FRandomStream Debris(7142);
    for (int32 I=0; I<18; ++I)
    {
        auto* Piece = Shape(SignalRoot,Cube,Debris.VRand()*Debris.FRandRange(500,1600),
            FVector(Debris.FRandRange(50,200),Debris.FRandRange(25,70),Debris.FRandRange(20,60)),1);
        Piece->SetRelativeRotation(FRotator(Debris.FRandRange(0,180),Debris.FRandRange(0,180),30));
    }
    Planet = Shape(RootComponent, Sphere, FVector(18000,0,-3000), FVector(12000), 6);
    Planet->SetCollisionResponseToAllChannels(ECR_Ignore);
    Planet->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    Planet->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    if (auto* Surface = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Foundation/M_Planet.M_Planet"))) Planet->SetMaterial(0,Surface);
}

void AAquilaFlightPawn::ResetSessionView()
{
    LastDuelViewFrame=MAX_uint64;
    bOnGlacier=false;
    for (auto& Footing : DuelFooting) Footing.Reset();
    for (auto& Deck : DuelDecks)
        if (Deck) { Deck->Destroy(); Deck=nullptr; }
    Bursts.Reset();
    SeenImpact = 0;
    bBridge = bCommandsVisible = bObserverPlaced = bTouching = false;
    PlacementWeapon.Reset();
    bFitEnemy = false;
    bMoveHere = false;
    CameraTarget = EAquilaCameraTarget::Free;
    OrbitDistance = 4800;
    OrbitPitch = 38;
    TouchWalk = FVector2D::ZeroVector;
    if (Planet)
    {
        const bool bMatch = Game()->Session() == EAquilaSession::Match;
        Planet->SetVisibility(bMatch);
        Planet->SetCollisionEnabled(bMatch ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
    }
    UpdateCursor();
    if (FSlateApplication::IsInitialized()) FSlateApplication::Get().SetUserFocusToGameViewport(0);
}

void AAquilaFlightPawn::RenderCampaign(float Seconds)
{
    auto* G = Game();
    if (!G) return;
    const auto& C = G->Campaign;
    if (bBridge && C.Hull <= 0 && C.Impacts.ContainsByPredicate([this](const FAquilaImpact& Hit)
        { return Hit.Serial > SeenImpact && Hit.bDestroyed && !Hit.bTargetEnemy; }))
    {
        ToggleBridge();
        OrbitDistance = 3300;
        FrameFreeView(C.ShipPosition);
        UpdateCamera(0);
    }
    const FVector Origin = DuelVisitor && bOnGlacier ? C.EnemyPosition : (bBridge || DuelVisitor ? C.ShipPosition : ObserverOrigin);
    const bool bMatch = G->Session() == EAquilaSession::Match;
    const bool bSimulator = G->Session() == EAquilaSession::Simulator;
    const bool bFitting = G->bFittingOpen;
    RefreshHardpoints(C.Loadout);
#if PLATFORM_WINDOWS
    // The arena uses a miniature hull. Fitting and the human-scale bridge both need its close-up maps.
    const int32 CloseShip=bFitting ? int32(bFitEnemy) : DuelVisitor ? int32(bOnGlacier) : INDEX_NONE;
    for (int32 Side=0; Side<2; ++Side)
    {
        auto* Hull=HullBodies[Side].Get();
        const bool bClose=Side==CloseShip;
        if (Hull->IsMipStreamingForced()==bClose) continue;
        Hull->bForceMipStreaming=bClose;
        IStreamingManager::Get().NotifyPrimitiveUpdated(Hull);
        if (bClose) Hull->PrestreamTextures(3.f,false);
    }
#endif
    if (bSimulator && !HullShieldRoots[0]) BuildHullShields();
    const bool bMeshShield = bSimulator && bDuelMeshShield && HullShieldRoots[0];
    if (bMatch && !Planet) BuildMatchScene();
    SignalRoot->SetVisibility(bMatch && C.ExpeditionActive(),true);
    SignalRoot->SetRelativeLocation(((C.Expedition == EAquilaExpedition::Salvage ? C.EnemyPosition : C.SignalPosition) - Origin) * ExteriorScale);
    ShipRoot->SetVisibility(bFitting ? !bFitEnemy : !bBridge && C.Hull > 0, true);
    ShipRoot->SetRelativeLocation((C.ShipPosition - Origin) * ExteriorScale);
    PortUpgradeGlow->SetVisibility(C.PortTurretLevel > 0 && !bBridge && C.Hull > 0);
    RoomRoot->SetVisibility(bBridge, true);
    EnemyRoot->SetVisibility(bFitting ? bFitEnemy : C.bBattle && C.EnemyHull > 0, true);
    ShipRoot->SetRelativeRotation(FRotator(0, FMath::RadiansToDegrees(C.Heading), 0));
    EnemyRoot->SetRelativeLocation((C.EnemyPosition - Origin) * ExteriorScale);
    EnemyRoot->SetRelativeRotation(FRotator(0, 180, 0));
    if (bMatch) Planet->SetRelativeLocation((FVector(180000,0,-30000) - Origin) * ExteriorScale);
    Stars->SetRelativeLocation(Camera->GetRelativeLocation());
    FriendlyRounds->ClearInstances();
    HostileRounds->ClearInstances();
    DefensiveBolts->ClearInstances();
    UpdateTurretPreviews(bFitting, Seconds);
    for (int32 Side = 0; Side < 2; ++Side)
        if (auto* Cell = MountedReserve[Side].Get())
        {
            const int32 Slot = Cell->GetMaterialIndex(TEXT("Emitter"));
            auto* Light = !bFitting && C.DuelStats && C.DuelStats->bReserveSpent[Side] ?
                Materials[1].Get() : DefenseLights[int32(EAquilaDefense::ReserveCapacitor)].Get();
            if (Cell->GetMaterial(Slot) != Light) Cell->SetMaterial(Slot, Light);
        }
    for (auto* Effect : {BurstSprites.Get(), HotFragments.Get()}) if (Effect) Effect->SetVisibility(!bFitting);
    if (bFitting)
    {
        const FVector Centers[] = {ShipRoot->GetRelativeLocation(), EnemyRoot->GetRelativeLocation()};
        const FQuat Rotations[] = {ShipRoot->GetRelativeRotation().Quaternion(), EnemyRoot->GetRelativeRotation().Quaternion()};
        RenderShieldShells(C, Centers, Rotations, bMeshShield, true);
        return;
    }
    if (C.DuelStats)
    {
        for (const auto& Bolt : C.DuelStats->ScreenShots)
            DefensiveBolts->AddInstance(FTransform((Bolt.Position - Bolt.Origin).Rotation(),
                (Bolt.Position - Origin) * ExteriorScale, FVector(.95, .22, .22)));
        for (const auto& Flash : C.DuelStats->Flashes)
        {
            const FVector Point = (Flash.Position - Origin) * ExteriorScale;
            const double Fade = 1. - Flash.Age / .8;
            DefensiveBolts->AddInstance(FTransform(FQuat::Identity, Point, FVector(1.6, .16, .16) * Fade));
            DefensiveBolts->AddInstance(FTransform(FQuat::Identity, Point, FVector(.16, .16, 1.3) * Fade));
        }
    }
    for (const auto& R : C.Rounds)
    {
        const auto Weapon = R.Turret == INDEX_NONE ? C.Loadout.ForShip(R.bEnemy).Weapon : R.Weapon;
        FVector Size(0.6, 0.055, 0.055);
        if (Weapon == EAquilaWeapon::PlasmaCaster) Size = FVector(0.55, 0.38, 0.38);
        if (Weapon == EAquilaWeapon::PulseRepeater) Size = FVector(0.95, 0.14, 0.14);
        const FTransform Transform(R.Velocity.Rotation(), RenderRoundPosition(C, R, Origin, bMeshShield), Size);
        (R.bEnemy ? HostileRounds : FriendlyRounds)->AddInstance(Transform);
    }
    RenderCombatVisuals(C, Origin, (G->bPaused || G->bFittingOpen) ? 0.f : Seconds, bMeshShield);
}

void AAquilaFlightPawn::Tick(float Seconds)
{
    Super::Tick(Seconds);
    if (DuelVisitor)
    {
        RenderDuelBridge(Seconds);
        return;
    }
    auto* G = Game();
    auto* PC = Cast<APlayerController>(GetController());
    if (!G || !PC) return;
    if (bStartAboard)
    {
        bStartAboard=false;
        StartDuelAboard();
        return;
    }
    // A restored campaign can arrive after the pawn's BeginPlay. Seed the observer only from the ready world.
    if (!bObserverPlaced && G->HasActorBegunPlay())
    {
        FVector FramingPoint = G->Campaign.ShipPosition;
        OrbitYaw = FRotator::NormalizeAxis(32.f + FMath::RadiansToDegrees(G->Campaign.Heading));
        if (G->Session() == EAquilaSession::Menu)
        {
            FramingPoint += FVector(6500, 0, 0);
            OrbitDistance = 2900;
            OrbitYaw = 110;
            OrbitPitch = 23;
        }
        else if (G->Session() == EAquilaSession::Simulator)
        {
            FramingPoint = (G->Campaign.ShipPosition + G->Campaign.EnemyPosition) * 0.5;
            OrbitYaw = 100;
        }
        FrameFreeView(FramingPoint);
        bObserverPlaced = true;
        UpdateCursor();
    }
    AliveTime += Seconds;
    if (PC->WasInputKeyJustPressed(EKeys::Escape)) G->Command(8);
    if (PC->WasInputKeyJustPressed(EKeys::C) ||
        (G->Session()==EAquilaSession::Simulator && PC->WasInputKeyJustPressed(EKeys::Tab))) ToggleBridge();
    if (G->Session()==EAquilaSession::Simulator && PC->WasInputKeyJustPressed(EKeys::N)) SwitchDuelShip();
    if (DuelVisitor) return;
    if (PC->WasInputKeyJustPressed(EKeys::E) && CommandEnabled(11)) G->Command(11);
    if (PC->WasInputKeyJustPressed(EKeys::U) && CommandEnabled(12)) G->Command(12);
    if (bTouchLayout) UpdateTouchCamera(PC);
    if (PC->WasInputKeyJustPressed(EKeys::Home) && !bBridge && G->Session() != EAquilaSession::Menu) Follow(EAquilaCameraTarget::Resolute);
    const double Forward = int32(PC->IsInputKeyDown(EKeys::W)) - int32(PC->IsInputKeyDown(EKeys::S));
    const double Side = int32(PC->IsInputKeyDown(EKeys::D)) - int32(PC->IsInputKeyDown(EKeys::A));
    if (!bBridge && G->Session() != EAquilaSession::Menu && !G->bFittingOpen)
    {
        const double Up = int32(PC->IsInputKeyDown(EKeys::R)) - int32(PC->IsInputKeyDown(EKeys::F));
        const double Speed = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift) ? 4.0 : 1.0;
        const FVector Flight = ObserverRotation().RotateVector(FVector(Forward, Side, 0)) + FVector::UpVector * Up;
        MoveObserver(Flight.GetClampedToMaxSize(1), OrbitDistance * 0.8 * Speed * FMath::Min(Seconds, 0.1f));
    }
    else if (bBridge && !G->bPaused)
    {
        const FRotator Heading(0,BridgeYaw,0);
        FVector Movement = Heading.RotateVector(FVector(Forward + TouchWalk.X, Side + TouchWalk.Y, 0)).GetClampedToMaxSize(1);
        const FVector Proposed = Eye + Movement * 500 * FMath::Min(Seconds, 0.05f);
        const bool bConsole = FMath::Abs(Proposed.X - 250) < 240 && FMath::Abs(FMath::Abs(Proposed.Y) - 460) < 130;
        const bool bChair = FMath::Abs(Proposed.X + 400) < 150 && FMath::Abs(Proposed.Y) < 140;
        if (!bConsole && !bChair)
        {
            Eye.X = FMath::Clamp(Proposed.X, -1030.0, 870.0);
            Eye.Y = FMath::Clamp(Proposed.Y, -770.0, 770.0);
        }
    }
    else TouchWalk = FVector2D::ZeroVector;
    UpdateCamera(Seconds);
    RenderCampaign(Seconds);
    FString Capture;
    if (!bCaptured && AliveTime > 12 && FParse::Value(FCommandLine::Get(), TEXT("AquilaCapture="), Capture))
    {
        FScreenshotRequest::RequestScreenshot(Capture, true, false);
        bCaptured = true;
    }
    if (bCaptured && AliveTime > 15 && FParse::Param(FCommandLine::Get(), TEXT("AquilaExitAfterCapture")))
        PC->ConsoleCommand(TEXT("quit"));
}

void AAquilaFlightPawn::BuildOverlay()
{
    if (!GEngine || !GEngine->GameViewport) return;
    bTouchLayout = PLATFORM_ANDROID || PLATFORM_IOS || FParse::Param(FCommandLine::Get(), TEXT("AquilaTouchUI"));
    if (bTouchLayout) { BuildTouchOverlay(); return; }
    auto Text = [](TAttribute<FText> Value, int32 Size, FName Weight = "Regular")
    {
        return SNew(STextBlock).Text(Value).Font(FCoreStyle::GetDefaultFontStyle(Weight, Size))
            .ColorAndOpacity(Ink).AutoWrapText(true);
    };
    auto Heading = [](const FText& Value)
    {
        return SNew(STextBlock).Text(Value).Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
            .ColorAndOpacity(FLinearColor(0.39f, 0.81f, 0.87f));
    };
    auto Button = [this](TAttribute<FText> Label, int32 Id)
    {
        return SNew(SButton).HAlign(HAlign_Center).VAlign(VAlign_Center).ContentPadding(FMargin(12, 7))
            .ButtonColorAndOpacity(FLinearColor(0.12f, 0.23f, 0.31f))
            .IsEnabled_Lambda([this, Id]() { return CommandEnabled(Id); })
            .OnClicked_Lambda([this, Id]() { RunControl(Id); return FReply::Handled(); })
            [SNew(STextBlock).Text(Label).Font(FCoreStyle::GetDefaultFontStyle("Regular", 15)).ColorAndOpacity(Ink)];
    };

    auto Expedition = SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight()[Heading(INVTEXT("Expedition"))]
        + SVerticalBox::Slot().AutoHeight().Padding(0, 6)[Text(TAttribute<FText>::CreateLambda([this]() {
            const auto* G = Game();
            if (!G) return FText::GetEmpty();
            if (G->Campaign.Expedition == EAquilaExpedition::Battle) return INVTEXT("Secure the signal.");
            return FText::FromString(G->Campaign.ExpeditionStatus());
        }), 17)]
        + SVerticalBox::Slot().AutoHeight()[Text(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() ? Game()->Campaign.UpgradeStatus() : FString());
        }), 14)]
        + SVerticalBox::Slot().AutoHeight().Padding(0, 10, 0, 0)[SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1).Padding(0, 0, 6, 0)[Button(TAttribute<FText>::CreateLambda([this]() {
                return FText::FromString(Game() ? Game()->Campaign.ExpeditionActionLabel() : FString());
            }), 11)]
            + SHorizontalBox::Slot().FillWidth(1)[Button(INVTEXT("Refit port turret / 100"), 12)]];
    auto Helm = SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight()[Heading(INVTEXT("Navigation"))]
        + SVerticalBox::Slot().AutoHeight().Padding(0, 6, 0, 10)[Text(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() ? Game()->Campaign.OrderStatus() : FString());
        }), 15)]
        + SVerticalBox::Slot().AutoHeight()[SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1).Padding(0, 0, 6, 0)[Button(INVTEXT("Hold"), 0)]
            + SHorizontalBox::Slot().FillWidth(1)[Button(INVTEXT("Approach Vesper"), 1)]]
        + SVerticalBox::Slot().AutoHeight().Padding(0, 5, 0, 0)[SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1).Padding(0, 0, 6, 0)[Button(INVTEXT("Keep range"), 2)]
            + SHorizontalBox::Slot().FillWidth(1)[Button(INVTEXT("Withdraw"), 3)]];
    auto Tactical = SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight()[Heading(INVTEXT("Tactical"))]
        + SVerticalBox::Slot().AutoHeight().Padding(0, 6)[Text(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() ? Game()->Campaign.CombatStatus() : FString());
        }), 17)]
        + SVerticalBox::Slot().AutoHeight()[Text(TAttribute<FText>::CreateLambda([this]() {
            const auto* G = Game();
            if (!G) return FText::GetEmpty();
            const auto& C = G->Campaign;
            const TCHAR* Ammo = C.Loadout.Resolute.bPiercing ? TEXT("Piercing rounds") : TEXT("Standard rounds");
            if (!C.bBattle) return FText::FromString(Ammo);
            return FText::FromString(FString::Printf(TEXT("%s  |  Glacier Tongue shield %.0f / hull %.0f  |  %d in flight"),
                Ammo, C.EnemyShield, C.EnemyHull, C.Rounds.Num()));
        }), 14)]
        + SVerticalBox::Slot().AutoHeight().Padding(0, 10, 0, 0)[SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1).Padding(0, 0, 6, 0)[Button(TAttribute<FText>::CreateLambda([this]() {
                return Game() && Game()->Campaign.bBattle ? INVTEXT("Reset practice") : INVTEXT("Start practice");
            }), 4)]
            + SHorizontalBox::Slot().FillWidth(1)[Button(TAttribute<FText>::CreateLambda([this]() {
                return Game() && Game()->Campaign.bHoldFire ? INVTEXT("Resume fire") : INVTEXT("Hold fire");
            }), 5)]]
        + SVerticalBox::Slot().AutoHeight().Padding(0, 5, 0, 0)[Button(TAttribute<FText>::CreateLambda([this]() {
            return Game() && Game()->Campaign.Loadout.Resolute.bPiercing ? INVTEXT("Fit standard rounds") : INVTEXT("Fit piercing rounds");
        }), 6)];
    auto View = SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight()[Heading(INVTEXT("Ship controls"))]
        + SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 0)[SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1).Padding(0, 0, 6, 0)[Button(TAttribute<FText>::CreateLambda([this]() {
                return bBridge ? INVTEXT("Exterior view") : INVTEXT("Enter bridge");
            }), 7)]
            + SHorizontalBox::Slot().FillWidth(1)[Button(TAttribute<FText>::CreateLambda([this]() {
                return Game() && Game()->bPaused ? INVTEXT("Resume") : INVTEXT("Pause");
            }), 8)]]
        + SVerticalBox::Slot().AutoHeight().Padding(0, 5, 0, 0)[SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1).Padding(0, 0, 6, 0)[Button(INVTEXT("Find Delta Tree"), 13)]
            + SHorizontalBox::Slot().FillWidth(1)[Button(INVTEXT("Save voyage"), 9)]];

    auto Walk = SNew(SHorizontalBox);
    const TCHAR* Labels[] = {TEXT("Forward"), TEXT("Back"), TEXT("Left"), TEXT("Right")};
    const FVector2D Directions[] = {FVector2D(1,0), FVector2D(-1,0), FVector2D(0,-1), FVector2D(0,1)};
    for (int32 I = 0; I < 4; ++I)
    {
        const FVector2D Direction = Directions[I];
        Walk->AddSlot().FillWidth(1).Padding(2, 0)[SNew(SButton).HAlign(HAlign_Center).ContentPadding(6)
            .ButtonColorAndOpacity(FLinearColor(0.12f, 0.23f, 0.31f))
            .OnPressed_Lambda([this, Direction]() { TouchWalk = Direction; })
            .OnReleased_Lambda([this]() { TouchWalk = FVector2D::ZeroVector; })
            [Text(FText::FromString(Labels[I]), 14)]];
    }
    const FSlateBrush* Solid = FCoreStyle::Get().GetBrush("WhiteBrush");
    auto Panel = SNew(SBorder).Visibility(EVisibility::Visible)
        .BorderImage(Solid).BorderBackgroundColor(FLinearColor(0.12f, 0.37f, 0.43f)).Padding(1)
        [SNew(SBorder).BorderImage(Solid).BorderBackgroundColor(FLinearColor(0.012f, 0.027f, 0.043f, 0.96f)).Padding(24)
        [SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[Text(INVTEXT("Delta Tree"), 27, "Bold")]
                + SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center).Padding(20, 0)[Text(TAttribute<FText>::CreateLambda([this]() {
                    const auto* G = Game();
                    if (!G) return FText::GetEmpty();
                    return FText::FromString(FString::Printf(TEXT("Shield %.0f   Hull %.0f   |   %s"),
                        G->Campaign.Shield, G->Campaign.Hull, G->bPaused ? TEXT("Paused") : TEXT("Auto helm")));
                }), 16)]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0,0,8,0)[SNew(SButton)
                    .Text(INVTEXT("Main menu")).OnClicked_Lambda([this]() {
                        if (Game()->ReturnToMenu()) ResetSessionView();
                        return FReply::Handled();
                    })]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[Button(INVTEXT("Hide   [H]"), 10)]]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 18, 0, 0)[SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1).Padding(0, 0, 16, 0)[SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight()[Expedition]
                    + SVerticalBox::Slot().AutoHeight().Padding(0, 22, 0, 0)[Helm]]
                + SHorizontalBox::Slot().FillWidth(1).Padding(16, 0, 0, 0)[SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight()[Tactical]
                    + SVerticalBox::Slot().AutoHeight().Padding(0, 22, 0, 0)[View]]]
            + SVerticalBox::Slot().AutoHeight()[SNew(SBox).Padding(0, 12, 0, 0)
                .Visibility_Lambda([this]() { return bBridge ? EVisibility::Visible : EVisibility::Collapsed; })[Walk]]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 18, 0, 0)[Text(TAttribute<FText>::CreateLambda([this]() {
                if (bBridge) return INVTEXT("Bridge   ·   WASD walk   ·   Drag to look   ·   C returns to the observer");
                if (CameraTarget == EAquilaCameraTarget::Free)
                    return INVTEXT("W/S fly where you look   ·   A/D strafe   ·   R/F rise or lower   ·   Drag to look   ·   Wheel zoom   ·   Click a ship to follow");
                return FText::Format(INVTEXT("Following {0}   ·   Drag to orbit   ·   Wheel zoom   ·   Pan or click empty space to release"),
                    CameraTarget == EAquilaCameraTarget::Resolute ? INVTEXT("Delta Tree") : INVTEXT("Glacier Tongue"));
            }), 14)]
            + SVerticalBox::Slot().AutoHeight().Padding(0, 4, 0, 0)[Text(TAttribute<FText>::CreateLambda([this]() {
                return FText::FromString(Game() ? Game()->SaveStatus : FString());
            }), 13)]]];

    // Only the panel collapses. Open space still receives orbit and bridge look across the viewport.
    Overlay = SNew(SOverlay).Visibility_Lambda([this]() { return DuelVisitor ? EVisibility::Collapsed : EVisibility::SelfHitTestInvisible; })
        + SOverlay::Slot()[SAssignNew(MouseLook, SAquilaMouseLook)
            .Visibility_Lambda([this]() { return Game() && Game()->Session() != EAquilaSession::Menu ? EVisibility::Visible : EVisibility::Collapsed; })
            .OnDrag_UObject(this, &AAquilaFlightPawn::DragDesktopCamera)
            .OnPick_UObject(this, &AAquilaFlightPawn::PickShip).OnZoom_UObject(this, &AAquilaFlightPawn::ZoomObserver)]
        + SOverlay::Slot()[SNew(SScaleBox)
            .Visibility_Lambda([this]() { return bCommandsVisible ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed; })
            .Stretch(EStretch::ScaleToFit).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
            [SNew(SBox).WidthOverride(1600).HeightOverride(900).Visibility(EVisibility::SelfHitTestInvisible)
                .HAlign(HAlign_Center).VAlign(VAlign_Center)[SNew(SBox).WidthOverride(1080)[Panel]]]]
        + SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Center).Padding(64,0,0,0)[BuildModeMenu()]
        + SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Bottom).Padding(12)[BuildDuelPanel()]
        + SOverlay::Slot()[BuildFittingView()];
    GEngine->GameViewport->AddViewportWidgetContent(Overlay.ToSharedRef(), 10);
}

void AAquilaFlightPawn::BuildTouchOverlay()
{
    auto Text = [](TAttribute<FText> Value, int32 Size)
    {
        return SNew(STextBlock).Text(Value).Font(FCoreStyle::GetDefaultFontStyle("Bold", Size))
            .ColorAndOpacity(FLinearColor(0.94f,0.97f,1.f,1.f)).AutoWrapText(true)
            .ShadowOffset(FVector2D(1.5f,1.5f)).ShadowColorAndOpacity(FLinearColor(0,0,0,1));
    };
    auto Button = [this,Text](TAttribute<FText> Label, int32 Id)
    {
        return SNew(SBox).MinDesiredHeight(54)
            [SNew(SButton).HAlign(HAlign_Center).VAlign(VAlign_Center).ContentPadding(FMargin(12,6))
                .ButtonColorAndOpacity(FLinearColor(0.09f,0.19f,0.26f,0.42f))
                .IsEnabled_Lambda([this,Id]() { return CommandEnabled(Id); })
                .OnClicked_Lambda([this,Id]() {
                    RunControl(Id);
                    return FReply::Handled();
                })[Text(Label,20)]];
    };
    auto Label = [](const TCHAR* Value) { return FText::FromString(Value); };
    auto ViewLabel = TAttribute<FText>::CreateLambda([this]() { return FText::FromString(bBridge ? TEXT("Exterior") : TEXT("Bridge")); });
    auto Top = SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)
        [SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[Text(Label(TEXT("DELTA TREE")),22)]
            + SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0)[Text(TAttribute<FText>::CreateLambda([this]() {
                if (!Game()) return FText::GetEmpty(); const auto& C = Game()->Campaign;
                return FText::FromString(FString::Printf(TEXT("Shield %.0f   Hull %.0f   %s"), C.Shield, C.Hull,
                    Game()->bPaused ? TEXT("Paused") : TEXT("Auto helm")));
            }),18)]]
        + SHorizontalBox::Slot().AutoWidth().Padding(8,0)[SNew(SBox).WidthOverride(144)[Button(ViewLabel,7)]]
        + SHorizontalBox::Slot().AutoWidth().Padding(0,0,8,0)[SNew(SBox).WidthOverride(144)[Button(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() && Game()->bPaused ? TEXT("Resume") : TEXT("Pause"));
        }),8)]]
        + SHorizontalBox::Slot().AutoWidth()[SNew(SBox).WidthOverride(170)[Button(Label(TEXT("Hide menu")),10)]];

    auto Tabs = SNew(SHorizontalBox);
    const TCHAR* TabLabels[] = {TEXT("Navigation"),TEXT("Weapons"),TEXT("Expedition"),TEXT("Camera")};
    for (int32 I=0; I<UE_ARRAY_COUNT(TabLabels); ++I)
        Tabs->AddSlot().FillWidth(1).Padding(I == 0 ? 0 : 6,0,0,0)
            [SNew(SBox).MinDesiredHeight(46)
                [SNew(SButton).HAlign(HAlign_Center).VAlign(VAlign_Center).ContentPadding(FMargin(10,4))
                    .ButtonColorAndOpacity_Lambda([this,I]() {
                        return CommandTab == I ? FLinearColor(0.12f,0.46f,0.61f,0.65f) : FLinearColor(0.06f,0.12f,0.18f,0.32f);
                    })
                    .OnClicked_Lambda([this,I]() { CommandTab = I; return FReply::Handled(); })[Text(Label(TabLabels[I]),20)]]];
    auto Navigation = SNew(SHorizontalBox);
    const TCHAR* Orders[] = {TEXT("Hold"),TEXT("Approach"),TEXT("Keep range"),TEXT("Withdraw")};
    for (int32 I=0; I<4; ++I)
        Navigation->AddSlot().FillWidth(1).Padding(I == 0 ? 0 : 6,0,0,0)[Button(Label(Orders[I]),I)];
    auto Weapons = SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(1)[Button(Label(TEXT("Reset practice")),4)]
        + SHorizontalBox::Slot().FillWidth(1).Padding(6,0)[Button(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() && Game()->Campaign.bHoldFire ? TEXT("Auto fire") : TEXT("Hold fire"));
        }),5)]
        + SHorizontalBox::Slot().FillWidth(1)[Button(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() && Game()->Campaign.Loadout.Resolute.bPiercing ? TEXT("Fit standard") : TEXT("Fit piercing"));
        }),6)];
    auto Voyage = SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(1.4f)[Button(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() ? Game()->Campaign.ExpeditionActionLabel() : TEXT(""));
        }),11)]
        + SHorizontalBox::Slot().FillWidth(1.1f).Padding(6,0)[Button(Label(TEXT("Refit / 100")),12)]
        + SHorizontalBox::Slot().FillWidth(1)[Button(Label(TEXT("Save voyage")),9)];
    auto CameraControls = SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(1)[Button(Label(TEXT("Find Delta Tree")),13)]
        + SHorizontalBox::Slot().FillWidth(1).Padding(6,0)[Button(TAttribute<FText>::CreateLambda([this]() {
            if (CameraTarget != EAquilaCameraTarget::Free) return INVTEXT("Release");
            return bTouchRotate ? INVTEXT("Pan view") : INVTEXT("Rotate view");
        }),14)]
        + SHorizontalBox::Slot().FillWidth(1).Padding(0,0,6,0)[Button(Label(TEXT("Zoom in")),15)]
        + SHorizontalBox::Slot().FillWidth(1)[Button(Label(TEXT("Zoom out")),16)];
    auto Menu = SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.015f,0.035f,0.065f,0.18f)).Padding(12)
        [SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[Text(TAttribute<FText>::CreateLambda([this]() {
                if (!Game()) return FText::GetEmpty(); const auto& C = Game()->Campaign;
                if (CommandTab == 3)
                {
                    if (bBridge) return INVTEXT("On the bridge. Drag to look around.");
                    if (CameraTarget == EAquilaCameraTarget::Free)
                        return bTouchRotate ? INVTEXT("Free view. Drag to look around. Tap a ship to follow.")
                                            : INVTEXT("Free view. Drag to pan. Tap a ship to follow.");
                    return INVTEXT("Drag to orbit. Tap empty space to release.");
                }
                if (CommandTab == 2) return FText::FromString(C.ExpeditionStatus());
                if (CommandTab == 1) return FText::FromString(FString::Printf(TEXT("%s  |  Target shield %.0f / hull %.0f"),
                    *C.CombatStatus(), C.EnemyShield, C.EnemyHull));
                return FText::FromString(C.OrderStatus());
            }),17)]
            + SVerticalBox::Slot().AutoHeight()[SNew(SBox)
                .Visibility_Lambda([this]() { return CommandTab == 2 ? EVisibility::Visible : EVisibility::Collapsed; })
                [Text(TAttribute<FText>::CreateLambda([this]() {
                    return FText::FromString(Game() ? Game()->Campaign.UpgradeStatus() : TEXT(""));
                }),17)]]
            + SVerticalBox::Slot().AutoHeight().Padding(0,8)[Tabs]
            + SVerticalBox::Slot().AutoHeight()
            [SNew(SWidgetSwitcher).WidgetIndex_Lambda([this]() { return CommandTab; })
                + SWidgetSwitcher::Slot()[Navigation]
                + SWidgetSwitcher::Slot()[Weapons]
                + SWidgetSwitcher::Slot()[Voyage]
                + SWidgetSwitcher::Slot()[CameraControls]]
            + SVerticalBox::Slot().AutoHeight().Padding(0,6,0,0)[SNew(SBox)
                .Visibility_Lambda([this]() { return CommandTab == 2 ? EVisibility::Visible : EVisibility::Collapsed; })
                [Text(TAttribute<FText>::CreateLambda([this]() { return FText::FromString(Game() ? Game()->SaveStatus : TEXT("")); }),14)]]];

    auto Pad = SNew(SVerticalBox);
    const TCHAR* Labels[] = {TEXT("Forward"),TEXT("Back"),TEXT("Left"),TEXT("Right")};
    const FVector2D Directions[] = {FVector2D(1,0),FVector2D(-1,0),FVector2D(0,-1),FVector2D(0,1)};
    for (int32 Row=0; Row<2; ++Row)
    {
        auto PadRow = SNew(SHorizontalBox);
        for (int32 Col=0; Col<2; ++Col)
        {
            const int32 I = Row*2+Col;
            const FVector2D Direction = Directions[I];
            PadRow->AddSlot().AutoWidth().Padding(3)
                [SNew(SBox).WidthOverride(140).MinDesiredHeight(54)
                    [SNew(SButton).HAlign(HAlign_Center).VAlign(VAlign_Center).ContentPadding(6)
                        .ButtonColorAndOpacity(FLinearColor(0.09f,0.19f,0.26f,0.42f))
                        .OnPressed_Lambda([this,Direction]() { TouchWalk = Direction; })
                        .OnReleased_Lambda([this]() { TouchWalk = FVector2D::ZeroVector; })[Text(Label(Labels[I]),18)]]];
        }
        Pad->AddSlot().AutoHeight()[PadRow];
    }
    auto Walk = SNew(SBox).Visibility_Lambda([this]() { return bBridge ? EVisibility::Visible : EVisibility::Collapsed; })[Pad];
    auto Content = SNew(SOverlay)
        + SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Top).Padding(24,16)[Top]
        + SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Bottom).Padding(24,16)[Menu]
        + SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).Padding(24,104,0,0)[Walk];
    TouchUIRegions = {Top,Menu,Walk};
    // Phone layout has its own readable reference canvas; the desktop strip cannot be scaled down to fit a handset.
    Overlay = SNew(SDPIScaler)
        .Visibility_Lambda([this]() { return bCommandsVisible ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed; })
        .DPIScale_Lambda([]() {
        FVector2D Size(960,480); if (GEngine && GEngine->GameViewport) GEngine->GameViewport->GetViewportSize(Size);
        return FMath::Max(0.5f,FMath::Min(static_cast<float>(Size.X/960),static_cast<float>(Size.Y/480)));
    })[SNew(SSafeZone)[Content]];
    GEngine->GameViewport->AddViewportWidgetContent(Overlay.ToSharedRef(),10);
}

void AAquilaFlightPawn::EndPlay(const EEndPlayReason::Type Reason)
{
    if (IsValid(DuelVisitor)) DuelVisitor->Destroy();
    for (auto& Deck : DuelDecks) if (IsValid(Deck)) Deck->Destroy();
    if (Overlay && GEngine && GEngine->GameViewport) GEngine->GameViewport->RemoveViewportWidgetContent(Overlay.ToSharedRef());
    Overlay.Reset();
    MouseLook.Reset();
    TouchUIRegions.Empty();
    Super::EndPlay(Reason);
}
