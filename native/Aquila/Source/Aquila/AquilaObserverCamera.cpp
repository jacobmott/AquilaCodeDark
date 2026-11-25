#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "SAquilaMouseLook.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

FRotator AAquilaFlightPawn::ObserverRotation() const
{
    double Yaw = OrbitYaw;
    if (CameraTarget == EAquilaCameraTarget::Resolute) Yaw += FMath::RadiansToDegrees(Game()->Campaign.Heading);
    else if (CameraTarget == EAquilaCameraTarget::Sentinel) Yaw += 180.;
    return FRotator(-OrbitPitch, Yaw, 0);
}

void AAquilaFlightPawn::FrameFreeView(FVector Point)
{
    CameraTarget = EAquilaCameraTarget::Free;
    ObserverOrigin = Point - ObserverRotation().Vector() * (OrbitDistance / ExteriorScale);
}

void AAquilaFlightPawn::Follow(EAquilaCameraTarget Target)
{
    if (bBridge || !Game()) return;
    const auto& C = Game()->Campaign;
    if (Target == EAquilaCameraTarget::Sentinel && (!C.bBattle || C.EnemyHull <= 0)) return;
    CameraTarget = Target;
    if (Game()->Session() == EAquilaSession::Simulator && !Game()->bFittingOpen)
        bFitEnemy = Target == EAquilaCameraTarget::Sentinel;
    ObserverOrigin = Target == EAquilaCameraTarget::Resolute ? C.ShipPosition : C.EnemyPosition;
    OrbitYaw = 20;
    OrbitPitch = 23;
    OrbitDistance = 2200;
    RecenterDelay = 0;
}

void AAquilaFlightPawn::ReleaseFollow()
{
    if (CameraTarget == EAquilaCameraTarget::Free) return;
    OrbitYaw = FRotator::NormalizeAxis(ObserverRotation().Yaw);
    FrameFreeView(ObserverOrigin);
    bTouchRotate = false;
}

void AAquilaFlightPawn::MoveObserver(FVector WorldDirection, double RenderDistance)
{
    if (WorldDirection.IsNearlyZero() || RenderDistance == 0) return;
    ReleaseFollow();
    ObserverOrigin += WorldDirection * (RenderDistance / ExteriorScale);
}

void AAquilaFlightPawn::PanByPixels(FVector2D Delta)
{
    auto* PC = Cast<APlayerController>(GetController());
    int32 Width = 0, Height = 0;
    if (PC) PC->GetViewportSize(Width, Height);
    if (Width <= 0) return;
    const double Span = 2.0 * OrbitDistance * FMath::Tan(FMath::DegreesToRadians(Camera->FieldOfView * 0.5));
    const FRotator Heading(0, ObserverRotation().Yaw, 0);
    MoveObserver(Heading.RotateVector(FVector(Delta.Y, -Delta.X, 0)), Span / Width);
}

void AAquilaFlightPawn::ZoomObserver(float Steps)
{
    if (bBridge) return;
    const double PreviousDistance = OrbitDistance;
    OrbitDistance = FMath::Clamp(OrbitDistance * FMath::Pow(0.8, double(Steps)), 1000., 120000.);
    if (CameraTarget == EAquilaCameraTarget::Free)
        ObserverOrigin += ObserverRotation().Vector() * ((PreviousDistance - OrbitDistance) / ExteriorScale);
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
    GetWorld()->LineTraceMultiByChannel(Hits, Start, Start + Direction * 2.e7, ECC_Visibility,
        FCollisionQueryParams(SCENE_QUERY_STAT(PickShip),true));
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
    if (Game()->bFittingOpen)
    {
        const FVector Center = ((bFitEnemy ? C.EnemyPosition : C.ShipPosition) - ObserverOrigin) * ExteriorScale;
        const FRotator View(-86, (bFitEnemy ? 180. : FMath::RadiansToDegrees(C.Heading)) - 90., 0.);
        int32 Width = 0, Height = 0;
        if (const auto* PC = Cast<APlayerController>(GetController())) PC->GetViewportSize(Width, Height);
        // Keep space beside the hull for the four choices on narrower Windows displays.
        const double AspectMargin = Width > 0 ? FMath::Max(1., (16./9.) * Height / Width) : 1.;
        const double Distance = 1240. * AspectMargin / FMath::Tan(FMath::DegreesToRadians(Camera->FieldOfView * .5));
        Camera->SetRelativeLocationAndRotation(Center - View.Vector() * Distance, View);
        return;
    }
    if (CameraTarget == EAquilaCameraTarget::Resolute) ObserverOrigin = C.ShipPosition;
    if (CameraTarget == EAquilaCameraTarget::Sentinel)
    {
        if (C.bBattle && C.EnemyHull > 0) ObserverOrigin = C.EnemyPosition;
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
    const FRotator Rotation = ObserverRotation();
    const FVector Offset = CameraTarget == EAquilaCameraTarget::Free ? FVector::ZeroVector : -Rotation.Vector() * OrbitDistance;
    Camera->SetRelativeLocationAndRotation(Offset, Rotation);
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
