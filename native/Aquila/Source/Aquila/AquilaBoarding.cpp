#include "AquilaBoarding.h"
#include "AquilaBoardingBay.h"
#include "AquilaBridgeLayout.h"
#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "Camera/CameraTypes.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "TimerManager.h"

namespace
{
constexpr float BoardingWalkMultiplier = 5.f;
constexpr double BoardingMouseMultiplier = 3.;
constexpr double OnFootLook = .28 * BoardingMouseMultiplier;
constexpr double ExteriorLook = .35 * BoardingMouseMultiplier;
}

AAquilaBoardingWalker::AAquilaBoardingWalker()
{
    bManualBridge=FParse::Param(FCommandLine::Get(),TEXT("AquilaManualBridge"));
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(34,90);
    BaseEyeHeight = 74;
    bUseControllerRotationYaw = true;
    auto* Movement = GetCharacterMovement();
    Movement->MaxWalkSpeed = 270.f * BoardingWalkMultiplier;
    // Scale acceleration and braking with pace so short presses still work at a doorway.
    Movement->MaxAcceleration = GetDefault<UCharacterMovementComponent>()->MaxAcceleration * BoardingWalkMultiplier;
    Movement->BrakingDecelerationWalking = 1600.f * BoardingWalkMultiplier;
    Movement->MaxStepHeight = 24;
}

void AAquilaBoardingWalker::BeginPlay()
{
    Super::BeginPlay();
    DuelHost=Cast<AAquilaFlightPawn>(GetOwner());
    if (DuelHost) return;
    FString Ship;
    FParse::Value(FCommandLine::Get(),TEXT("AquilaBoardingShip="),Ship);
    FString Start;
    bStartAtBridge=FParse::Value(FCommandLine::Get(),TEXT("AquilaBoardingStart="),Start) && Start.Equals(TEXT("Bridge"),ESearchCase::IgnoreCase);
    // Register the bay's lights after the entry world finishes starting, as on later ship changes.
    GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this,&AAquilaBoardingWalker::BoardShip,
        Ship.Equals(TEXT("DeltaTree"),ESearchCase::IgnoreCase)));
}

bool AAquilaBoardingWalker::IsDeltaTree() const { return Bay && Bay->IsDeltaTree(); }

void AAquilaBoardingWalker::BoardShip(bool bDelta)
{
    auto* Next=GetWorld()->SpawnActorDeferred<AAquilaBoardingBay>(AAquilaBoardingBay::StaticClass(),FTransform::Identity,
        this,nullptr,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (!Next) return;
    const bool bKeepOrbit=bExterior, bKeepBerth=bDockFocus;
    if (Bay)
    {
        SetBase(static_cast<FMovementBaseInterfaceData*>(nullptr));
        Bay->SetActorEnableCollision(false);
        Bay->Destroy();
    }
    Next->SelectShip(bDelta,bManualBridge);
    Next->FinishSpawning(FTransform::Identity);
    Bay=Next;
    ResetRamp();
    bExterior=bKeepOrbit;
    SetOrbitFrame(bKeepOrbit && bKeepBerth);
    if (bStartAtBridge && bDelta) StartAtBridgeWindow();
    bStartAtBridge=false;
    UE_LOG(LogTemp, Display, TEXT("%s boarding test ready. Hull %d m."),bDelta?TEXT("Delta Tree"):TEXT("Glacier Tongue"),HullLengthMetres());
}

void AAquilaBoardingWalker::SwitchShip()
{
    if (DuelHost) DuelHost->SwitchDuelShip();
    else BoardShip(!IsDeltaTree());
}

void AAquilaBoardingWalker::SetupPlayerInputComponent(UInputComponent* Input)
{
    Super::SetupPlayerInputComponent(Input);
    if (DuelHost)
    {
        Input->BindKey(EKeys::N, IE_Pressed, this, &AAquilaBoardingWalker::SwitchShip);
        Input->BindKey(EKeys::Tab, IE_Pressed, this, &AAquilaBoardingWalker::ReturnToDuel);
        Input->BindKey(EKeys::C, IE_Pressed, this, &AAquilaBoardingWalker::ReturnToDuel);
        Input->BindKey(EKeys::Escape, IE_Pressed, this, &AAquilaBoardingWalker::ReturnToDuel);
        return;
    }
    Input->BindKey(EKeys::Tab, IE_Pressed, this, &AAquilaBoardingWalker::ToggleExterior);
    Input->BindKey(EKeys::V, IE_Pressed, this, &AAquilaBoardingWalker::FocusExterior);
    Input->BindKey(EKeys::B, IE_Pressed, this, &AAquilaBoardingWalker::SwitchHullSize);
    Input->BindKey(EKeys::N, IE_Pressed, this, &AAquilaBoardingWalker::SwitchShip);
    Input->BindKey(EKeys::R, IE_Pressed, this, &AAquilaBoardingWalker::ResetRamp);
    Input->BindKey(EKeys::F, IE_Pressed, this, &AAquilaBoardingWalker::UseLift);
    if (bManualBridge)
    {
        Input->BindKey(EKeys::T, IE_Pressed, this, &AAquilaBoardingWalker::TargetDrone);
        Input->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AAquilaBoardingWalker::FireTurret);
    }
    Input->BindKey(EKeys::Escape, IE_Pressed, this, &AAquilaBoardingWalker::MainMenu);
    Input->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &AAquilaBoardingWalker::ZoomIn);
    Input->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &AAquilaBoardingWalker::ZoomOut);
}

