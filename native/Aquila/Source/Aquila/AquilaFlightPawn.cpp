#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SDPIScaler.h"
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
constexpr double ExteriorScale = 0.1; // Render cm per simulation meter; bridge uses ordinary centimeters separately.
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
    if (!G) return false;
    if (Id == 7 || Id == 8 || Id == 10) return true;
    if (!G->bWritable) return false;
    if (Id == 4) return !G->Campaign.ExpeditionActive();
    if (Id == 11) return G->Campaign.CanExpeditionAction();
    if (Id == 12) return G->Campaign.CanUpgradePortTurret();
    return true;
}

UStaticMeshComponent* AAquilaFlightPawn::Shape(USceneComponent* Parent, const TCHAR* MeshPath, FVector Position, FVector Size, int32 Material)
{
    auto* Mesh = NewObject<UStaticMeshComponent>(this);
    Mesh->SetupAttachment(Parent);
    Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, MeshPath));
    Mesh->SetMaterial(0, Materials[Material]);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCastShadow(false);
    Mesh->RegisterComponent();
    Mesh->SetRelativeLocation(Position);
    Mesh->SetRelativeScale3D(Size / 100);
    return Mesh;
}

UInstancedStaticMeshComponent* AAquilaFlightPawn::Instances(int32 Material)
{
    auto* Mesh = NewObject<UInstancedStaticMeshComponent>(this);
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, Sphere));
    Mesh->SetMaterial(0, Materials[Material]);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCastShadow(false);
    Mesh->RegisterComponent();
    return Mesh;
}

void AAquilaFlightPawn::BuildShip(USceneComponent* Parent, bool bEnemy)
{
    const int32 Hull = bEnemy ? 1 : 0;
    Shape(Parent, Cube, FVector::ZeroVector, FVector(1360, 180, 52), Hull);
    Shape(Parent, Cube, FVector(-80, 0, 38), FVector(760, 150, 30), 1);
    Shape(Parent, Cube, FVector(-280, 0, 70), FVector(110, 90, 38), Hull);
    Shape(Parent, Cube, FVector(-220, 0, 76), FVector(4, 75, 12), 2);
    for (int32 Side : {-1, 1})
    {
        Shape(Parent, Cube, FVector(-60, Side * 175, 0), FVector(1020, 130, 76), Hull);
        Shape(Parent, Cube, FVector(-60, Side * 243, 0), FVector(760, 3, 12), 2);
        for (int32 I = 0; I < 7; ++I)
            Shape(Parent, Cube, FVector(-460 + I * 145, Side * 175, 42), FVector(112, 110, 8), 1);
        Shape(Parent, Cube, FVector(-580, Side * 175, 0), FVector(6, 90, 45), 2);
        Shape(Parent, Cube, FVector(160, Side * 190, 80), FVector(70, 70, 25), Hull);
        Shape(Parent, Cube, FVector(220, Side * 190, 85), FVector(100, 8, 8), 1);
        Shape(Parent, Cube, FVector(-260, Side * 100, 0), FVector(80, 90, 35), 1);
    }
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

void AAquilaFlightPawn::BeginPlay()
{
    Super::BeginPlay();
    UMaterialInterface* Hull = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Foundation/M_Hull.M_Hull"));
    UMaterialInterface* Glow = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Foundation/M_Glow.M_Glow"));
    if (!Hull || !Glow)
    {
        UE_LOG(LogTemp, Error, TEXT("Aquila content missing. Run scripts/native.ps1 -Action Prepare after Build."));
        Hull = Glow = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    }
    const FLinearColor Colors[] = {FLinearColor(0.24f,0.32f,0.4f), FLinearColor(0.06f,0.1f,0.15f), FLinearColor(0.04f,0.65f,0.85f),
        FLinearColor(1,0.63f,0.16f), FLinearColor(1,0.13f,0.25f), FLinearColor(0.21f,0.08f,0.7f), FLinearColor(0.035f,0.12f,0.21f), FLinearColor(0.38f,0.56f,0.8f)};
    for (int32 I = 0; I < 8; ++I)
    {
        auto* M = UMaterialInstanceDynamic::Create((I >= 2 && I != 6) ? Glow : Hull, this);
        M->SetVectorParameterValue(TEXT("Color"), Colors[I]);
        M->SetScalarParameterValue(TEXT("Glow"), I == 7 ? 1.0 : 2.0);
        Materials.Add(M);
    }
    BuildShip(ShipRoot, false);
    BuildShip(EnemyRoot, true);
    BuildRoom();
    PortUpgradeGlow = Shape(ShipRoot,Cube,FVector(160,-190,96),FVector(48,55,5),2);
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
    if (auto* Surface = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Foundation/M_Planet.M_Planet"))) Planet->SetMaterial(0,Surface);
    FriendlyRounds = Instances(3);
    HostileRounds = Instances(4);
    Impacts = Instances(5);
    Stars = Instances(7);
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
        PC->bShowMouseCursor = true;
        PC->bEnableTouchEvents = true;
        FInputModeGameAndUI Input;
        Input.SetHideCursorDuringCapture(false);
        Input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Input);
    }
    BuildOverlay();
    FString Demo;
    if (FParse::Value(FCommandLine::Get(), TEXT("AquilaDemo="), Demo))
    {
        bBridge = Demo == TEXT("bridge");
        if (Demo == TEXT("combat") && Game()) Game()->Command(4);
    }
    RenderCampaign();
}

