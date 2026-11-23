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
#include "InputKeyEventArgs.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Widgets/SViewport.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
TSharedPtr<SWidget> FindText(const TSharedRef<SWidget>& Widget, const TCHAR* Label)
{
    if (!Widget->GetVisibility().IsVisible()) return nullptr;
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
    const bool bExterior = Camera->GetRelativeLocation().Size() > 1500;

    const FGeometry View = Viewport->GetGameViewportWidget()->GetCachedGeometry();
    const FVector2D Center = View.LocalToAbsolute(View.GetLocalSize() * FVector2D(0.08, 0.5));
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
    const FVector2D PanelCenter = View.LocalToAbsolute(View.GetLocalSize() * 0.5f);
    const FWidgetPath AtStartup = HitPath(PanelCenter);
    if (!TestTrue(TEXT("The panel starts hidden; exterior selection has a pointer"), AtStartup.IsValid() && AtStartup.GetLastWidget() == Surface && PC->bShowMouseCursor == bExterior)) return false;
    const int32 StartRevision = Mode->Campaign.Sequence;
    const bool bStartPaused = Mode->bPaused;
    auto PressH = [&]()
    {
        PC->InputKey(FInputKeyEventArgs::CreateSimulated(EKeys::H, IE_Pressed, 1.f));
        PC->PlayerTick(1.f / 60);
        PC->InputKey(FInputKeyEventArgs::CreateSimulated(EKeys::H, IE_Released, 0.f));
        PC->PlayerTick(1.f / 60);
        Slate.ForceRedrawWindow(Viewport->GetWindow().ToSharedRef());
    };
    PressH();
    const auto PauseLabel = FindText(Viewport->GetWindow().ToSharedRef(), bStartPaused ? TEXT("Resume") : TEXT("Pause"));
    if (!TestTrue(TEXT("Pause command present"), PauseLabel.IsValid())) return false;
    const FGeometry LabelGeometry = PauseLabel->GetCachedGeometry();
    const FVector2D OverButton = LabelGeometry.LocalToAbsolute(LabelGeometry.GetLocalSize() * 0.5f);
    const FWidgetPath Shown = HitPath(OverButton);
    if (!TestTrue(TEXT("H reveals a clickable command and pointer"), PC->bShowMouseCursor && Shown.IsValid() && Shown.GetLastWidget() != Surface)) return false;
    PressH();
    const FWidgetPath Hidden = HitPath(OverButton);
    TestTrue(TEXT("H hides the command and returns its pixels to camera control"), PC->bShowMouseCursor == bExterior && Hidden.IsValid() && Hidden.GetLastWidget() == Surface);
    PressH();
    const FWidgetPath Command = HitPath(OverButton);
    if (!TestTrue(TEXT("A third H reopens the panel"), PC->bShowMouseCursor && Command.IsValid() && Command.GetLastWidget() != Surface)) return false;
    TestTrue(TEXT("HUD toggles preserve pause and the save revision"), Mode->bPaused == bStartPaused && Mode->Campaign.Sequence == StartRevision);

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
    TestTrue(TEXT("Free observer and bridge keep their heading"), FMath::IsNearlyEqual(YawFromStart(), 90., 0.15));
    TGuardValue<bool> SkipTestSave(Mode->bWritable, false);
    Move(Command, OverButton, FVector2D::ZeroVector, Empty);
    Slate.RoutePointerDownEvent(Command, ButtonEvent(OverButton, EKeys::LeftMouseButton, Left));
    Slate.RoutePointerUpEvent(Command, ButtonEvent(OverButton, EKeys::LeftMouseButton, Empty));
    TestTrue(TEXT("Clicking the command toggles pause after a drag"), Mode->bPaused);
    TSharedPtr<SWidget> FocusButton;
    for (int32 Index = 0; Index < Command.Widgets.Num(); ++Index)
    {
        const FArrangedWidget& Item = Command.Widgets[Index];
        if (Item.Widget->GetType() == TEXT("SButton"))
        {
            FocusButton = Item.Widget;
            break;
        }
    }
    if (!TestTrue(TEXT("Command receives keyboard focus"), FocusButton && Slate.SetUserFocus(0, FocusButton, EFocusCause::Navigation))) return false;
    const FKeyEvent H(EKeys::H, FModifierKeysState(), 0, false, 0, 0);
    Slate.ProcessKeyDownEvent(H);
    PC->PlayerTick(1.f / 60);
    Slate.ProcessKeyUpEvent(H);
    PC->PlayerTick(1.f / 60);
    Slate.ForceRedrawWindow(Viewport->GetWindow().ToSharedRef());
    const FWidgetPath AfterClick = HitPath(OverButton);
    TestTrue(TEXT("H closes a focused command panel while paused"), PC->bShowMouseCursor == bExterior && AfterClick.IsValid() && AfterClick.GetLastWidget() == Surface);
    AddInfo(bExterior ? TEXT("Exterior camera exercised") : TEXT("Bridge camera exercised"));
    return true;
}

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FAquilaTouchMenuTest, "Aquila.Input.TouchMenu",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::NonNullRHI | EAutomationTestFlags::EngineFilter)