void AAquilaBoardingWalker::ResetRamp()
{
    bExterior = bArrived = bCrossedLock = false;
    if (bDroneView) SetOrbitFrame(false);
    bRiding=bGoingUp=bDroneView=false;
    LiftProgress=0;
    SetBase(static_cast<FMovementBaseInterfaceData*>(nullptr));
    GetCharacterMovement()->SetMovementMode(MOVE_Falling);
    if (IsDeltaTree())
    {
        Bay->SetLift(0,false);
        if (bManualBridge) Bay->ResetTraining();
    }
    GetCharacterMovement()->StopMovementImmediately();
    SetActorLocationAndRotation(FVector(0,2730,294), FRotator(0,-90,0), false, nullptr, ETeleportType::TeleportPhysics);
    if (Controller) Controller->SetControlRotation(FRotator(0,-90,0));
}

void AAquilaBoardingWalker::StartAtBridgeWindow()
{
    using namespace AquilaBridge;
    LiftProgress=1.;
    Bay->SetLift(1.,false);
    bArrived=bCrossedLock=true;
    const FVector Footing=(Station-Fore*2.6+FVector(0,0,.94))*100.;
    const FRotator Facing(0,BowYaw+2.,0);
    SetActorLocationAndRotation(Footing,Facing,false,nullptr,ETeleportType::TeleportPhysics);
    if (Controller) Controller->SetControlRotation(Facing);
    UE_LOG(LogTemp,Display,TEXT("Delta Tree walk starts at the bridge window."));
}

void AAquilaBoardingWalker::ToggleExterior()
{
    if (bRiding) return;
    if (!bExterior && IsDeltaTree())
    {
        const bool bFrameDrone=bManualBridge ? AtTactical() : IsWatchingBridge();
        if (bFrameDrone)
        {
            bDroneView=true;
            OrbitYaw=(-AquilaBridge::Fore*.58+AquilaBridge::Right*.81).Rotation().Yaw;
            OrbitPitch=10.;
            OrbitRadius=5800.;
        }
        else if (bDroneView) SetOrbitFrame(false);
    }
    bExterior = !bExterior;
    GetCharacterMovement()->StopMovementImmediately();
}

int32 AAquilaBoardingWalker::HullLengthMetres() const
{
    return Bay ? Bay->HullLengthMetres() : 0;
}

