#include "AquilaFlightPawn.h"
#include "AquilaBoarding.h"
#include "AquilaBoardingBay.h"
#include "AquilaBridgeLayout.h"
#include "AquilaGameMode.h"
#include "Camera/CameraComponent.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

namespace
{
// The renderer uses a compact arena. Each occupied ship's room and walker stay at full human scale.
constexpr double DuelToCentimetres=2.*AAquilaBoardingBay::ReferenceLengthMetres*100./AquilaHulls::KeelLength;
FRotator DuelHeading(const FAquilaCampaign& Duel,bool bGlacier)
{
    return FRotator(0,bGlacier ? 180. : FMath::RadiansToDegrees(Duel.Heading),0);
}
}

void AAquilaFlightPawn::StartDuelAboard()
{
    auto* Mode=Game();
    if (!Mode || Mode->Session()!=EAquilaSession::Simulator) return;
    Mode->StartDuel(Mode->Campaign.Loadout);
    Mode->bFastDuel=false;
    ResetSessionView();
    EnterDuelBridge();
}

void AAquilaFlightPawn::EnterDuelBridge()
{
    if (!Game() || DuelVisitor) return;
    const auto& C=Game()->Campaign;
    const bool bLastBridgeAlive=(bOnGlacier ? C.EnemyHull : C.Hull)>0;
    VisitDuelShip(bLastBridgeAlive ? bOnGlacier : !bOnGlacier);
}

void AAquilaFlightPawn::SwitchDuelShip()
{
    VisitDuelShip(!bOnGlacier);
}

