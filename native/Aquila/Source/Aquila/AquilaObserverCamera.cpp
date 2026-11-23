#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "SAquilaMouseLook.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

float AAquilaFlightPawn::TargetHeading() const
{
    if (CameraTarget == EAquilaCameraTarget::Resolute) return FMath::RadiansToDegrees(Game()->Campaign.Heading);
    return CameraTarget == EAquilaCameraTarget::Sentinel ? 180.f : 0.f;
}

void AAquilaFlightPawn::Follow(EAquilaCameraTarget Target)
{
    if (bBridge || !Game()) return;
    const auto& C = Game()->Campaign;
    if (Target == EAquilaCameraTarget::Sentinel && (!C.bBattle || C.EnemyHull <= 0)) return;
    CameraTarget = Target;
    ObserverFocus = Target == EAquilaCameraTarget::Resolute ? C.ShipPosition : C.EnemyPosition;
    OrbitYaw = 20;
    OrbitPitch = 23;
    OrbitDistance = 2200;
    RecenterDelay = 0;
}

void AAquilaFlightPawn::ReleaseFollow()
{
    if (CameraTarget == EAquilaCameraTarget::Free) return;
    OrbitYaw = FRotator::NormalizeAxis(OrbitYaw + TargetHeading());
    CameraTarget = EAquilaCameraTarget::Free;
    bTouchRotate = false;
}

void AAquilaFlightPawn::PanObserver(FVector Direction, double RenderDistance)
{
    if (Direction.IsNearlyZero() || RenderDistance == 0) return;
    ReleaseFollow();
    ObserverFocus += FRotator(0, OrbitYaw, 0).RotateVector(Direction) * (RenderDistance / ExteriorScale);
}

void AAquilaFlightPawn::PanByPixels(FVector2D Delta)
{
    auto* PC = Cast<APlayerController>(GetController());
    int32 Width = 0, Height = 0;
    if (PC) PC->GetViewportSize(Width, Height);
    if (Width <= 0) return;
    const double Span = 2.0 * OrbitDistance * FMath::Tan(FMath::DegreesToRadians(Camera->FieldOfView * 0.5));
    PanObserver(FVector(Delta.Y, -Delta.X, 0), Span / Width);
}

void AAquilaFlightPawn::ZoomObserver(float Steps)
{
    if (!bBridge) OrbitDistance = FMath::Clamp(OrbitDistance * FMath::Pow(0.8, double(Steps)), 1000., 120000.);
}

void AAquilaFlightPawn::DragDesktopCamera(FVector2D Delta, FKey Button)
{
    if (Button == EKeys::MiddleMouseButton)
    {
        if (!bBridge) PanByPixels(Delta);
        return;
    }
    if (bBridge)
    {
        BridgeYaw = FRotator::NormalizeAxis(BridgeYaw + Delta.X * 0.375);
        BridgePitch = FMath::Clamp(BridgePitch - float(Delta.Y) * 0.3f, -60.f, 60.f);
    }
    else
    {
        OrbitYaw = FRotator::NormalizeAxis(OrbitYaw + Delta.X * 0.375);
        OrbitPitch = FMath::Clamp(OrbitPitch + float(Delta.Y) * 0.3f, -60.f, 82.f);
        RecenterDelay = 0.65f;
    }
}

void AAquilaFlightPawn::PickShip(FVector2D ViewUV)
{
    auto* PC = Cast<APlayerController>(GetController());
    if (bBridge || !PC) return;
    int32 Width, Height;
    PC->GetViewportSize(Width, Height);
    FVector Start, Direction;
    if (!PC->DeprojectScreenPositionToWorld(ViewUV.X * Width, ViewUV.Y * Height, Start, Direction)) return;
    // Hulls overlap this query. A planet blocks it. Hidden hulls cannot win a pick.
    TArray<FHitResult> Hits;
    GetWorld()->LineTraceMultiByChannel(Hits, Start, Start + Direction * 2.e7, ECC_Visibility);
    for (const FHitResult& Hit : Hits)
    {
        const auto* Part = Hit.GetComponent();
        if (!Part || !Part->IsVisible()) continue;
        if (Part->GetAttachParent() == ShipRoot) { Follow(EAquilaCameraTarget::Resolute); return; }
        if (Part->GetAttachParent() == EnemyRoot) { Follow(EAquilaCameraTarget::Sentinel); return; }
    }
    ReleaseFollow();
}