void AAquilaBoardingWalker::SwitchHullSize()
{
    if (!Bay || IsDeltaTree()) return;
    const FVector Position = GetActorLocation();
    const FVector2D OnDeck(Position.X / 100., Position.Y / 100.);
    const double FootHeight = Position.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    const bool bOnLargerPad = Bay->HullScale() > 1 && Bay->LandingPadMetres().IsInsideOrOn(OnDeck)
        && GetCharacterMovement()->IsMovingOnGround() && FMath::IsWithinInclusive(FootHeight, -5., 130.);
    Bay->SwitchHullSize();
    if (!bOnLargerPad) return;

    // The extra deck can disappear under a walker. Keep their footing inside the smaller perimeter rails.
    FBox2D SafeDeck = Bay->LandingPadMetres();
    const double EdgeMargin = .8 + GetCapsuleComponent()->GetScaledCapsuleRadius() / 100.;
    SafeDeck.Min.X += EdgeMargin;
    SafeDeck.Max.X -= EdgeMargin;
    SafeDeck.Max.Y -= EdgeMargin;
    const FVector2D SafeSpot = SafeDeck.GetClosestPointTo(OnDeck);
    if (!SafeSpot.Equals(OnDeck, .001))
    {
        GetCharacterMovement()->StopMovementImmediately();
        SetActorLocation({SafeSpot.X*100.,SafeSpot.Y*100.,Position.Z-FootHeight+2.},false,nullptr,ETeleportType::TeleportPhysics);
    }
}
void AAquilaBoardingWalker::FocusExterior()
{
    if (!bExterior) return;
    SetOrbitFrame(!bDroneView && !bDockFocus);
}
void AAquilaBoardingWalker::SetOrbitFrame(bool bBerth)
{
    bDroneView=false;
    bDockFocus=bBerth;
    if (IsDeltaTree())
    {
        OrbitYaw=bBerth ? 35 : 23;
        OrbitPitch=bBerth ? 20 : 6;
        OrbitRadius=bBerth ? 15200 : 47000;
        return;
    }
    OrbitYaw=bBerth ? 41 : 71;
    OrbitPitch=bBerth ? 8 : -7;
    OrbitRadius=bBerth ? 12300 : 47000;
}
void AAquilaBoardingWalker::ZoomIn() { if (bExterior) OrbitRadius = FMath::Max(bDroneView ? 2300. : bDockFocus ? 4500. : 15500., OrbitRadius / 1.15); }
void AAquilaBoardingWalker::ZoomOut() { if (bExterior) OrbitRadius = FMath::Min(105000., OrbitRadius * 1.15); }
void AAquilaBoardingWalker::MainMenu()
{
    if (auto* Viewport = GetWorld()->GetGameViewport())
        Viewport->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
    if (auto* Mode = GetWorld()->GetAuthGameMode<AAquilaGameMode>()) Mode->TravelToBoarding(false);
}

