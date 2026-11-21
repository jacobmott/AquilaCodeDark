#if WITH_DEV_AUTOMATION_TESTS && PLATFORM_WINDOWS

#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"
#include "GameFramework/PlayerController.h"
#include "Input/HittestGrid.h"
#include "Misc/AutomationTest.h"
#include "Widgets/SViewport.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
TSharedPtr<SWidget> FindText(const TSharedRef<SWidget>& Widget, const TCHAR* Label)
{
    if (Widget->GetType() == TEXT("STextBlock") && StaticCastSharedRef<STextBlock>(Widget)->GetText().ToString() == Label)
        return Widget;
    if (FChildren* Children = Widget->GetChildren())
        for (int32 Index = 0; Index < Children->Num(); ++Index)
            if (auto Match = FindText(Children->GetChildAt(Index), Label)) return Match;
    return nullptr;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaDesktopCameraTest, "Aquila.Input.DesktopCamera",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::NonNullRHI | EAutomationTestFlags::EngineFilter)

bool FAquilaDesktopCameraTest::RunTest(const FString& Parameters)
{
    UGameViewportClient* Viewport = GEngine->GameViewport;
    if (!TestTrue(TEXT("Rendered game window"), Viewport && Viewport->GetWindow() && Viewport->GetGameViewportWidget())) return false;
    auto* World = Viewport->GetWorld();
    auto* PC = World ? World->GetFirstPlayerController() : nullptr;
    auto* Pawn = PC ? Cast<AAquilaFlightPawn>(PC->GetPawn()) : nullptr;
    auto* Mode = World ? World->GetAuthGameMode<AAquilaGameMode>() : nullptr;
    auto* Camera = Pawn ? Pawn->FindComponentByClass<UCameraComponent>() : nullptr;
    if (!TestTrue(TEXT("Live Aquila camera and campaign"), Camera && Mode)) return false;

    const FGeometry View = Viewport->GetGameViewportWidget()->GetCachedGeometry();
    const FVector2D Center = View.LocalToAbsolute(View.GetLocalSize() * 0.5f);
    FHittestGrid& Grid = Viewport->GetWindow()->GetHittestGrid();
    FSlateApplication& Slate = FSlateApplication::Get();
    const auto VirtualUser = Slate.FindOrCreateVirtualUser(0);
    const int32 UserId = VirtualUser->GetUserIndex();
    const auto User = Slate.GetUser(UserId);
    if (!TestTrue(TEXT("Virtual pointer registered"), User.IsValid())) return false;
    const uint32 Pointer = FSlateApplication::CursorPointerIndex;
    auto HitPath = [&](FVector2D Position)
    {
        auto HitWidgets = Grid.GetBubblePath(Position, 0, false, UserId);
        return FWidgetPath(HitWidgets);
    };
    const FWidgetPath Space = HitPath(Center);
    if (!TestTrue(TEXT("Open space routes to camera"), Space.IsValid() && Space.GetLastWidget()->GetType() == TEXT("SAquilaMouseLook"))) return false;
    const auto Surface = Space.GetLastWidget();
    const auto PauseLabel = FindText(Viewport->GetWindow().ToSharedRef(), TEXT("Pause / resume"));
    if (!TestTrue(TEXT("Pause command present"), PauseLabel.IsValid())) return false;
    const FGeometry LabelGeometry = PauseLabel->GetCachedGeometry();
    const FVector2D OverButton = LabelGeometry.LocalToAbsolute(LabelGeometry.GetLocalSize() * 0.5f);
    const FWidgetPath Command = HitPath(OverButton);
    if (!TestTrue(TEXT("Command reachable above camera"), Command.IsValid() && Command.GetLastWidget() != Surface)) return false;

    const TSet<FKey> Empty;
    const TSet<FKey> Left{EKeys::LeftMouseButton};
    const TSet<FKey> Right{EKeys::RightMouseButton};
    auto ButtonEvent = [&](FVector2D Position, FKey Button, const TSet<FKey>& Held)
    {
        return FPointerEvent(UserId, Pointer, Position, Position, Held, Button, 0, FModifierKeysState());
    };
    auto Move = [&](const FWidgetPath& UnderPointer, FVector2D Position, FVector2D Raw, const TSet<FKey>& Held)
    {
        Slate.RoutePointerMoveEvent(UnderPointer, FPointerEvent(UserId, Pointer, Position, Position, Raw, Held, FModifierKeysState()), false);
    };
    TGuardValue<bool> RestorePause(Mode->bPaused, true);
    const FRotator Start = Camera->GetRelativeRotation();
    const double Radius = Camera->GetRelativeLocation().Size();
    const bool bExterior = FMath::IsNearlyEqual(Radius, 2200., 1.);
    auto YawFromStart = [&]() { return FMath::FindDeltaAngleDegrees(Start.Yaw, Camera->GetRelativeRotation().Yaw); };

    const auto Down = Slate.RoutePointerDownEvent(Space, ButtonEvent(Center, EKeys::RightMouseButton, Right));
    TestTrue(TEXT("Right drag acquires relative capture"), Down.ShouldUseHighPrecisionMouse() && User->GetPointerCaptor(Pointer) == Surface);
    Move(Command, OverButton, FVector2D(240, -20), Right);
    Pawn->Tick(1.f / 60);
    TestTrue(TEXT("One fast paused drag turns 90 degrees"), FMath::IsNearlyEqual(YawFromStart(), 90., 0.05));
    TestTrue(TEXT("Upward drag tilts the view up"), FMath::IsNearlyEqual(Camera->GetRelativeRotation().Pitch - Start.Pitch, 6., 0.05));
    TestTrue(TEXT("Looking preserves camera distance"), FMath::IsNearlyEqual(Radius, Camera->GetRelativeLocation().Size(), 0.1));
    const auto Up = Slate.RoutePointerUpEvent(Command, ButtonEvent(OverButton, EKeys::RightMouseButton, Empty));
    TestTrue(TEXT("Release ends capture and restores the cursor"), User->GetPointerCaptor(Pointer) != Surface && Up.GetRequestedMousePos() == Center.IntPoint());
    Move(Space, Center, FVector2D(100, 0), Empty);
    Mode->bPaused = false;
    Pawn->Tick(0.1f);
    TestTrue(TEXT("View holds briefly after release"), FMath::IsNearlyEqual(YawFromStart(), 90., 0.05));

    const auto LeftDown = Slate.RoutePointerDownEvent(Space, ButtonEvent(Center, EKeys::LeftMouseButton, Left));
    TestTrue(TEXT("Left drag acquires relative capture"), LeftDown.ShouldUseHighPrecisionMouse() && User->GetPointerCaptor(Pointer) == Surface);
    for (int32 Sample = 0; Sample < 8; ++Sample) Move(Space, Center, FVector2D(-30, 2.5), Left);
    Pawn->Tick(1.f / 60);
    TestTrue(TEXT("Eight smaller samples reverse the same turn"), Camera->GetRelativeRotation().Equals(Start, 0.05));
    Move(Space, Center, FVector2D(240, 0), Left);
    User->ReleaseCapture(Pointer);
    Move(Space, Center, FVector2D(120, 0), Empty);
    Pawn->Tick(1.f / 60);
    TestTrue(TEXT("Lost capture ends the drag"), FMath::IsNearlyEqual(YawFromStart(), 90., 0.05));
    for (int32 Frame = 0; Frame < 180; ++Frame) Pawn->Tick(1.f / 60);
    const double ExpectedYaw = bExterior ? 0. : 90.;
    TestTrue(TEXT("Exterior recenters and bridge keeps its heading"), FMath::IsNearlyEqual(YawFromStart(), ExpectedYaw, 0.15));
    TGuardValue<bool> SkipTestSave(Mode->bWritable, false);
    Move(Command, OverButton, FVector2D::ZeroVector, Empty);
    Slate.RoutePointerDownEvent(Command, ButtonEvent(OverButton, EKeys::LeftMouseButton, Left));
    Slate.RoutePointerUpEvent(Command, ButtonEvent(OverButton, EKeys::LeftMouseButton, Empty));
    TestTrue(TEXT("Clicking the command toggles pause after a drag"), Mode->bPaused);
    AddInfo(bExterior ? TEXT("Exterior camera exercised") : TEXT("Bridge camera exercised"));
    return true;
}

#endif