void AAquilaFlightPawn::VisitDuelShip(bool bGlacier)
{
    using namespace AquilaBridge;
    auto* Mode=Game();
    auto* PC=Cast<APlayerController>(DuelVisitor ? DuelVisitor->GetController() : GetController());
    if (!Mode || !PC || Mode->Session()!=EAquilaSession::Simulator || Mode->bFittingOpen || LastDuelViewFrame==GFrameCounter) return;
    const auto& C=Mode->Campaign;
    if ((bGlacier ? C.EnemyHull : C.Hull)<=0 || (DuelVisitor && bOnGlacier==bGlacier)) return;
    const FTransform& Anchor=AquilaHulls::Layout(bGlacier).BridgeAnchor;
    // Delta's original boarding datum is 17 m ahead of its socket. Glacier's datum sits on the crown itself.
    const FVector Datum=bGlacier ? FVector::ZeroVector : FVector(0,-1700,0);
    const FTransform DeckTransform(Anchor.GetRotation(),Anchor.GetLocation()*DuelToCentimetres-Anchor.GetRotation().RotateVector(Datum));
    auto& Deck=DuelDecks[int32(bGlacier)];
    if (!Deck)
    {
        Deck=GetWorld()->SpawnActorDeferred<AAquilaBoardingBay>(AAquilaBoardingBay::StaticClass(),DeckTransform,
            this,nullptr,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
        if (!Deck) return;
        Deck->SelectDuelDeck(bGlacier);
        Deck->FinishSpawning(DeckTransform);
    }
    const FVector Window=DeckTransform.TransformPosition((bGlacier ? GlacierWindow : Station-Fore*2.6+FVector(0,0,.94))*100.);
    const FVector ToOther=bGlacier ? C.ShipPosition-C.EnemyPosition : C.EnemyPosition-C.ShipPosition;
    const FVector Other=DuelHeading(C,bGlacier).UnrotateVector(ToOther)*ExteriorScale*DuelToCentimetres;
    const FTransform Visit=DuelFooting[int32(bGlacier)].Get(FTransform((Other-Window).Rotation(),Window));
    const FRotator Look=Visit.Rotator();
    Deck->SetActorHiddenInGame(false);
    Deck->SetActorEnableCollision(true);
    if (DuelVisitor)
    {
        DuelFooting[int32(bOnGlacier)].Emplace(PC->GetControlRotation(),DuelVisitor->GetActorLocation());
        auto* Previous=DuelDecks[int32(bOnGlacier)].Get();
        DuelVisitor->SetBase(static_cast<FMovementBaseInterfaceData*>(nullptr));
        DuelVisitor->GetCharacterMovement()->StopMovementImmediately();
        Previous->SetActorEnableCollision(false);
        Previous->SetActorHiddenInGame(true);
        DuelVisitor->SetActorLocationAndRotation(Visit.GetLocation(),FRotator(0,Look.Yaw,0),false,nullptr,ETeleportType::TeleportPhysics);
        DuelVisitor->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
    }
    else
    {
        FActorSpawnParameters Spawn;
        Spawn.Owner=this;
        Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        DuelVisitor=GetWorld()->SpawnActor<AAquilaBoardingWalker>(Visit.GetLocation(),FRotator(0,Look.Yaw,0),Spawn);
        if (!DuelVisitor)
        {
            Deck->SetActorEnableCollision(false);
            Deck->SetActorHiddenInGame(true);
            return;
        }
        ExteriorTransform=GetActorTransform();
        if (auto* Viewport=GetWorld()->GetGameViewport()) ExteriorCapture=Viewport->GetMouseCaptureMode();
        SetActorTransform(FTransform(FQuat::Identity,FVector::ZeroVector,FVector(DuelToCentimetres)));
        PC->Possess(DuelVisitor);
    }
    bOnGlacier=bGlacier;
    LastDuelViewFrame=GFrameCounter;
    PC->SetControlRotation(Look);
    RenderDuelBridge(0);
    UE_LOG(LogTemp,Display,TEXT("Entered %s bridge during simulator duel."),bGlacier ? TEXT("Glacier Tongue") : TEXT("Delta Tree"));
}

void AAquilaFlightPawn::RenderDuelBridge(float Seconds)
{
    const auto& C=Game()->Campaign;
    if ((bOnGlacier ? C.EnemyHull : C.Hull)<=0)
    {
        LeaveDuelBridge();
        return;
    }
    // The character owns the view. The existing arena renderer uses that eye for shield rims and impact billboards.
    FMinimalViewInfo View;
    DuelVisitor->CalcCamera(Seconds,View);
    // Keep the room fixed to its own deck as the simulated helm translates and turns.
    SetActorRotation(DuelHeading(C,bOnGlacier).GetInverse());
    Camera->SetWorldLocationAndRotation(View.Location,View.Rotation);
    RenderCampaign(Seconds);
    DuelDecks[int32(bOnGlacier)]->UpdateDuelInstruments(C);
}

void AAquilaFlightPawn::LeaveDuelBridge()
{
    auto* PC=DuelVisitor ? Cast<APlayerController>(DuelVisitor->GetController()) : nullptr;
    if (!PC || LastDuelViewFrame==GFrameCounter) return;
    LastDuelViewFrame=GFrameCounter;
    DuelFooting[int32(bOnGlacier)].Emplace(PC->GetControlRotation(),DuelVisitor->GetActorLocation());
    auto* Visitor=DuelVisitor.Get();
    DuelVisitor=nullptr;
    PC->Possess(this);
    Visitor->Destroy();
    auto* Deck=DuelDecks[int32(bOnGlacier)].Get();
    Deck->SetActorEnableCollision(false);
    Deck->SetActorHiddenInGame(true);
    SetActorTransform(ExteriorTransform);
    const auto& C=Game()->Campaign;
    if ((bOnGlacier ? C.EnemyHull : C.Hull)<=0)
    {
        CameraTarget=EAquilaCameraTarget::Free;
        OrbitDistance=3300.;
        FrameFreeView(bOnGlacier ? C.EnemyPosition : C.ShipPosition);
    }
    FInputModeGameAndUI Input;
    Input.SetHideCursorDuringCapture(false);
    Input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(Input);
    if (auto* Viewport=GetWorld()->GetGameViewport()) Viewport->SetMouseCaptureMode(ExteriorCapture);
    if (FSlateApplication::IsInitialized()) FSlateApplication::Get().SetUserFocusToGameViewport(0);
    UpdateCursor();
    UpdateCamera(0);
    RenderCampaign(0);
}