void AAquilaBoardingWalker::Tick(float Seconds)
{
    Super::Tick(Seconds);
    auto* PC = Cast<APlayerController>(Controller);
    if (!PC) return;
    if (!bCaptured)
    {
        bCaptured = true;
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
        if (auto* Viewport = GetWorld()->GetGameViewport())
            Viewport->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
        if (FSlateApplication::IsInitialized()) FSlateApplication::Get().SetUserFocusToGameViewport(0);
    }
    float DX = 0, DY = 0;
    PC->GetInputMouseDelta(DX,DY);
    if (bExterior)
    {
        OrbitYaw -= DX * ExteriorLook;
        OrbitPitch = FMath::Clamp(OrbitPitch + DY * ExteriorLook, -82., 82.);
        return;
    }
    auto Look = PC->GetControlRotation();
    Look.Yaw += DX * OnFootLook;
    Look.Pitch = FMath::Clamp(FRotator::NormalizeAxis(Look.Pitch) + DY * OnFootLook, -84., 84.);
    PC->SetControlRotation(Look);
    if (bRiding) { UpdateLift(Seconds); return; }
    const FRotationMatrix Frame(FRotator(0, Look.Yaw, 0));
    AddMovementInput(Frame.GetUnitAxis(EAxis::X), int32(PC->IsInputKeyDown(EKeys::W))-int32(PC->IsInputKeyDown(EKeys::S)));
    AddMovementInput(Frame.GetUnitAxis(EAxis::Y), int32(PC->IsInputKeyDown(EKeys::D))-int32(PC->IsInputKeyDown(EKeys::A)));
    if (DuelHost) return;
    const FVector P = GetActorLocation();
    if (P.Z < -800) { ResetRamp(); return; }
    const bool bDelta=IsDeltaTree();
    const double LockOuter=bDelta?-3940.:255., LockInner=bDelta?-4540.:-465.;
    const double GoalOuter=bDelta?-6660.:-770., GoalInner=bDelta?-7880.:-2000.;
    if (FMath::Abs(P.X)<210 && P.Y<LockOuter && P.Y>LockInner) bCrossedLock=true;
    if (!bArrived && bCrossedLock && FMath::Abs(P.X)<450 && P.Y<GoalOuter && P.Y>GoalInner)
    {
        bArrived = true;
        UE_LOG(LogTemp, Display, TEXT("%s reached through airlock."),bDelta?TEXT("Delta Tree first branch junction"):TEXT("Glacier Tongue lift lobby"));
    }
}

void AAquilaBoardingWalker::ReturnToDuel()
{
    if (DuelHost) DuelHost->LeaveDuelBridge();
}

void AAquilaBoardingWalker::CalcCamera(float Seconds, FMinimalViewInfo& View)
{
    if (!bExterior)
    {
        Super::CalcCamera(Seconds, View);
        View.FOV = 82;
        return;
    }
    const FVector Offset = FRotator(OrbitPitch,OrbitYaw,0).Vector() * OrbitRadius;
    if (bDroneView)
    {
        View.Location=AquilaBridge::Drone*100.+Offset;
        View.Rotation=(-Offset).Rotation();
        View.FOV=58.;
        return;
    }
    const double FramingScale = !bDockFocus && Bay ? Bay->HullScale() : 1.;
    FVector Center = bDockFocus ? FVector(0,5400,780) : FVector(0,0,5400)*FramingScale;
    if (IsDeltaTree()) Center = bDockFocus ? FVector(0,1200,500) : Bay->HullViewCenter();
    View.Location = Center + Offset*FramingScale;
    View.Rotation = (-Offset).Rotation();
    View.FOV = 70;
}

FString AAquilaBoardingWalker::LocationName() const
{
    const double Y = GetActorLocation().Y;
    if (IsDeltaTree())
    {
        if (IsDroneView()) return TEXT("Training drone exterior");
        if (bExterior) return bDockFocus?TEXT("Outer dock and shuttle"):TEXT("Delta Tree exterior");
        if (bRiding || InLift()) return TEXT("Command lift");
        if (GetActorLocation().Z>4600 && LiftProgress==1)
            return bManualBridge && AtTactical()?TEXT("Tactical station"):GetActorLocation().X < -16900 ? TEXT("Command bridge") : TEXT("Command deck");
        if (Y>1510 && Y<3150 && FMath::Abs(GetActorLocation().X)<210) return TEXT("Peregrine ramp");
        if (Y>815) return TEXT("Outer landing apron");
        if (Y>-3940) return TEXT("Round arrivals dock");
        if (Y>-4540) return TEXT("Branch airlock");
        if (Y>-6640) return TEXT("Branch passage");
        return TEXT("First branch junction");
    }
    if (bExterior) return bDockFocus ? TEXT("Shuttle berth") : TEXT("Port exterior");
    if (Y > 1510) return TEXT("Peregrine ramp");
    if (Y > 260) return TEXT("Boarding deck");
    if (Y > -490) return TEXT("Airlock 01");
    return TEXT("Lift lobby");
}