void AAquilaFlightPawn::ToggleBridge()
{
    bBridge = !bBridge;
    if (Game()) Game()->Save();
}

void AAquilaFlightPawn::RenderCampaign()
{
    auto* G = Game();
    if (!G) return;
    const auto& C = G->Campaign;
    SignalRoot->SetVisibility(C.ExpeditionActive(),true);
    SignalRoot->SetRelativeLocation(((C.Expedition == EAquilaExpedition::Salvage ? C.EnemyPosition : C.SignalPosition) - C.ShipPosition) * ExteriorScale);
    ShipRoot->SetVisibility(!bBridge, true);
    PortUpgradeGlow->SetVisibility(C.PortTurretLevel > 0 && !bBridge);
    RoomRoot->SetVisibility(bBridge, true);
    EnemyRoot->SetVisibility(C.bBattle && C.EnemyHull > 0, true);
    ShipRoot->SetRelativeRotation(FRotator(0, FMath::RadiansToDegrees(C.Heading), 0));
    EnemyRoot->SetRelativeLocation((C.EnemyPosition - C.ShipPosition) * ExteriorScale);
    EnemyRoot->SetRelativeRotation(FRotator(0, 180, 0));
    Planet->SetRelativeLocation((FVector(180000,0,-30000) - C.ShipPosition) * ExteriorScale);
    FriendlyRounds->ClearInstances();
    HostileRounds->ClearInstances();
    Impacts->ClearInstances();
    for (const auto& R : C.Rounds)
    {
        const FTransform Transform(R.Velocity.Rotation(), (R.Position - C.ShipPosition) * ExteriorScale, FVector(0.6,0.055,0.055));
        (R.bEnemy ? HostileRounds : FriendlyRounds)->AddInstance(Transform);
    }
    for (const auto& Hit : C.Impacts)
    {
        const double Size = (Hit.bShield ? 1.5 : 0.5) * (1 + Hit.Age * 4);
        Impacts->AddInstance(FTransform(FRotator::ZeroRotator, (Hit.Position - C.ShipPosition) * ExteriorScale, FVector(Size,Size,0.12)));
    }
}