void FAquilaTouchMenuTest::GetTests(TArray<FString>& Names, TArray<FString>& Commands) const
{
    if (FParse::Param(FCommandLine::Get(), TEXT("AquilaTouchUI")))
    {
        Names.Add(TEXT("RevealAndHide"));
        Commands.Add(TEXT("Touch"));
    }
}

bool FAquilaTouchMenuTest::RunTest(const FString& Parameters)
{
    UGameViewportClient* Viewport = GEngine->GameViewport;
    auto* PC = Viewport && Viewport->GetWorld() ? Viewport->GetWorld()->GetFirstPlayerController() : nullptr;
    if (!TestTrue(TEXT("Live touch preview"), PC && Viewport->GetWindow())) return false;
    const auto Window = Viewport->GetWindow().ToSharedRef();
    auto& Slate = FSlateApplication::Get();
    auto Touch = [&](ETouchIndex::Type Finger, ETouchType::Type Event)
    {
        const FTouchId Id(FInputDeviceId::CreateFromInternalId(0), Finger);
        PC->InputTouch(Id, Event, FVector2D(300 + int32(Finger) * 50, 300), Event == ETouchType::Ended ? 0.f : 1.f, FPlatformTime::Cycles64());
        PC->PlayerTick(1.f / 60);
        Slate.ForceRedrawWindow(Window);
    };
    TestFalse(TEXT("Touch interface starts hidden"), FindText(Window, TEXT("RESOLUTE")).IsValid());
    Touch(ETouchIndex::Touch1, ETouchType::Began);
    TestFalse(TEXT("One finger keeps the view clear"), FindText(Window, TEXT("Hide menu")).IsValid());
    Touch(ETouchIndex::Touch2, ETouchType::Began);
    Touch(ETouchIndex::Touch2, ETouchType::Ended);
    Touch(ETouchIndex::Touch1, ETouchType::Ended);
    const auto Hide = FindText(Window, TEXT("Hide menu"));
    if (!TestTrue(TEXT("Second finger reveals touch controls"), Hide.IsValid())) return false;
    const auto Geometry = Hide->GetCachedGeometry();
    const FVector2D Point = Geometry.LocalToAbsolute(Geometry.GetLocalSize() * 0.5f);
    const auto VirtualUser = Slate.FindOrCreateVirtualUser(0);
    const int32 UserId = VirtualUser->GetUserIndex();
    const FWidgetPath Path(Window->GetHittestGrid().GetBubblePath(Point, 0, false, UserId));
    if (!TestTrue(TEXT("Hide button has a live hit path"), Path.IsValid())) return false;
    AddInfo(FString::Printf(TEXT("Hide at %.0f, %.0f. Hit %s"), Point.X, Point.Y, *Path.GetLastWidget()->GetTypeAsString()));
    const uint32 Finger = 0;
    const FPointerEvent Press(UserId, Finger, Point, Point, 1.f, true);
    const FPointerEvent Release(UserId, Finger, Point, Point, 0.f, false);
    Slate.RoutePointerDownEvent(Path, Press);
    Slate.RoutePointerUpEvent(Path, Release);
    Slate.ForceRedrawWindow(Window);
    TestFalse(TEXT("Hide removes the top bar"), FindText(Window, TEXT("RESOLUTE")).IsValid());
    TestFalse(TEXT("Hide removes the command tabs"), FindText(Window, TEXT("Navigation")).IsValid());
    TestFalse(TEXT("Hide removes bridge movement controls"), FindText(Window, TEXT("Forward")).IsValid());
    return true;
}

#endif