bool AAquilaBoardingWalker::InLift() const
{
    if (!IsDeltaTree()) return false;
    const FVector Delta=GetActorLocation()/100.-AquilaBridge::CarPosition(LiftProgress);
    return FMath::Abs(Delta.X)<3.02 && FMath::Abs(Delta.Y)<2.71 && Delta.Z>.4 && Delta.Z<2.4;
}

void AAquilaBoardingWalker::UseLift()
{
    if (bExterior || bRiding || !InLift()) return;
    bGoingUp=LiftProgress==0;
    RiderOffset=GetActorLocation()-AquilaBridge::CarPosition(LiftProgress)*100.;
    GetCharacterMovement()->StopMovementImmediately();
    SetBase(static_cast<FMovementBaseInterfaceData*>(nullptr));
    GetCharacterMovement()->DisableMovement();
    bRiding=true;
    Bay->SetLift(LiftProgress,true);
}

void AAquilaBoardingWalker::UpdateLift(double Seconds)
{
    using namespace AquilaBridge;
    LiftProgress=FMath::Clamp(LiftProgress+(bGoingUp?Seconds:-Seconds)/RideSeconds,0.,1.);
    const bool bArriving=bGoingUp ? LiftProgress==1. : LiftProgress==0.;
    Bay->SetLift(LiftProgress,!bArriving);
    SetActorLocation(CarPosition(LiftProgress)*100.+RiderOffset,false,nullptr,ETeleportType::TeleportPhysics);
    if (!bArriving) return;
    bRiding=false;
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    if (Controller) Controller->SetControlRotation(FRotator(0,180,0));
}

bool AAquilaBoardingWalker::AtTactical() const
{
    if (!IsDeltaTree() || bRiding) return false;
    return FVector::DistSquared(GetActorLocation()/100.,AquilaBridge::Station+FVector(0,0,.9))<FMath::Square(2.05);
}

bool AAquilaBoardingWalker::IsWatchingBridge() const
{
    if (bManualBridge || !IsDeltaTree() || bRiding) return false;
    if (bExterior) return IsDroneView();
    const FVector Offset=GetActorLocation()/100.-AquilaBridge::Center;
    return Offset.SizeSquared2D()<=FMath::Square(17.4) && Offset.Z>=0 && Offset.Z<5.5;
}

void AAquilaBoardingWalker::TargetDrone()
{
    if (!bManualBridge || bExterior || !AtTactical()) return;
    Bay->LockDrone();
    if (Controller) Controller->SetControlRotation((AquilaBridge::Drone*100.-GetPawnViewLocation()).Rotation());
}

void AAquilaBoardingWalker::FireTurret()
{
    if (!bExterior && AtTactical()) Bay->FireTrainingTurret();
}

bool AAquilaBoardingWalker::TracksDrone() const
{
    return IsDeltaTree() && Bay->DroneLocked() && (IsDroneView() || (!bExterior && AtTactical()));
}
int32 AAquilaBoardingWalker::ShieldReading() const { return IsDeltaTree() ? Bay->DroneShield():0; }

FString AAquilaBoardingWalker::BoardingHint() const
{
    if (!IsDeltaTree()) return !bExterior && bArrived ? TEXT("Boarding route complete") : TEXT("");
    if (IsWatchingBridge()) return FString();
    if (IsDroneView()) return TEXT("Tab   Return to tactical station");
    if (bExterior) return TEXT("");
    if (bRiding) return bGoingUp ? TEXT("Lift to Command deck") : TEXT("Lift to Junction 01");
    if (InLift()) return LiftProgress==0 ? TEXT("F   Ride to Command deck") : TEXT("Walk out to bridge    /    F   Return to junction");
    if (bManualBridge && AtTactical())
    {
        if (!Bay->DroneLocked()) return TEXT("T   Lock training drone");
        return ShieldReading() ? TEXT("Space   Fire kinetic cannon     Tab   View drone outside") : TEXT("T   Recharge training drone");
    }
    if (LiftProgress==1 && GetActorLocation().Z>4600) return TEXT("Forward windows    /    Follow the deck lights");
    if (bArrived) return TEXT("Command lift    /    Follow the blue line through the right branch");
    return TEXT("Command bridge    /    Follow Arrivals to Junction 01");
}