void AAquilaFlightPawn::Tick(float Seconds)
{
    Super::Tick(Seconds);
    auto* G = Game();
    auto* PC = Cast<APlayerController>(GetController());
    if (!G || !PC) return;
    AliveTime += Seconds;
    if (PC->WasInputKeyJustPressed(EKeys::Escape)) G->Command(8);
    if (PC->WasInputKeyJustPressed(EKeys::C)) ToggleBridge();
    if (PC->WasInputKeyJustPressed(EKeys::E) && CommandEnabled(11)) G->Command(11);
    if (PC->WasInputKeyJustPressed(EKeys::U) && CommandEnabled(12)) G->Command(12);
    float DX = 0, DY = 0;
    PC->GetInputMouseDelta(DX, DY);
    bool bHeld = PC->IsInputKeyDown(EKeys::RightMouseButton) || PC->IsInputKeyDown(EKeys::LeftMouseButton);
    float TX, TY;
    bool bTouch;
    PC->GetInputTouchState(ETouchIndex::Touch1, TX, TY, bTouch);
    int32 Width, Height;
    PC->GetViewportSize(Width, Height);
    // Use the actual phone panel bounds: opening a larger menu must not turn its buttons into camera drags.
    bool bCameraArea = TY > Height * 0.2 && TY < Height * 0.68 && TX > Width * 0.25;
    if (bTouchLayout && Overlay && Width > 0 && Height > 0)
    {
        const FGeometry& Geometry = Overlay->GetCachedGeometry();
        const FVector2D Point = Geometry.LocalToAbsolute(FVector2D(TX / Width, TY / Height) * Geometry.GetLocalSize());
        bCameraArea = true;
        for (const auto& Region : TouchUIRegions)
            if (Region->GetVisibility().IsVisible() && Region->GetCachedGeometry().IsUnderLocation(Point)) bCameraArea = false;
        if (bTouch) bHeld = false;
    }
    if (bTouch && bCameraArea)
    {
        if (bTouching) { DX = TX - LastTouch.X; DY = TY - LastTouch.Y; }
        LastTouch = FVector2D(TX,TY);
        bHeld = true;
    }
    bTouching = bTouch && bCameraArea;
    if (G->bPaused) TouchWalk = FVector2D::ZeroVector;
    if (!G->bPaused)
    {
        // Bound anomalous input samples: cursor warps and a resumed touch cannot snap the view.
        if (bHeld && FMath::Abs(DX) < 100 && FMath::Abs(DY) < 100)
        {
            if (bBridge)
            {
                BridgeYaw += DX * 0.17;
                BridgePitch = FMath::Clamp(BridgePitch - DY * 0.17f, -60.f, 60.f);
            }
            else
            {
                OrbitYaw += DX * 0.2;
                OrbitPitch = FMath::Clamp(OrbitPitch + DY * 0.17f, -60.f, 75.f);
            }
        }
        if (!bHeld && !bBridge)
        {
            OrbitYaw = FMath::FInterpTo(OrbitYaw, 20.f, Seconds, 3.f);
            OrbitPitch = FMath::FInterpTo(OrbitPitch, 23.f, Seconds, 3.f);
        }
        if (bBridge)
        {
            const double Forward = (PC->IsInputKeyDown(EKeys::W) ? 1 : 0) - (PC->IsInputKeyDown(EKeys::S) ? 1 : 0) + TouchWalk.X;
            const double Side = (PC->IsInputKeyDown(EKeys::D) ? 1 : 0) - (PC->IsInputKeyDown(EKeys::A) ? 1 : 0) + TouchWalk.Y;
            const FRotator Heading(0,BridgeYaw,0);
            FVector Movement = Heading.RotateVector(FVector(Forward,Side,0)).GetClampedToMaxSize(1);
            const FVector Proposed = Eye + Movement * 500 * FMath::Min(Seconds, 0.05f);
            const bool bConsole = FMath::Abs(Proposed.X - 250) < 240 && FMath::Abs(FMath::Abs(Proposed.Y) - 460) < 130;
            const bool bChair = FMath::Abs(Proposed.X + 400) < 150 && FMath::Abs(Proposed.Y) < 140;
            if (!bConsole && !bChair)
            {
                Eye.X = FMath::Clamp(Proposed.X, -1030.0, 870.0);
                Eye.Y = FMath::Clamp(Proposed.Y, -770.0, 770.0);
            }
        }
    }
    // The entry map may offset the pawn at spawn. Keep camera and scene in the same local frame.
    if (bBridge) Camera->SetRelativeLocationAndRotation(Eye, FRotator(BridgePitch,BridgeYaw,0));
    else
    {
        const double Yaw = FMath::RadiansToDegrees(G->Campaign.Heading) + OrbitYaw;
        const FVector Offset = FRotator(-OrbitPitch,Yaw,0).RotateVector(FVector(-2200,0,0));
        Camera->SetRelativeLocationAndRotation(Offset, (-Offset).Rotation());
    }
    RenderCampaign();
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
    auto Text = [](TAttribute<FText> Value, int32 Size)
    {
        return SNew(STextBlock).Text(Value).Font(FCoreStyle::GetDefaultFontStyle("Regular", Size))
            .ColorAndOpacity(Ink).ShadowOffset(FVector2D(1,1)).ShadowColorAndOpacity(FLinearColor(0,0,0,0.9f));
    };
    auto Button = [this,Text](const TCHAR* Label, int32 Id)
    {
        return SNew(SButton).ButtonColorAndOpacity(FLinearColor(0.06f,0.15f,0.22f,0.32f)).ContentPadding(FMargin(18,12))
            .IsEnabled_Lambda([this,Id]() { return CommandEnabled(Id); })
            .OnClicked_Lambda([this,Id]() { if (Id == 7) ToggleBridge(); else if (Game()) Game()->Command(Id); return FReply::Handled(); })
            [Text(FText::FromString(Label),16)];
    };
    auto Commands = SNew(SHorizontalBox);
    auto Helm = SNew(SVerticalBox);
    Helm->AddSlot().AutoHeight().Padding(0,0,0,10)[Text(FText::FromString(TEXT("01   NAVIGATION")),12)];
    auto HelmRow = SNew(SHorizontalBox);
    HelmRow->AddSlot().AutoWidth().Padding(0,0,6,0)[Button(TEXT("Hold"),0)];
    HelmRow->AddSlot().AutoWidth().Padding(0,0,6,0)[Button(TEXT("Approach Vesper"),1)];
    HelmRow->AddSlot().AutoWidth()[Button(TEXT("Keep range"),2)];
    Helm->AddSlot().AutoHeight()[HelmRow];
    Helm->AddSlot().AutoHeight().Padding(0,6,0,0)[Button(TEXT("Withdraw"),3)];
    auto Tactical = SNew(SVerticalBox);
    Tactical->AddSlot().AutoHeight().Padding(0,0,0,10)[Text(FText::FromString(TEXT("02   TACTICAL")),12)];
    auto BattleRow = SNew(SHorizontalBox);
    BattleRow->AddSlot().AutoWidth().Padding(0,0,6,0)[Button(TEXT("Start / reset practice"),4)];
    BattleRow->AddSlot().AutoWidth()[Button(TEXT("Auto / hold fire"),5)];
    Tactical->AddSlot().AutoHeight()[BattleRow];
    Tactical->AddSlot().AutoHeight().Padding(0,6,0,0)[Button(TEXT("Standard / piercing rounds"),6)];
    auto View = SNew(SVerticalBox);
    View->AddSlot().AutoHeight().Padding(0,0,0,10)[Text(FText::FromString(TEXT("03   COMMAND")),12)];
    auto ViewRow = SNew(SHorizontalBox);
    ViewRow->AddSlot().AutoWidth().Padding(0,0,6,0)[Button(TEXT("Bridge / exterior"),7)];
    ViewRow->AddSlot().AutoWidth()[Button(TEXT("Pause / resume"),8)];
    View->AddSlot().AutoHeight()[ViewRow];
    View->AddSlot().AutoHeight().Padding(0,6,0,0)[Button(TEXT("Save voyage"),9)];
    Commands->AddSlot().FillWidth(1).Padding(0,0,24,0)[Helm];
    Commands->AddSlot().FillWidth(1).Padding(0,0,24,0)[Tactical];
    Commands->AddSlot().FillWidth(1)[View];
    auto Content = SNew(SOverlay)
    + SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).Padding(48,30)
    [SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight()[Text(FText::FromString(TEXT("A Q U I L A")),30)]
        + SVerticalBox::Slot().AutoHeight().Padding(0,9)[Text(FText::FromString(TEXT("RESOLUTE  /  RESEARCH BATTLESHIP")),13)]
        + SVerticalBox::Slot().AutoHeight()[Text(TAttribute<FText>::CreateLambda([this]() {
            auto* G = Game(); if (!G) return FText::GetEmpty();
            return FText::FromString(FString::Printf(TEXT("SHIELD %03.0f    HULL %03.0f    %s"),G->Campaign.Shield,G->Campaign.Hull,G->bPaused?TEXT("PAUSED"):TEXT("AUTO HELM")));
        }),18)]
        + SVerticalBox::Slot().AutoHeight().Padding(0,6)[Text(TAttribute<FText>::CreateLambda([this]() {
            if (!Game()) return FText::GetEmpty(); const auto& C=Game()->Campaign;
            return FText::FromString(C.OrderStatus());
        }),13)]]
    + SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Top).Padding(48,35)
    [SNew(SBox).WidthOverride(620)[SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight()[Text(FText::FromString(TEXT("VESPER ECHO  /  EXPEDITION")),19)]
        + SVerticalBox::Slot().AutoHeight().Padding(0,12)[SNew(STextBlock)
            .Text_Lambda([this]() { return FText::FromString(Game() ? Game()->Campaign.ExpeditionStatus() : TEXT("")); })
            .Font(FCoreStyle::GetDefaultFontStyle("Bold",16)).AutoWrapText(true).ColorAndOpacity(Ink)
            .ShadowOffset(FVector2D(1,1)).ShadowColorAndOpacity(FLinearColor::Black)]
        + SVerticalBox::Slot().AutoHeight().Padding(0,0,0,10)[Text(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() ? Game()->Campaign.UpgradeStatus() : TEXT(""));
        }),14)]
        + SVerticalBox::Slot().AutoHeight()[SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1).Padding(0,0,8,0)[SNew(SButton)
                .ButtonColorAndOpacity(FLinearColor(0.06f,0.3f,0.38f,0.28f)).ContentPadding(FMargin(18,12))
                .IsEnabled_Lambda([this]() { return CommandEnabled(11); })
                .OnClicked_Lambda([this]() { if (Game()) Game()->Command(11); return FReply::Handled(); })
                [Text(TAttribute<FText>::CreateLambda([this]() {
                    return FText::FromString(Game() ? Game()->Campaign.ExpeditionActionLabel() : TEXT(""));
                }),16)]]
            + SHorizontalBox::Slot().AutoWidth()[Button(TEXT("Refit port turret / 100"),12)]]
        + SVerticalBox::Slot().AutoHeight().Padding(0,10)[Text(FText::FromString(TEXT("E: expedition order   U: refit   C: bridge   Esc: pause")),12)]]]
    + SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Bottom).Padding(48,24)
    [SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.015f,0.035f,0.065f,0.16f)).Padding(18)
        [SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight().Padding(0,0,0,8)[Text(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() ? Game()->Campaign.CombatStatus() : TEXT("INITIALIZING"));
        }),14)]
        + SVerticalBox::Slot().AutoHeight().Padding(0,0,0,14)[Text(TAttribute<FText>::CreateLambda([this]() {
            if (!Game()) return FText::GetEmpty(); const auto& C=Game()->Campaign;
            return FText::FromString(FString::Printf(TEXT("%s  |  SENTINEL SHIELD %.0f / HULL %.0f  |  %d ROUNDS IN FLIGHT"),C.bPiercing?TEXT("PIERCING"):TEXT("STANDARD"),C.EnemyShield,C.EnemyHull,C.Rounds.Num()));
        }),12)]
        + SVerticalBox::Slot().AutoHeight()[Commands]
        + SVerticalBox::Slot().AutoHeight().Padding(0,15,0,0)[Text(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() ? Game()->SaveStatus : TEXT(""));
        }),11)]]];
    auto Pad = SNew(SHorizontalBox);
    const TCHAR* Labels[] = {TEXT("Forward"),TEXT("Back"),TEXT("Left"),TEXT("Right")};
    const FVector2D Directions[] = {FVector2D(1,0),FVector2D(-1,0),FVector2D(0,-1),FVector2D(0,1)};
    for (int32 I=0; I<4; ++I)
    {
        const FVector2D Direction = Directions[I];
        Pad->AddSlot().AutoWidth().Padding(3)[SNew(SButton).ContentPadding(FMargin(12,18))
            .OnPressed_Lambda([this,Direction]() { TouchWalk = Direction; })
            .OnReleased_Lambda([this]() { TouchWalk = FVector2D::ZeroVector; })[Text(FText::FromString(Labels[I]),14)]];
    }
    Content->AddSlot().HAlign(HAlign_Left).VAlign(VAlign_Center).Padding(48,0)
        [SNew(SBox).Visibility_Lambda([this]() { return bBridge ? EVisibility::Visible : EVisibility::Collapsed; })[Pad]];
    Overlay = SNew(SDPIScaler).DPIScale_Lambda([]() {
        FVector2D Size(1600,900); if(GEngine && GEngine->GameViewport) GEngine->GameViewport->GetViewportSize(Size);
        return FMath::Max(0.4f,FMath::Min(static_cast<float>(Size.X/1600),static_cast<float>(Size.Y/900)));
    })[Content];
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
                    if (Id == 7) { TouchWalk = FVector2D::ZeroVector; ToggleBridge(); }
                    else if (Id == 10) bCommandsVisible = !bCommandsVisible;
                    else if (Game()) Game()->Command(Id);
                    return FReply::Handled();
                })[Text(Label,20)]];
    };
    auto Label = [](const TCHAR* Value) { return FText::FromString(Value); };
    auto ViewLabel = TAttribute<FText>::CreateLambda([this]() { return FText::FromString(bBridge ? TEXT("Exterior") : TEXT("Bridge")); });
    auto Top = SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(1).VAlign(VAlign_Center)
        [SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[Text(Label(TEXT("RESOLUTE")),22)]
            + SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0)[Text(TAttribute<FText>::CreateLambda([this]() {
                if (!Game()) return FText::GetEmpty(); const auto& C = Game()->Campaign;
                return FText::FromString(FString::Printf(TEXT("Shield %.0f   Hull %.0f   %s"), C.Shield, C.Hull,
                    Game()->bPaused ? TEXT("Paused") : TEXT("Auto helm")));
            }),18)]]
        + SHorizontalBox::Slot().AutoWidth().Padding(8,0)[SNew(SBox).WidthOverride(144)[Button(ViewLabel,7)]]
        + SHorizontalBox::Slot().AutoWidth().Padding(0,0,8,0)[SNew(SBox).WidthOverride(144)[Button(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() && Game()->bPaused ? TEXT("Resume") : TEXT("Pause"));
        }),8)]]
        + SHorizontalBox::Slot().AutoWidth()[SNew(SBox).WidthOverride(170)[Button(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(bCommandsVisible ? TEXT("Hide menu") : TEXT("Menu"));
        }),10)]];

    auto Tabs = SNew(SHorizontalBox);
    const TCHAR* TabLabels[] = {TEXT("Navigation"),TEXT("Weapons"),TEXT("Expedition")};
    for (int32 I=0; I<3; ++I)
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
            return FText::FromString(Game() && Game()->Campaign.bPiercing ? TEXT("Fit standard") : TEXT("Fit piercing"));
        }),6)];
    auto Voyage = SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(1.4f)[Button(TAttribute<FText>::CreateLambda([this]() {
            return FText::FromString(Game() ? Game()->Campaign.ExpeditionActionLabel() : TEXT(""));
        }),11)]
        + SHorizontalBox::Slot().FillWidth(1.1f).Padding(6,0)[Button(Label(TEXT("Refit / 100")),12)]
        + SHorizontalBox::Slot().FillWidth(1)[Button(Label(TEXT("Save voyage")),9)];
    auto Menu = SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .Visibility_Lambda([this]() { return bCommandsVisible ? EVisibility::Visible : EVisibility::Collapsed; })
        .BorderBackgroundColor(FLinearColor(0.015f,0.035f,0.065f,0.18f)).Padding(12)
        [SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[Text(TAttribute<FText>::CreateLambda([this]() {
                if (!Game()) return FText::GetEmpty(); const auto& C = Game()->Campaign;
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
                + SWidgetSwitcher::Slot()[Voyage]]
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
    Overlay = SNew(SDPIScaler).DPIScale_Lambda([]() {
        FVector2D Size(960,480); if (GEngine && GEngine->GameViewport) GEngine->GameViewport->GetViewportSize(Size);
        return FMath::Max(0.5f,FMath::Min(static_cast<float>(Size.X/960),static_cast<float>(Size.Y/480)));
    })[SNew(SSafeZone)[Content]];
    GEngine->GameViewport->AddViewportWidgetContent(Overlay.ToSharedRef(),10);
}

void AAquilaFlightPawn::EndPlay(const EEndPlayReason::Type Reason)
{
    if (Overlay && GEngine && GEngine->GameViewport) GEngine->GameViewport->RemoveViewportWidgetContent(Overlay.ToSharedRef());
    Overlay.Reset();
    TouchUIRegions.Empty();
    Super::EndPlay(Reason);
}