void AAquilaFlightPawn::UpdateCamera(float Seconds)
{
    const auto& C = Game()->Campaign;
    if (CameraTarget == EAquilaCameraTarget::Resolute) ObserverFocus = C.ShipPosition;
    if (CameraTarget == EAquilaCameraTarget::Sentinel)
    {
        if (C.bBattle && C.EnemyHull > 0) ObserverFocus = C.EnemyPosition;
        else ReleaseFollow();
    }
    if (bBridge)
    {
        Camera->SetRelativeLocationAndRotation(Eye, FRotator(BridgePitch, BridgeYaw, 0));
        return;
    }
    if (CameraTarget != EAquilaCameraTarget::Free && !Game()->bPaused)
    {
        if ((MouseLook && MouseLook->IsDragging()) || bTouching) RecenterDelay = 0.65f;
        else if (RecenterDelay > 0) RecenterDelay = FMath::Max(0.f, RecenterDelay - Seconds);
        else
        {
            OrbitYaw = 20.f + FMath::FInterpTo(FMath::FindDeltaAngleDegrees(20.f, OrbitYaw), 0.f, Seconds, 3.f);
            OrbitPitch = FMath::FInterpTo(OrbitPitch, 23.f, Seconds, 3.f);
        }
    }
    const FRotator Rotation(-OrbitPitch, TargetHeading() + OrbitYaw, 0);
    Camera->SetRelativeLocationAndRotation(-Rotation.Vector() * OrbitDistance, Rotation);
}

void AAquilaFlightPawn::BeginCameraTouch(ETouchIndex::Type Finger, FVector Position)
{
    if (!bTouchLayout) return;
    auto* PC = Cast<APlayerController>(GetController());
    if (!PC) return;
    if (Finger == ETouchIndex::Touch2)
    {
        bTouching = false;
        float X, Y;
        bool bFirstDown;
        PC->GetInputTouchState(ETouchIndex::Touch1, X, Y, bFirstDown);
        if (bFirstDown && !bCommandsVisible) ToggleCommands();
        return;
    }
    if (Finger != ETouchIndex::Touch1) return;
    TouchStart = LastTouch = FVector2D(Position.X, Position.Y);
    bTouching = false;
    bTouchDragged = false;
    if (bCommandsVisible && Overlay)
    {
        int32 Width, Height;
        PC->GetViewportSize(Width, Height);
        if (Width <= 0 || Height <= 0) return;
        const FGeometry& Geometry = Overlay->GetCachedGeometry();
        const FVector2D Point = Geometry.LocalToAbsolute(TouchStart / FVector2D(Width, Height) * Geometry.GetLocalSize());
        for (const auto& Region : TouchUIRegions)
            if (Region->GetVisibility().IsVisible() && Region->GetCachedGeometry().IsUnderLocation(Point)) return;
    }
    bTouching = true;
}

void AAquilaFlightPawn::EndCameraTouch(ETouchIndex::Type Finger, FVector Position)
{
    if (Finger != ETouchIndex::Touch1 || !bTouching) return;
    bTouching = false;
    const FVector2D Point(Position.X, Position.Y);
    if (bTouchDragged || FVector2D::Distance(TouchStart, Point) >= 8 || bBridge) return;
    int32 Width, Height;
    CastChecked<APlayerController>(GetController())->GetViewportSize(Width, Height);
    if (Width > 0 && Height > 0) PickShip(Point / FVector2D(Width, Height));
}

void AAquilaFlightPawn::UpdateTouchCamera(APlayerController* PC)
{
    if (!bTouching) return;
    float X, Y;
    bool bPressed;
    PC->GetInputTouchState(ETouchIndex::Touch1, X, Y, bPressed);
    if (!bPressed) { bTouching = false; return; }
    const FVector2D Point(X, Y);
    if (!bTouchDragged && FVector2D::Distance(Point, TouchStart) < 8) return;
    bTouchDragged = true;
    const FVector2D Delta = Point - LastTouch;
    LastTouch = Point;
    if (!bBridge && CameraTarget == EAquilaCameraTarget::Free && !bTouchRotate) PanByPixels(Delta);
    else DragDesktopCamera(Delta * 0.55, EKeys::RightMouseButton);
}