void AAquilaBoardingHUD::DrawHUD()
{
    Super::DrawHUD();
    auto* Walker = PlayerOwner ? Cast<AAquilaBoardingWalker>(PlayerOwner->GetPawn()) : nullptr;
    if (!Walker || !Canvas || Walker->IsWatchingBridge()) return;
    const float S = FMath::Clamp(Canvas->ClipX / 1440.f, .75f, 1.5f);
    const FLinearColor Cyan(.44f,.88f,.94f), Ink(.012f,.025f,.038f,.86f);
    DrawRect(Ink,20*S,20*S,256*S,58*S);
    DrawRect(Cyan,20*S,20*S,3*S,58*S);
    const FString ShipLabel = FString(Walker->IsDeltaTree()?TEXT("DELTA TREE   /   "):TEXT("GLACIER TONGUE   /   "))
        + FText::AsNumber(Walker->HullLengthMetres()).ToString() + TEXT(" m");
    DrawText(ShipLabel, Cyan,34*S,25*S,GEngine->GetSmallFont(),1.05f*S);
    DrawText(Walker->LocationName(), FLinearColor::White,34*S,44*S,GEngine->GetMediumFont(),1.15f*S);
    FString Help=Walker->IsDeltaTree()?TEXT("N  Glacier Tongue     "):TEXT("N  Delta Tree     B  compare sizes     ");
    Help+=Walker->IsOutside() ? TEXT("Mouse  orbit     Wheel  zoom     V  ship / berth     Tab  aboard     Esc  menu") :
        TEXT("WASD  walk     Mouse  look     Tab  exterior     R  ramp     Esc  menu");
    float HelpWidth,HelpHeight;
    GetTextSize(Help,HelpWidth,HelpHeight,GEngine->GetSmallFont(),1.02f*S);
    DrawRect(Ink,20*S,Canvas->ClipY-52*S,HelpWidth+20*S,32*S);
    DrawText(Help,FLinearColor::White,30*S,Canvas->ClipY-47*S,GEngine->GetSmallFont(),1.02f*S);
    const FString Hint=Walker->BoardingHint();
    if (!Hint.IsEmpty())
    {
        float W,H;
        GetTextSize(Hint,W,H,GEngine->GetSmallFont(),1.12f*S);
        DrawRect(Ink,20*S,85*S,W+26*S,28*S);
        DrawText(Hint,Cyan,32*S,89*S,GEngine->GetSmallFont(),1.12f*S);
    }
    if (Walker->TracksDrone())
    {
        float X,Y;
        FVector2D Mark, Top;
        if (PlayerOwner->ProjectWorldLocationToScreen(AquilaBridge::Drone*100.,Mark) &&
            PlayerOwner->ProjectWorldLocationToScreen((AquilaBridge::Drone+FVector(0,0,AquilaBridge::ShieldRadius))*100.,Top))
        {
            const double Gap=FVector2D::Distance(Mark,Top)*1.14;
            for (double Side : {-1.,1.})
                DrawLine(Mark.X+Side*Gap,Mark.Y,Mark.X+Side*(Gap+8*S),Mark.Y,Cyan,1.5f*S);
        }
        const FString Readout=FString::Printf(TEXT("DRONE 01    /    SHIELD  %d%%"),Walker->ShieldReading());
        GetTextSize(Readout,X,Y,GEngine->GetSmallFont(),1.12f*S);
        DrawRect(Ink,20*S,117*S,X+26*S,28*S);
        DrawText(Readout,FLinearColor::White,32*S,121*S,GEngine->GetSmallFont(),1.12f*S);
    }
}
