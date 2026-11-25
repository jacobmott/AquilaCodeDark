#if WITH_DEV_AUTOMATION_TESTS && PLATFORM_WINDOWS

#include "AquilaFlightPawn.h"
#include "AquilaUITest.h"
#include "AquilaFiringRibbon.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AquilaGameMode.h"
#include "AquilaSave.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"
#include "GameFramework/PlayerController.h"
#include "Input/HittestGrid.h"
#include "InputKeyEventArgs.h"
#include "ImageUtils.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Widgets/SViewport.h"
#include "Widgets/SCanvas.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
TSharedPtr<SWidget> FindText(const TSharedRef<SWidget>& Widget, const TCHAR* Label, FName Scope = NAME_None)
{
    if (!Widget->GetVisibility().IsVisible()) return nullptr;
    if (Widget->GetTag() == Scope) Scope = NAME_None;
    if (Scope.IsNone() && Widget->GetType() == TEXT("STextBlock") && StaticCastSharedRef<STextBlock>(Widget)->GetText().ToString() == Label)
        return Widget;
    if (FChildren* Children = Widget->GetChildren())
        for (int32 Index = 0; Index < Children->Num(); ++Index)
            if (auto Match = FindText(Children->GetChildAt(Index), Label, Scope)) return Match;
    return nullptr;
}

bool ClickDuelAt(FAutomationTestBase& Test,const TSharedRef<SWindow>& Window,const FVector2D& Position,const TCHAR* Text)
{
    auto& Slate=FSlateApplication::Get();
    const auto VirtualUser=Slate.FindOrCreateVirtualUser(0);
    const int32 User=VirtualUser->GetUserIndex();
    const FWidgetPath Path(Window->GetHittestGrid().GetBubblePath(Position,0,false,User));
    const bool bButton=Path.Widgets.GetInternalArray().ContainsByPredicate([](const FArrangedWidget& Item) {
        return Item.Widget->GetType()==TEXT("SButton");
    });
    if (!Test.TestTrue(FString::Printf(TEXT("Hit a button for %s"),Text),bButton)) return false;
    const TSet<FKey> Empty;
    Slate.RoutePointerMoveEvent(Path,FPointerEvent(User,FSlateApplication::CursorPointerIndex,Position,Position,
        FVector2D::ZeroVector,Empty,FModifierKeysState()),false);
    for (bool bDown : {true,false})
    {
        TSet<FKey> Held;
        if (bDown) Held.Add(EKeys::LeftMouseButton);
        FPointerEvent Event(User,FSlateApplication::CursorPointerIndex,Position,Position,Held,EKeys::LeftMouseButton,0,FModifierKeysState());
        if (bDown) Slate.RoutePointerDownEvent(Path,Event);
        else Slate.RoutePointerUpEvent(Path,Event);
    }
    return true;
}
}

bool AquilaUITest::ClickText(FAutomationTestBase& Test,const TSharedRef<SWindow>& Window,const TCHAR* Text,FName Scope)
{
    const auto Label=FindText(Window,Text,Scope);
    return Test.TestTrue(Text,Label.IsValid()) &&
        ClickDuelAt(Test,Window,Label->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D(.5,.5)),Text);
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
    const auto* Bridge = FindObjectFast<USceneComponent>(Pawn, TEXT("Bridge"));
    if (!TestNotNull(TEXT("Bridge scene"), Bridge)) return false;
    const bool bExterior = !Bridge->IsVisible();

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
    const FVector StartEye = Camera->GetComponentLocation();
    auto YawFromStart = [&]() { return FMath::FindDeltaAngleDegrees(Start.Yaw, Camera->GetRelativeRotation().Yaw); };

    const auto Down = Slate.RoutePointerDownEvent(Space, ButtonEvent(Center, EKeys::RightMouseButton, Right));
    TestTrue(TEXT("Right drag acquires relative capture"), Down.ShouldUseHighPrecisionMouse() && User->GetPointerCaptor(Pointer) == Surface);
    Move(Command, OverButton, FVector2D(240, -20), Right);
    Pawn->Tick(1.f / 60);
    TestTrue(TEXT("One fast paused drag turns 90 degrees"), FMath::IsNearlyEqual(YawFromStart(), 90., 0.05));
    TestTrue(TEXT("Upward drag tilts the view up"), FMath::IsNearlyEqual(Camera->GetRelativeRotation().Pitch - Start.Pitch, 6., 0.05));
    TestTrue(TEXT("Looking turns around the camera's own position"), Camera->GetComponentLocation().Equals(StartEye, 0.01));
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
    TestFalse(TEXT("Touch interface starts hidden"), FindText(Window, TEXT("DELTA TREE")).IsValid());
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
    TestFalse(TEXT("Hide removes the top bar"), FindText(Window, TEXT("DELTA TREE")).IsValid());
    TestFalse(TEXT("Hide removes the command tabs"), FindText(Window, TEXT("Navigation")).IsValid());
    TestFalse(TEXT("Hide removes bridge movement controls"), FindText(Window, TEXT("Forward")).IsValid());
    return true;
}

// Explicit opt-in because this exercise opens the supplied disposable match at the end of the menu test.
IMPLEMENT_COMPLEX_AUTOMATION_TEST(FAquilaSimulatorTest, "Aquila.Input.Simulator",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::NonNullRHI | EAutomationTestFlags::EngineFilter)

void FAquilaSimulatorTest::GetTests(TArray<FString>& Names, TArray<FString>& Commands) const
{
    FString Directory, Mode;
    const TCHAR* Args = FCommandLine::Get();
    if (!FParse::Param(Args, TEXT("AquilaDuelTest")) || !FParse::Value(Args, TEXT("AquilaSaveDir="), Directory)) return;
    FParse::Value(Args, TEXT("AquilaMode="), Mode);
    Names.Add(Mode);
    Commands.Add(Mode);
}

bool FAquilaSimulatorTest::RunTest(const FString& ModeName)
{
    UGameViewportClient* Viewport = GEngine->GameViewport;
    auto* Game = Viewport && Viewport->GetWorld() ? Viewport->GetWorld()->GetAuthGameMode<AAquilaGameMode>() : nullptr;
    if (!TestTrue(TEXT("Rendered duel window"), Game && Viewport->GetWindow())) return false;
    const auto Window = Viewport->GetWindow().ToSharedRef();
    auto& Slate = FSlateApplication::Get();
    const auto VirtualUser = Slate.FindOrCreateVirtualUser(0);
    const int32 User = VirtualUser->GetUserIndex();
    auto ClickAt = [&](const FVector2D& Position, const TCHAR* Text)
    {
        return ClickDuelAt(*this,Window,Position,Text);
    };
    auto Redraw = [&]() {
        auto* PC = Game->GetWorld()->GetFirstPlayerController();
        CastChecked<AAquilaFlightPawn>(PC->GetPawn())->Tick(0);
        PC->PlayerCameraManager->UpdateCamera(0);
        Slate.ForceRedrawWindow(Window);
        Slate.ForceRedrawWindow(Window);
    };
    auto Click = [&](const TCHAR* Text, FName Scope = NAME_None) {
        Redraw();
        return AquilaUITest::ClickText(*this,Window,Text,Scope);
    };
    const bool bMenu = ModeName.Equals(TEXT("Menu"), ESearchCase::IgnoreCase);
    if (bMenu && !TestTrue(TEXT("Menu precedes match loading"), Game->Session() == EAquilaSession::Menu && Game->Campaign.Sequence == 0))
        return false;
    if (bMenu && !Click(TEXT("Combat Simulator"))) return false;
    if (!TestTrue(TEXT("Duel active"), Game->Session() == EAquilaSession::Simulator)) return false;
    if (!Click(TEXT("Replay duel")) || !Click(TEXT("Speed 8×"))) return false;
    if (!TestFalse(TEXT("Real-time speed selected"), Game->bFastDuel)) return false;
    for (bool bPiercing : {false, true, true})
    {
        const auto ExpectedAmmo = bPiercing ? EAquilaAmmo::Piercing : EAquilaAmmo::Standard;
        if (ExpectedAmmo != Game->Campaign.Loadout.Resolute.Rack->Stock[0].Ammo)
        {
            if (!Click(TEXT("Configure ship")) || !Click(TEXT("07")) || !Click(TEXT("Piercing")) ||
                !Click(TEXT("27")) || !Click(TEXT("Piercing")) || !Click(TEXT("Start duel"))) return false;
        }
        else if (!Click(TEXT("Replay duel"))) return false;
        const auto& Duel = Game->Campaign;
        TestTrue(TEXT("Both ships and all flying rounds reset"), Duel.Shield == 100 && Duel.Hull == 100 &&
            Duel.EnemyShield == 100 && Duel.EnemyHull == 100 && Duel.BattleTime == 0 && Duel.Rounds.IsEmpty());
        TestTrue(TEXT("Chosen ammunition starts at the same origin"), Duel.Loadout.Resolute.Rack->Stock[0].Ammo == ExpectedAmmo && Duel.ShipPosition.IsZero());
        FAquilaCampaign Match;
        Match.Expedition = EAquilaExpedition::Survey;
        Match.SurveySeconds = FAquilaCampaign::SurveyDuration;
        Match.SetPiercing(bPiercing);
        Match.AdvanceExpedition(0); // The ordinary expedition enters its battle at the identical survey position.
        bool bParity = true;
        for (int32 Frame = 0; Frame < 60 * 900 && Match.Expedition == EAquilaExpedition::Battle; ++Frame)
        {
            Game->Tick(1.f/60);
            Match.Advance(1.f/60);
            bParity &= Match.Shield == Duel.Shield && Match.Hull == Duel.Hull && Match.EnemyHull == Duel.EnemyHull &&
                Match.EnemyShield == Duel.EnemyShield && Match.Rounds.Num() == Duel.Rounds.Num() && Match.BattleTime == Duel.BattleTime;
        }
        TestTrue(TEXT("Shields, hull, shot count and clock match the expedition every frame"), bParity);
        TestTrue(TEXT("Ammunition produces its expected result"), bPiercing ? Duel.EnemyHull == 0 : (Duel.bArmorBlocked && Duel.EnemyHull == 100 && Duel.Hull == 0));
        AddInfo(FString::Printf(TEXT("%s at %.2f s: Delta Tree hull %.2f, Glacier Tongue shield %.2f and hull %.2f."),
            bPiercing ? TEXT("Piercing") : TEXT("Standard"), Duel.BattleTime, Duel.Hull, Duel.EnemyShield, Duel.EnemyHull));
        for (int32 Frame = 0; Frame < 25 * 60; ++Frame)
        {
            Game->Tick(1.f/60);
            Match.Advance(1.f/60);
            bParity &= Match.Shield == Duel.Shield && Match.Hull == Duel.Hull && Match.EnemyHull == Duel.EnemyHull &&
                Match.EnemyShield == Duel.EnemyShield && Match.Rounds.Num() == Duel.Rounds.Num();
        }
        TestTrue(TEXT("Last rounds resolve as in the match after disablement"), bParity && Duel.Rounds.IsEmpty() && Duel.Impacts.IsEmpty());
    }
    if (!Click(TEXT("Replay duel")) || !Click(TEXT("Speed 1×"))) return false;
    Game->Tick(0.125f);
    TestTrue(TEXT("8x runs eight identical batches of combat steps"), FMath::IsNearlyEqual(Game->Campaign.BattleTime, 1., 1.e-8));
    if (!Click(TEXT("Pause"))) return false;
    const double Stopped = Game->Campaign.BattleTime;
    Game->Tick(.125f);
    TestEqual(TEXT("Pause freezes the duel"), Game->Campaign.BattleTime, Stopped);
    auto* Flight = Cast<AAquilaFlightPawn>(Game->GetWorld()->GetFirstPlayerController()->GetPawn());
    if (!TestNotNull(TEXT("Shield comparison pawn"), Flight)) return false;
    const FString Frozen = FAquilaSave::Encode(Game->Campaign);
    if (!Click(TEXT("Oval"))) return false;
    TestTrue(TEXT("Oval button shows only the two bubbles"), !Flight->bDuelMeshShield &&
        Flight->ShieldShells[0]->IsVisible() && Flight->ShieldShells[1]->IsVisible() &&
        Flight->ShieldMaterials[0]->K2_GetScalarParameterValue(TEXT("ShellStrength")) == 1 &&
        Flight->ShieldMaterials[1]->K2_GetScalarParameterValue(TEXT("ShellStrength")) == 1 &&
        !Flight->HullShieldRoots[0]->IsVisible() && !Flight->HullShieldRoots[1]->IsVisible());
    if (!Click(TEXT("Ship mesh"))) return false;
    TestTrue(TEXT("Ship mesh button restores the two ship-shaped shields"), Flight->bDuelMeshShield &&
        Flight->HullShieldRoots[0]->IsVisible() && Flight->HullShieldRoots[1]->IsVisible() &&
        Flight->ShieldMaterials[0]->K2_GetScalarParameterValue(TEXT("ShellStrength")) == 0 &&
        Flight->ShieldMaterials[1]->K2_GetScalarParameterValue(TEXT("ShellStrength")) == 0);
    TestTrue(TEXT("Both shape switches preserve pause and every serialized combat value"),
        Game->bPaused && Frozen == FAquilaSave::Encode(Game->Campaign));
    auto CaptureLab = [&](const TCHAR* Name)
    {
        FString Directory;
        if (!FParse::Value(FCommandLine::Get(), TEXT("AquilaLabFrames="), Directory)) return;
        Game->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);
        Viewport->Viewport->Draw(false);
        Slate.ForceRedrawWindow(Window);
        TArray<FColor> Pixels;
        FIntVector Size;
        if (!TestTrue(TEXT("Capture the fitted duel window"), Slate.TakeScreenshot(Window, Pixels, Size))) return;
        TArray64<uint8> Png;
        FImageUtils::PNGCompressImageArray(Size.X, Size.Y, Pixels, Png);
        TestTrue(TEXT("Save the lab view"), FFileHelper::SaveArrayToFile(Png, *(Directory / Name)));
    };
    auto PickMesh = [&](UStaticMeshComponent* Model, FVector2D UV) {
        if (!TestTrue(TEXT("Floating fitting model is visible"), Model && Model->IsVisible() && Model->GetStaticMesh())) return false;
        auto* PC = Game->GetWorld()->GetFirstPlayerController();
        int32 W,H; PC->GetViewportSize(W,H);
        FVector2D Pixel;
        const FVector Pivot = Model->GetComponentTransform().TransformPosition(Model->GetStaticMesh()->GetBoundingBox().GetCenter());
        if (!TestTrue(TEXT("Pointer lands on the model's projected center"), PC->ProjectWorldLocationToScreen(Pivot,Pixel) &&
            (Pixel-UV*FVector2D(W,H)).Size() < 2.)) return false;
        return ClickAt(Flight->MountCanvas->GetCachedGeometry().GetAbsolutePositionAtCoordinates(Pixel/FVector2D(W,H)), TEXT("floating model"));
    };
    auto PickModel = [&](EAquilaWeapon Weapon) {
        Redraw();
        return PickMesh(Flight->TurretPreviews[int32(Weapon)], Flight->TurretPreviewUV(Weapon));
    };
    auto PickDefense = [&](EAquilaDefense Type) {
        Redraw();
        if (!PickMesh(Flight->DefensePreviews[int32(Type)], Flight->DefensePreviewUV(Type))) return false;
        Redraw();
        return true;
    };
    auto PickAmmo = [&](EAquilaAmmo Kind) {
        Redraw();
        if (!PickMesh(Flight->AmmoPreviews[int32(Kind)], Flight->AmmoPreviewUV(Kind))) return false;
        Redraw();
        return true;
    };
    auto AmmoHidden = [&]() {
        for (const auto& Style : AquilaAmmunition::Models)
        {
            const auto* Model = Flight->AmmoPreviews[int32(Style.Type)].Get();
            if ((Model && Model->IsVisible()) || FindText(Window,Style.Name).IsValid()) return false;
        }
        return true;
    };
    auto CheckFinish = [&](bool bEnemy, EAquilaDefense Type) {
        auto* Root = (bEnemy ? Flight->EnemyRoot : Flight->ShipRoot).Get();
        auto* Sample = Flight->DefensePreviews[int32(Type)].Get();
        if (!TestNotNull(TEXT("Authored armor sample exists"), Sample)) return false;
        auto* Plate = Flight->ArmorFinish(bEnemy,static_cast<EAquilaArmor>(Type));
        if (Sample->IsVisible() && Flight->bFitEnemy==bEnemy &&
            !TestTrue(TEXT("Visible armor sample carries this ship's plating"),Sample->GetMaterial(Sample->GetMaterialIndex(TEXT("Plate")))==Plate))
            return false;
        int32 MatchingTiles = 0;
        for (USceneComponent* Child : Root->GetAttachChildren())
        {
            auto* Mesh = Cast<UStaticMeshComponent>(Child);
            if (!Mesh) continue;
            if (Mesh->GetStaticMesh() == Sample->GetStaticMesh() && Mesh->GetMaterial(Mesh->GetMaterialIndex(TEXT("Plate"))) == Plate) ++MatchingTiles;
        }
        auto* Body=Flight->HullBodies[int32(bEnemy)].Get();
        return TestTrue(TEXT("The study's armor surfaces and installed sample share the gallery finish"),
            Body && Body->GetMaterial(Body->GetMaterialIndex(TEXT("Armor")))==Plate &&
            Body->GetMaterial(Body->GetMaterialIndex(TEXT("Spine")))!=Plate && MatchingTiles==1);
    };
    auto PickPort = [&](int32 Part) {
        Redraw();
        return ClickAt(Flight->MountCanvas->GetCachedGeometry().LocalToAbsolute(Flight->FittingPin(Part)), TEXT("physical port"));
    };
    if (!Click(TEXT("Configure ship"))) return false;
    Redraw();
    const double UtilityPortSpan = FMath::Abs(Flight->FittingPin(Flight->ShieldPart).X - Flight->FittingPin(Flight->ArmorPart).X);
    TestTrue(TEXT("The framed hull leaves both turret columns clear on this display"),
        UtilityPortSpan < .58 * Flight->MountCanvas->GetCachedGeometry().GetLocalSize().X);
    FTransform Initial[4];
    TSet<UStaticMesh*> Assets;
    for (const auto& Style : AquilaWeapons::Models)
    {
        const int32 I = int32(Style.Weapon);
        const auto* Model = Flight->TurretPreviews[I].Get();
        if (!TestTrue(TEXT("Each preview loads its authored mesh"), Model && Model->IsVisible() && Model->GetStaticMesh() &&
            Model->GetStaticMesh()->GetName() == Style.Asset)) return false;
        Assets.Add(Model->GetStaticMesh());
        Initial[I] = Model->GetRelativeTransform();
    }
    TestEqual(TEXT("Four distinct floating silhouettes"), Assets.Num(), 4);
    FTransform InitialAmmo[3];
    TSet<UStaticMesh*> Rounds;
    for (const auto& Style : AquilaAmmunition::Models)
    {
        const auto I = int32(Style.Type);
        const auto* Model = Flight->AmmoPreviews[I].Get();
        if (!TestTrue(TEXT("Each lower ammunition choice displays its own cooked model"), Model && Model->IsVisible() &&
            Model->GetStaticMesh() && Model->GetStaticMesh()->GetName()==Style.Asset && !Model->IsCollisionEnabled() &&
            Flight->AmmoPreviewUV(Style.Type).Y>.8 && FindText(Window,Style.Name).IsValid())) return false;
        const auto& Canvas = Flight->MountCanvas->GetCachedGeometry();
        const auto CaptionTop = FindText(Window,Style.Name)->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D(.5,0));
        const double CaptionY = Canvas.AbsoluteToLocal(CaptionTop).Y / Canvas.GetLocalSize().Y;
        TestTrue(FString::Printf(TEXT("%s name sits below the model selector"), Style.Name), CaptionY - Flight->AmmoPreviewUV(Style.Type).Y > .047);
        auto* Light = Cast<UMaterialInstanceDynamic>(Model->GetMaterial(Model->GetMaterialIndex(TEXT("Emitter"))));
        TestTrue(TEXT("The luminous material uses the round's tracer color"), Light && Light->K2_GetVectorParameterValue(TEXT("Color")).Equals(Style.Tracer));
        Rounds.Add(Model->GetStaticMesh());
        InitialAmmo[I] = Model->GetRelativeTransform();
    }
    TestEqual(TEXT("Three distinct ammunition silhouettes"), Rounds.Num(), 3);
    const double EditingTime = Game->Campaign.BattleTime;
    const bool bRecord = FParse::Param(FCommandLine::Get(), TEXT("AquilaGalleryFrames"));
    for (int32 Frame = 0; Frame < (bRecord ? 60 : 1); ++Frame)
    {
        const float Dt = bRecord ? 1.f/12 : .75f;
        Game->Tick(Dt);
        Flight->Tick(Dt);
        if (bRecord) CaptureLab(*FString::Printf(TEXT("gallery-%03d.png"),Frame));
    }
    CaptureLab(TEXT("turret-gallery.png"));
    for (const auto& Style : AquilaWeapons::Models)
    {
        const int32 I = int32(Style.Weapon);
        const auto* Model = Flight->TurretPreviews[I].Get();
        const FVector Center = Model->GetStaticMesh()->GetBoundingBox().GetCenter();
        const auto Now = Model->GetRelativeTransform();
        TestTrue(TEXT("Mesh rotates without center drift or combat advance"), !Now.GetRotation().Equals(Initial[I].GetRotation(),.001) &&
            Now.TransformPosition(Center).Equals(Initial[I].TransformPosition(Center),.01) && Game->Campaign.BattleTime == EditingTime);
    }
    for (const auto& Style : AquilaAmmunition::Models)
    {
        const auto I = int32(Style.Type);
        const auto* Model = Flight->AmmoPreviews[I].Get();
        const FVector Center = Model->GetStaticMesh()->GetBoundingBox().GetCenter();
        const auto Now = Model->GetRelativeTransform();
        TestTrue(TEXT("The round rolls in place over its stationary selector"), !Now.GetRotation().Equals(InitialAmmo[I].GetRotation(),.001) &&
            Now.TransformPosition(Center).Equals(InitialAmmo[I].TransformPosition(Center),.01));
    }
    // Route hover and selection through the packaged viewport, including clicks starting on the physical gun.
    FVector2D LastPointer = Window->GetPositionInScreen();
    auto HoverAt = [&](FVector2D UV) {
        Redraw();
        const auto Pos = Flight->MountCanvas->GetCachedGeometry().GetAbsolutePositionAtCoordinates(UV);
        const FWidgetPath Path(Window->GetHittestGrid().GetBubblePath(Pos,0,false,User));
        const TSet<FKey> Empty;
        Slate.RoutePointerMoveEvent(Path, FPointerEvent(User,FSlateApplication::CursorPointerIndex,Pos,LastPointer,
            Pos-LastPointer,Empty,FModifierKeysState()),false);
        LastPointer = Pos;
        Redraw();
    };
    const FVector2D Blank(.43,.92);
    const auto InitialPayload = Flight->FittedTurret()->Ammo;
    for (const auto& Style : AquilaAmmunition::Models) HoverAt(Flight->AmmoPreviewUV(Style.Type));
    TestTrue(TEXT("Hovering over ammunition leaves the fitting draft alone"), Flight->FittedTurret()->Ammo==InitialPayload &&
        !Flight->PlacementAmmo && Flight->EditingShip()->Rack->Installed()==2);
    if (!PickModel(EAquilaWeapon::PlasmaCaster)) return false;
    Redraw();
    TestTrue(TEXT("Plasma shows neither kinetic rounds nor their selectors"), AmmoHidden());
    if (!PickModel(EAquilaWeapon::PlasmaCaster)) return false;
    HoverAt(Blank);
    for (const auto& Style : AquilaWeapons::Models)
    {
        HoverAt(Flight->TurretPreviewUV(Style.Weapon));
        if (!TestTrue(FString::Printf(TEXT("Hover starts %s (hover %d, selected %d, preview %d, target %d)"),Style.Name,
            Flight->HoveredWeapon ? int32(Flight->HoveredWeapon.GetValue()) : -1,
            Flight->PlacementWeapon ? int32(Flight->PlacementWeapon.GetValue()) : -1,
            Flight->FiringPreview ? int32(Flight->FiringPreview->Gun.Weapon) : -1, int32(Flight->PreviewTarget->IsVisible())), Flight->FiringPreview &&
            Flight->FiringPreview->Gun.Weapon==Style.Weapon && !Flight->PlacementWeapon && Flight->PreviewTarget->IsVisible())) return false;
        auto* PC = Game->GetWorld()->GetFirstPlayerController();
        int32 W,H; PC->GetViewportSize(W,H);
        FVector2D TargetPixel;
        TestTrue(TEXT("The target remains in the open upper center"), PC->ProjectWorldLocationToScreen(Flight->PreviewTarget->GetComponentLocation(),TargetPixel) &&
            (TargetPixel-Flight->PreviewTargetUV()*FVector2D(W,H)).Size()<2);
        Flight->Tick(.20f);
        auto& Ribbon = *Flight->FiringRibbon;
        Redraw();
        const bool bRight = Flight->TurretPreviewUV(Style.Weapon).X>.5;
        const double End = Ribbon.Duration();
        const auto First = Ribbon.FrameAt(.20,bRight), Later = Ribbon.FrameAt(.39,bRight);
        const auto AtRest = Ribbon.FrameAt(End-.99,bRight), Still = Ribbon.FrameAt(End-.01,bRight);
        TestTrue(TEXT("Descriptions move toward their own edge with one second on the final phrase"), End>2 &&
            !First.Text.IsEmpty() && (bRight ? Later.X>First.X : Later.X<First.X) && AtRest.bFinalHold && Still.bFinalHold &&
            AtRest.X==Still.X && AtRest.Text==Still.Text && Ribbon.FrameAt(End+.2,bRight).Text==First.Text);
        TestTrue(TEXT("Gun and mini-duel advance without spending real ammunition"), Flight->FiringPreview->Range.Rounds.Num()>0 &&
            Game->Campaign.BattleTime==EditingTime && Flight->EditingShip()->Rack->Stock.Num()==2);
        HoverAt(Blank);
        if (!TestTrue(TEXT("Leaving an unselected gun clears the sample"), !Flight->FiringPreview && !Flight->PreviewTarget->IsVisible())) return false;
    }
    if (!PickModel(EAquilaWeapon::TwinTurrets)) return false;
    HoverAt(Flight->TurretPreviewUV(EAquilaWeapon::PulseRepeater));
    TestTrue(TEXT("Hover temporarily overrides a pinned gun"), Flight->FiringPreview && Flight->FiringPreview->Gun.Weapon==EAquilaWeapon::PulseRepeater);
    HoverAt(Blank);
    if (!TestTrue(TEXT("Leaving restores the selected burst turret"), Flight->FiringPreview && Flight->FiringPreview->Gun.Weapon==EAquilaWeapon::TwinTurrets)) return false;
    const bool bFilm = FParse::Param(FCommandLine::Get(),TEXT("AquilaFiringFrames"));
    const bool bFilmAmmo = FParse::Param(FCommandLine::Get(),TEXT("AquilaAmmoFrames"));
    int32 AmmoFrame = 0;
    const auto OtherPortAmmo = Flight->EditingShip()->Rack->Stock[1].Ammo;
    for (auto Ammo : {EAquilaAmmo::Standard, EAquilaAmmo::Piercing, EAquilaAmmo::Phase})
    {
        const TCHAR* Button = Ammo == EAquilaAmmo::Standard ? TEXT("Standard") : Ammo == EAquilaAmmo::Piercing ? TEXT("Piercing") : TEXT("Phase");
        if (!PickAmmo(Ammo)) return false;
        TestTrue(TEXT("Clicking the physical round edits only this mounted turret"), Flight->FittedTurret()->Ammo==Ammo &&
            Flight->EditingShip()->Rack->Stock[1].Ammo==OtherPortAmmo);
        if (!TestTrue(TEXT("Ammunition controls update the pinned example"), Flight->FiringPreview && Flight->FiringPreview->Gun.Ammo==Ammo)) return false;
        auto& Demo = *Flight->FiringPreview;
        const int32 InitialPass = Demo.Pass;
        bool bShield = false, bCrossed = false, bPlate = false, bCapturedCrossing = false;
        int32 Frame = 0;
        for (; Frame < 150 && Demo.Pass==InitialPass; ++Frame)
        {
            Game->Tick(1.f/20);
            Flight->Tick(1.f/20);
            const bool bInside = Demo.Range.Rounds.ContainsByPredicate([&](const FAquilaRound& R) {
                const double X = Demo.Range.EnemyPosition.X-R.Position.X;
                return X<Demo.Range.ShieldRadius && X>Demo.Range.HullRadius && Demo.Range.EnemyShield==Demo.ShieldCapacity;
            });
            if (!bCapturedCrossing && Ammo==EAquilaAmmo::Phase && Demo.Range.Rounds.ContainsByPredicate([&](const FAquilaRound& R) {
                const double X = Demo.Range.EnemyPosition.X-R.Position.X;
                return !R.bEnemy && X<(Demo.Range.HullRadius+Demo.Range.ShieldRadius)*.5 && X>Demo.Range.HullRadius;
            }))
            {
                CaptureLab(TEXT("preview-phase-crossing.png"));
                bCapturedCrossing = true;
            }
            bCrossed |= bInside;
            bShield |= Demo.Range.EnemyShield<Demo.ShieldCapacity;
            const bool bResult = Ammo==EAquilaAmmo::Piercing ? Demo.Range.EnemyHull<Demo.HullCapacity && Demo.Range.EnemyShield==0 :
                Demo.Range.DuelStats->Shots[0].Blocked>0 && Demo.Range.EnemyHull==Demo.HullCapacity;
            if (bResult && !bPlate)
            {
                TestTrue(TEXT("Only bypassed shields stay visible at the armor contact"),
                    Flight->PreviewShield->IsVisible()==(Ammo==EAquilaAmmo::Phase));
                CaptureLab(*FString::Printf(TEXT("preview-%s-impact.png"),Button));
            }
            bPlate |= bResult;
            if (Ammo==EAquilaAmmo::Phase && Frame==16) CaptureLab(TEXT("round-models.png"));
            if (bFilmAmmo) CaptureLab(*FString::Printf(TEXT("rounds-%03d.png"),AmmoFrame++));
            if (bFilm) CaptureLab(*FString::Printf(TEXT("preview-%s-%03d.png"),Button,Frame));
        }
        TestTrue(TEXT("A complete rendered firing loop teaches the configured payload"), Demo.Pass==InitialPass+1 && bPlate &&
            bShield==(Ammo!=EAquilaAmmo::Phase) && bCrossed==(Ammo==EAquilaAmmo::Phase) && Game->Campaign.BattleTime==EditingTime);
        AddInfo(FString::Printf(TEXT("Rendered %s sample: %d frames at 20 fps, target reset."),Button,Frame));
        if (bFilm && Ammo==EAquilaAmmo::Standard)
        {
            const double ReadToEnd = Flight->FiringRibbon->Duration()+.8;
            for (; Demo.DescriptionAge < ReadToEnd && Frame < 800; ++Frame)
            {
                Flight->Tick(1.f/20);
                CaptureLab(*FString::Printf(TEXT("preview-Standard-%03d.png"),Frame));
            }
            TestTrue(TEXT("Standard clip includes the entire description, final hold and restart"), Demo.DescriptionAge >= ReadToEnd);
        }
    }
    if (!Click(TEXT("Done placing")) || !PickPort(29)) return false;
    Redraw();
    TestTrue(TEXT("An empty port has no ammunition row until a compatible turret is selected"), AmmoHidden());
    if (!PickModel(EAquilaWeapon::KineticCannon) || !PickAmmo(EAquilaAmmo::Phase)) return false;
    Redraw();
    TestTrue(TEXT("A bare port can preview new ammunition without borrowing or changing another gun"), Flight->FiringPreview &&
        Flight->FiringPreview->Gun.Ammo==EAquilaAmmo::Phase && Flight->EditingShip()->Rack->At(29)==INDEX_NONE);
    if (!PickPort(6) || !TestTrue(TEXT("An explicitly previewed payload follows the next successful fitting"),
        Flight->FittedTurret() && Flight->FittedTurret()->Weapon==EAquilaWeapon::KineticCannon && Flight->FittedTurret()->Ammo==EAquilaAmmo::Phase)) return false;
    if (!Click(TEXT("Glacier Tongue"))) return false;
    HoverAt(Flight->TurretPreviewUV(EAquilaWeapon::KineticCannon));
    TestTrue(TEXT("The other ship previews its own ammunition"), Flight->FiringPreview && Flight->FiringPreview->Gun.Ammo==EAquilaAmmo::Standard);
    if (!PickAmmo(EAquilaAmmo::Piercing)) return false;
    TestTrue(TEXT("A Glacier Tongue model click changes its own preview and preserves Delta Tree's loadout"), Flight->FiringPreview &&
        Flight->FiringPreview->Gun.Ammo==EAquilaAmmo::Piercing && Game->Campaign.Loadout.Resolute.Rack->Stock[0].Ammo==EAquilaAmmo::Phase);
    if (!Click(TEXT("Armor"))) return false;
    Redraw();
    TestTrue(TEXT("Ammunition, target and weapon text leave the defense view"), AmmoHidden() && !Flight->FiringPreview && !Flight->PreviewTarget->IsVisible());
    if (!Click(TEXT("Cancel"))) return false;
    Redraw();
    if (!TestTrue(TEXT("Closing fitting keeps both rounds and the frozen encounter"), Game->bPaused && !Game->bFittingOpen &&
        Game->Campaign.BattleTime==EditingTime && Game->Campaign.Loadout.Resolute.Rack->Stock[0].Ammo==EAquilaAmmo::Phase &&
        Game->Campaign.Loadout.Sentinel.Rack->Stock[0].Ammo==EAquilaAmmo::Piercing)) return false;
    if (!Click(TEXT("Configure ship")) || !Click(TEXT("Delta Tree")) || !PickModel(EAquilaWeapon::TwinTurrets) || !PickPort(6) ||
        !PickAmmo(EAquilaAmmo::Piercing) || !Click(TEXT("Glacier Tongue")) || !PickPort(14) || !PickAmmo(EAquilaAmmo::Standard) ||
        !Click(TEXT("Delta Tree"))) return false;
    if (!PickPort(Flight->ArmorPart)) return false;
    for (const auto& Style : AquilaDefenses::Models)
    {
        if (!AquilaDefenses::IsPlating(Style.Type)) continue;
        if (!PickDefense(Style.Type) || !CheckFinish(false, Style.Type)) return false;
        const auto* Model = Flight->DefensePreviews[int32(Style.Type)].Get();
        TestTrue(TEXT("Armor selection replaces turret tools without advancing combat"),
            !Flight->TurretPreviews[0]->IsVisible() && !FindText(Window,TEXT("Burst turret")).IsValid() &&
            Model->GetStaticMesh()->GetName() == Style.Asset && Game->Campaign.BattleTime == EditingTime &&
            AquilaDefenses::Plate(Flight->EditingShip()->Armor) == Style.Type);
        const auto Before = Model->GetRelativeTransform();
        Flight->Tick(.35f);
        const auto After = Model->GetRelativeTransform();
        const auto Mid = Model->GetStaticMesh()->GetBoundingBox().GetCenter();
        TestTrue(TEXT("Each armor sample rotates in place"), !Before.GetRotation().Equals(After.GetRotation(),.001) &&
            Before.TransformPosition(Mid).Equals(After.TransformPosition(Mid),.02));
        CaptureLab(*FString::Printf(TEXT("armor-%d.png"),int32(Style.Type)));
    }
    if (!PickPort(Flight->ShieldPart) || !PickDefense(EAquilaDefense::EmptySocket) || !PickDefense(EAquilaDefense::ReserveCapacitor)) return false;
    TestTrue(TEXT("An empty socket rejects the capacitor and hides the shell"), !Flight->EditingShip()->bShield &&
        !Flight->EditingShip()->bReserveCapacitor && !Flight->MountedReserve[0] && !Flight->HullShieldRoots[0]->IsVisible());
    if (!PickDefense(EAquilaDefense::ShieldEmitter) || !PickDefense(EAquilaDefense::ReserveCapacitor)) return false;
    TestTrue(TEXT("Standard shield and optional cell fit together on the selected ship"), Flight->EditingShip()->bShield &&
        Flight->EditingShip()->bReserveCapacitor && Flight->MountedReserve[0] && Flight->HullShieldRoots[0]->IsVisible() &&
        !Flight->HullShieldRoots[1]->IsVisible() && !Flight->DefensePreviews[0]->IsVisible() &&
        Flight->MountedReserve[0]->GetStaticMesh() == Flight->DefensePreviews[int32(EAquilaDefense::ReserveCapacitor)]->GetStaticMesh());
    CaptureLab(TEXT("shield-fitting.png"));
    if (!PickDefense(EAquilaDefense::ReserveCapacitor)) return false;
    TestTrue(TEXT("Removing only the enhancement leaves the shield"), Flight->EditingShip()->bShield && !Flight->MountedReserve[0]);
    if (!PickDefense(EAquilaDefense::ReserveCapacitor) || !PickDefense(EAquilaDefense::EmptySocket) || !PickDefense(EAquilaDefense::ShieldEmitter)) return false;
    TestFalse(TEXT("Removing the generator clears its enhancement"), Flight->EditingShip()->bReserveCapacitor);
    if (!Click(TEXT("Turrets")) || !PickPort(6)) return false;
    if (!PickModel(EAquilaWeapon::KineticCannon)) return false;
    auto& FirstRack = Game->Campaign.Loadout.Resolute.Rack.GetValue();
    TestTrue(TEXT("Choosing a model alone changes no fitted turret"), Flight->PlacementWeapon == EAquilaWeapon::KineticCannon &&
        FirstRack.Stock[0].Weapon == EAquilaWeapon::TwinTurrets);
    if (!PickPort(29)) return false;
    TestTrue(TEXT("An empty port remains inspectable with no spare"), Flight->SelectedPart == 29 && FirstRack.Installed() == 2 && FirstRack.At(29) == INDEX_NONE);
    if (!PickPort(6)) return false;
    TestTrue(TEXT("One physical port click immediately refits the ship"), FirstRack.Stock[0].Weapon == EAquilaWeapon::KineticCannon &&
        FirstRack.At(6)==0 && Game->bFittingOpen);
    Redraw();
    auto* Mount = Flight->ShipRoot->GetAttachChildren().FindByPredicate([&](const TObjectPtr<USceneComponent>& Part) {
        return Part->ComponentHasTag(AquilaHulls::GunSocket(6)) &&
            Part->GetRelativeTransform().Equals(FAquilaTurret{6,EAquilaWeapon::KineticCannon}.MountedPose(false),.01);
    });
    TestTrue(TEXT("Fitted turret uses the same mesh as its floating model"), Mount && CastChecked<UStaticMeshComponent>(Mount->Get())->GetStaticMesh() ==
        Flight->TurretPreviews[int32(EAquilaWeapon::KineticCannon)]->GetStaticMesh());
    CaptureLab(TEXT("turret-placed.png"));
    if (!PickModel(EAquilaWeapon::KineticCannon) || !PickPort(26)) return false;
    TestTrue(TEXT("Clicking the selected model again releases placement"), !Flight->PlacementWeapon.IsSet() &&
        FirstRack.Stock[1].Weapon == EAquilaWeapon::TwinTurrets);
    if (!PickModel(EAquilaWeapon::PulseRepeater) || !Click(TEXT("Glacier Tongue")) || !PickPort(14)) return false;
    TestTrue(TEXT("Changing ships discards the placement tool and leaves opposing gear intact"), !Flight->PlacementWeapon.IsSet() &&
        Game->Campaign.Loadout.Sentinel.Rack->Stock[0].Weapon == EAquilaWeapon::KineticCannon);
    if (!Click(TEXT("Delta Tree")) || !PickModel(EAquilaWeapon::KineticCannon)) return false;
    if (!Click(TEXT("Done placing")) || !Click(TEXT("30")) || !Click(TEXT("Move a turret here")) ||
        !Click(TEXT("07")) || !Click(TEXT("Cancel"))) return false;
    Redraw();
    TestTrue(TEXT("Cancel exits with the moved cannon and the same paused encounter"), Game->bPaused && !Game->bFittingOpen &&
        Game->Campaign.BattleTime==EditingTime && FirstRack.At(29)==0 && FirstRack.Stock[0].Weapon==EAquilaWeapon::KineticCannon &&
        !Flight->TurretPreviews[0]->IsVisible() && !Flight->TurretPreviews[3]->IsVisible());
    if (!CheckFinish(false, EAquilaDefense::PrismPlate)) return false;
    if (!Click(TEXT("Configure ship")) || !PickPort(6) || !Click(TEXT("Move a turret here")) || !PickPort(29)) return false;
    const auto& Fitting = Game->Campaign.Loadout;
    for (int32 I = 0; I < 30; ++I)
    {
        const FString Pin = FString::Printf(TEXT("%02d"), I+1);
        if (!Click(*Pin) || !TestEqual(TEXT("Click selects the matching physical mount"), Flight->SelectedPart, I)) return false;
    }
    Slate.ForceRedrawWindow(Window);
    TestTrue(TEXT("Last empty mount remains inspectable with all stock installed"),
        FindText(Window,TEXT("Empty mount · No spare turrets")).IsValid() && Fitting.Resolute.Rack->At(29) == INDEX_NONE);
    if (!Click(TEXT("07")) || !Click(TEXT("Fit interception chip")) || !Click(TEXT("30")) ||
        !Click(TEXT("Move a turret here")) || !Click(TEXT("07"))) return false;
    TestTrue(TEXT("Moving carries the chip and consumes no extra inventory"), Fitting.Resolute.Rack->At(6) == INDEX_NONE &&
        Fitting.Resolute.Rack->At(29) == 0 && Fitting.Resolute.Rack->Stock[0].bIntercept && Fitting.Resolute.Rack->Installed() == 2);
    if (!PickModel(EAquilaWeapon::PulseRepeater) || !PickPort(29)) return false;
    Flight->Tick(0);
    CaptureLab(TEXT("ship-fitting.png"));
    if (!Click(TEXT("Armor")) || !Click(TEXT("Reinforced armor")) || !Click(TEXT("Shield")) || !PickDefense(EAquilaDefense::EmptySocket)) return false;
    if (!Click(TEXT("Glacier Tongue")) || !PickModel(EAquilaWeapon::PlasmaCaster) || !PickPort(14) || !Click(TEXT("Shield")) ||
        !PickDefense(EAquilaDefense::EmptySocket) || !Click(TEXT("Start duel"))) return false;
    auto& Duel = Game->Campaign;
    TestTrue(TEXT("Both opponents receive their selected items and defenses"), Duel.Loadout.Resolute.Rack->At(29) == 0 &&
        Duel.Loadout.Resolute.Rack->Stock[0].Weapon == EAquilaWeapon::PulseRepeater && Duel.Loadout.Resolute.Rack->Stock[0].bIntercept &&
        Duel.Loadout.Sentinel.Rack->Stock[0].Weapon == EAquilaWeapon::PlasmaCaster && Duel.Shield == 0 && Duel.EnemyShield == 0);
    for (int32 I = 0; I < 60; ++I) Game->Tick(.125f);
    Flight->Tick(0);
    const int32 Stops = Duel.DuelStats->Shots[0].Intercepted;
    TestTrue(TEXT("Installed chip defeats incoming rounds during a normal duel"), Stops > 0);
    AddInfo(FString::Printf(TEXT("Mount trial at 60 s: %d projectiles intercepted, Delta Tree hull %.2f, Glacier Tongue hull %.2f."),
        Stops, Duel.Hull, Duel.EnemyHull));
    CaptureLab(TEXT("ship-defense.png"));
    if (!Click(TEXT("Replay duel"))) return false;
    TestTrue(TEXT("Replay retains the relocated chip and resets counters"), Duel.Loadout.Resolute.Rack->At(29) == 0 &&
        Duel.Loadout.Resolute.Rack->Stock[0].bIntercept && Duel.DuelStats->Shots[0].Intercepted == 0);
    if (!Click(TEXT("Configure ship")) || !Click(TEXT("Delta Tree")) || !PickModel(EAquilaWeapon::TwinTurrets) || !PickPort(29) ||
        !Click(TEXT("Phase")) || !Click(TEXT("27")) || !Click(TEXT("Phase")) ||
        !Click(TEXT("Armor")) || !Click(TEXT("Light plating")) || !Click(TEXT("Shield")) || !PickDefense(EAquilaDefense::ShieldEmitter) ||
        !Click(TEXT("Glacier Tongue")) || !PickModel(EAquilaWeapon::KineticCannon) || !PickPort(14) || !Click(TEXT("Phase")) ||
        !Click(TEXT("Armor")) || !Click(TEXT("Light plating")) || !Click(TEXT("Shield")) || !PickDefense(EAquilaDefense::ShieldEmitter) ||
        !PickPort(14)) return false;
    Flight->Tick(0);
    CaptureLab(TEXT("phase-fitting.png"));
    if (!Click(TEXT("Start duel"))) return false;
    TestTrue(TEXT("Both ships start with phase rounds in their selected mounts"), Duel.Loadout.Resolute.Rack->Stock[0].Ammo == EAquilaAmmo::Phase &&
        Duel.Loadout.Resolute.Rack->Stock[1].Ammo == EAquilaAmmo::Phase && Duel.Loadout.Sentinel.Rack->Stock[0].Ammo == EAquilaAmmo::Phase);
    for (int32 I = 0; I < 1200 && Duel.EnemyHull == 100; ++I) Game->Tick(1.f/120);
    TestTrue(TEXT("First live phase impact leaves Glacier Tongue's shield full"), Duel.EnemyHull < 100 && Duel.EnemyShield == 100 &&
        Duel.Impacts.ContainsByPredicate([](const FAquilaImpact& Hit) { return Hit.bTargetEnemy && Hit.bHull && !Hit.bShield; }));
    for (int32 I = 0; I < 1200 && Duel.EnemyHull > 96; ++I) Game->Tick(1.f/120);
    Flight->Tick(1.f/120);
    CaptureLab(TEXT("phase-impact.png"));
    AddInfo(FString::Printf(TEXT("Phase trial: Glacier Tongue shield %.2f, hull %.2f."), Duel.EnemyShield, Duel.EnemyHull));
    for (bool bFast : {false, true})
    {
        if (!Click(TEXT("Replay duel"))) return false;
        if (Game->bFastDuel != bFast && !Click(Game->bFastDuel ? TEXT("Speed 8×") : TEXT("Speed 1×"))) return false;
        Duel.EnemyHull = .1; // Only the starting health changes. Both batteries fire their own live shots.
        for (int32 I = 0; I < 1200 && Duel.EnemyHull > 0; ++I) Game->Tick(1.f/120);
        TSet<uint64> Residual;
        for (const auto& R : Duel.Rounds) if (R.bEnemy && !R.bClaimed) Residual.Add(R.Id);
        const uint64 FinalLaunch = Duel.DuelStats->NextRoundId;
        const int32 Before = Duel.DuelStats->Shots[0].Intercepted;
        bool bNewClaim = false;
        bool bSawIncoming = false;
        for (int32 I = 0; I < 3000 && !Duel.Rounds.IsEmpty(); ++I)
        {
            Game->Tick(1.f/120);
            for (const auto& R : Duel.Rounds) bNewClaim |= R.bClaimed && Residual.Contains(R.Id);
            if (!bFast && !bSawIncoming && Duel.DuelStats->ScreenShots.ContainsByPredicate([&Residual](const FAquilaScreenShot& Bolt) {
                return Residual.Contains(Bolt.TargetId) && Bolt.Time >= .5 * Bolt.Duration;
            }))
            {
                Flight->Tick(1.f/120);
                Slate.ForceRedrawWindow(Window);
                bSawIncoming = FindText(Window, TEXT("Glacier Tongue destroyed. Incoming rounds still in flight.")).IsValid();
                CaptureLab(TEXT("last-volley.png"));
            }
        }
        const int32 After = Duel.DuelStats->Shots[0].Intercepted - Before;
        if (!bFast) TestTrue(TEXT("HUD shows the incoming volley after the explosion"), bSawIncoming);
        TestTrue(TEXT("Survivor acquires and intercepts a real shot launched before destruction"), Duel.EnemyHull == 0 && bNewClaim && After > 0);
        TestTrue(TEXT("Combat resolves every remaining shot without firing at a wreck"), Duel.Rounds.IsEmpty() && Duel.DuelStats->NextRoundId == FinalLaunch);
        AddInfo(FString::Printf(TEXT("Last volley at %dx: %d unclaimed when Glacier Tongue fell, %d interceptions after destruction, Delta Tree hull %.2f."),
            bFast ? 8 : 1, Residual.Num(), After, Duel.Hull));
    }
    if (!Click(TEXT("Configure ship")) || !Click(TEXT("Delta Tree")) || !PickPort(29) || !Click(TEXT("Remove interception chip")) ||
        !PickPort(Flight->ArmorPart) || !PickDefense(EAquilaDefense::PrismPlate) ||
        !PickPort(Flight->ShieldPart) || !PickDefense(EAquilaDefense::ReserveCapacitor) ||
        !Click(TEXT("Glacier Tongue")) || !PickPort(14) || !Click(TEXT("Standard")) ||
        !PickPort(Flight->ArmorPart) || !PickDefense(EAquilaDefense::ReinforcedPlate) ||
        !PickPort(Flight->ShieldPart) || !PickDefense(EAquilaDefense::ReserveCapacitor) || !Click(TEXT("Start duel"))) return false;
    for (bool bFast : {false,true})
    {
        if (!Click(TEXT("Replay duel"))) return false;
        if (Game->bFastDuel != bFast && !Click(Game->bFastDuel ? TEXT("Speed 8×") : TEXT("Speed 1×"))) return false;
        Redraw();
        if (!CheckFinish(false, EAquilaDefense::PrismPlate) || !CheckFinish(true, EAquilaDefense::ReinforcedPlate)) return false;
        if (!TestTrue(TEXT("Replay rearms both fitted capacitors and hides all fitting samples"),
            Duel.Shield == 100 && Duel.EnemyShield == 100 && !Duel.DuelStats->bReserveSpent[0] && !Duel.DuelStats->bReserveSpent[1] &&
            Flight->MountedReserve[0] && Flight->MountedReserve[1] &&
            !Flight->DefensePreviews[int32(EAquilaDefense::ReserveCapacitor)]->IsVisible())) return false;
        Duel.Shield = 10. * FAquilaCampaign::ExpeditionDamageScale; // Exactly one of the Sentinel's live cannon hits.
        for (int32 I = 0; I < 2400 && !Duel.DuelStats->bReserveSpent[0]; ++I) Game->Tick(1.f/120);
        Redraw();
        auto* Cell = Flight->MountedReserve[0].Get();
        TestTrue(TEXT("The live shield refills once at zero, dims its cell, and shows the spent charge"), Duel.Shield == 100 && Duel.Hull == 100 &&
            Duel.DuelStats->bReserveSpent[0] && !Duel.DuelStats->bReserveSpent[1] &&
            Cell->GetMaterial(Cell->GetMaterialIndex(TEXT("Emitter"))) == Flight->Materials[1] &&
            FindText(Window,TEXT("Prism plating  ·  Shield · Reserve spent")).IsValid());
        if (!bFast) CaptureLab(TEXT("reserve-spent.png"));
        for (int32 I = 0; I < 72000 && Duel.Hull == 100; ++I) Game->Tick(1.f/120);
        TestTrue(TEXT("Later standard fire reaches the hull with the reserve exhausted"), Duel.Shield == 0 && Duel.Hull < 100);
    }
    if (!Click(TEXT("Configure ship")) || !Click(TEXT("Borrow full lab stock")) || !PickModel(EAquilaWeapon::TwinTurrets)) return false;
    for (int32 I = 0; I < 30; ++I)
    {
        const FString Pin = FString::Printf(TEXT("%02d"), I+1);
        if (!Click(*Pin)) return false;
    }
    if (!Click(TEXT("Start duel"))) return false;
    TestEqual(TEXT("All thirty player-fitted mounts start the duel"), Duel.Loadout.Resolute.Rack->Installed(), 30);
    TestEqual(TEXT("Borrowing on one ship leaves the opposing stock intact"), Duel.Loadout.Sentinel.Rack->Stock.Num(), 1);
    auto BareShip = [&](bool bEnemy, int32 Bodies) {
        const int32 Side = bEnemy ? 1 : 0;
        const auto& Fit = Game->Campaign.Loadout.ForShip(bEnemy);
        const auto* Root = (bEnemy ? Flight->EnemyRoot : Flight->ShipRoot).Get();
        for (const auto& Part : Root->GetAttachChildren())
        {
            if (Part->ComponentHasTag(TEXT("FittedTurret")) || Part->ComponentHasTag(TEXT("FittedDefense"))) return false;
        }
        const auto* Body=Flight->HullBodies[Side].Get();
        return Body && Body->GetMaterial(Body->GetMaterialIndex(TEXT("Armor")))==Flight->ArmorFinish(bEnemy,EAquilaArmor::BareHull) &&
            Fit.Rack && Fit.Rack->Stock.Num()==Bodies && !Fit.Rack->Stock.ContainsByPredicate(
            [](const FAquilaTurret& Body) { return Body.Mount!=INDEX_NONE || Body.bIntercept || Body.Ammo!=EAquilaAmmo::Standard; }) &&
            Fit.Armor==EAquilaArmor::BareHull && !Fit.bShield && !Fit.bReserveCapacitor &&
            !Flight->MountedReserve[Side] && !Flight->HullShieldRoots[Side]->IsVisible();
    };
    for (int32 Frame = 0; Frame < 120 && Duel.Rounds.IsEmpty(); ++Frame) Game->Tick(1.f/120);
    if (!TestTrue(TEXT("A real volley is in flight before opening the fitting screen"), !Duel.Rounds.IsEmpty())) return false;
    const auto EncounterId = Duel.CampaignId;
    const auto ClearTime = Duel.BattleTime;
    const auto FlyingId = Duel.Rounds[0].Id;
    const auto FlyingPosition = Duel.Rounds[0].Position;
    const auto FlyingCount = Duel.Rounds.Num();
    if (!Click(TEXT("Pause")) || !Click(TEXT("Configure ship")) || !Click(TEXT("Delta Tree"))) return false;
    Redraw();
    CaptureLab(TEXT("full-fitting.png"));
    if (!PickModel(EAquilaWeapon::KineticCannon) || !PickAmmo(EAquilaAmmo::Phase) || !Click(TEXT("Clear fitting"))) return false;
    Redraw();
    if (!TestTrue(TEXT("Clear strips all thirty mounts and active tools from only Delta Tree"), BareShip(false,30) &&
        Duel.Shield==0 && Duel.EnemyShield==100 && Duel.Loadout.Resolute.Rack->Installed()==0 && Duel.CampaignId==EncounterId &&
        !Flight->PlacementWeapon && !Flight->PlacementAmmo && !Flight->FiringPreview && AmmoHidden() &&
        Game->Campaign.Loadout.Sentinel.Rack->Installed()==1 && Game->Campaign.Loadout.Sentinel.bReserveCapacitor &&
        FindText(Window,TEXT("0 installed  ·  30 in stock")).IsValid())) return false;
    CaptureLab(TEXT("clear-fitting.png"));
    if (!Click(TEXT("Clear fitting")) || !PickPort(29) || !PickModel(EAquilaWeapon::KineticCannon) || !PickPort(29)) return false;
    if (!TestTrue(TEXT("A returned body can be refitted with clean ammunition and no chip"), Flight->FittedTurret() &&
        Flight->FittedTurret()->Weapon==EAquilaWeapon::KineticCannon && Flight->FittedTurret()->Ammo==EAquilaAmmo::Standard &&
        !Flight->FittedTurret()->bIntercept && Flight->EditingShip()->Rack->Installed()==1)) return false;
    if (!PickPort(Flight->ShieldPart) || !PickDefense(EAquilaDefense::ShieldEmitter) || !PickDefense(EAquilaDefense::ReserveCapacitor)) return false;
    TestTrue(TEXT("Shield hardware can be fitted directly to a bare hull"), Flight->EditingShip()->Armor==EAquilaArmor::BareHull &&
        Flight->MountedReserve[0] && Flight->HullShieldRoots[0]->IsVisible());
    if (!Click(TEXT("Clear fitting"))) return false;
    Redraw();
    TestTrue(TEXT("Clear also strips shield hardware from the open shield view"), BareShip(false,30));
    if (!Click(TEXT("Cancel"))) return false;
    Redraw();
    if (!TestTrue(TEXT("Cancel closes fitting without rolling back Clear or starting a duel"), Game->bPaused && !Game->bFittingOpen &&
        Duel.CampaignId==EncounterId && Duel.BattleTime==ClearTime && Duel.Rounds.Num()==FlyingCount &&
        Duel.Rounds[0].Id==FlyingId && Duel.Rounds[0].Position==FlyingPosition && Duel.Hull==100 &&
        Duel.Loadout.Resolute.Rack->Installed()==0 && Duel.Shield==0 && Duel.Loadout.Resolute.Armor==EAquilaArmor::BareHull &&
        Duel.Loadout.Sentinel.Rack->Installed()==1 && Duel.EnemyShield==100 && FindText(Window,TEXT("Delta Tree has no turrets installed.")).IsValid())) return false;
    AddInfo(FString::Printf(TEXT("Clear and Cancel retained %d flying rounds at %.3f seconds."), FlyingCount, ClearTime));
    CaptureLab(TEXT("closed-after-clear.png"));
    if (!Click(TEXT("Configure ship"))) return false;
    Redraw();
    if (!TestTrue(TEXT("Reopening shows the same bare ship and all thirty spares"), BareShip(false,30) &&
        FindText(Window,TEXT("0 installed  ·  30 in stock")).IsValid())) return false;
    CaptureLab(TEXT("reopened-after-clear.png"));
    if (!PickModel(EAquilaWeapon::TwinTurrets) || !PickPort(6) || !PickPort(Flight->ShieldPart) ||
        !PickDefense(EAquilaDefense::ShieldEmitter) || !PickDefense(EAquilaDefense::ReserveCapacitor) ||
        !PickPort(Flight->ArmorPart) || !PickDefense(EAquilaDefense::LightPlate) || !Click(TEXT("Glacier Tongue")) ||
        !PickPort(Flight->ArmorPart) || !Click(TEXT("Clear fitting"))) return false;
    Redraw();
    if (!TestTrue(TEXT("Clearing Glacier Tongue immediately keeps Delta Tree's independent rebuild"), BareShip(true,1) && Duel.EnemyShield==0 &&
        FindText(Window,TEXT("Bare hull · Choose hull plating")).IsValid() && Duel.Loadout.Resolute.Rack->Installed()==1 &&
        Duel.Loadout.Resolute.bReserveCapacitor && Duel.Loadout.Resolute.Armor==EAquilaArmor::Light)) return false;
    if (!PickDefense(EAquilaDefense::ReinforcedPlate) || !CheckFinish(true, EAquilaDefense::ReinforcedPlate) ||
        !Click(TEXT("Clear fitting")) || !Click(TEXT("Cancel")) || !Click(TEXT("Configure ship"))) return false;
    Redraw();
    if (!TestTrue(TEXT("Reopening returns to the cleared Glacier Tongue with its one spare"), Flight->bFitEnemy &&
        BareShip(true,1) && Duel.Loadout.Resolute.Rack->Installed()==1)) return false;
    if (!Click(TEXT("Start duel"))) return false;
    Redraw();
    TestTrue(TEXT("The duel identifies the unarmed, unshielded ship immediately"), FindText(Window,TEXT("Bare hull  ·  No shield")).IsValid() &&
        FindText(Window,TEXT("Glacier Tongue has no turrets installed.")).IsValid() && Duel.Shield==100 && Duel.EnemyShield==0);
    for (int32 I = 0; I<3600 && Duel.EnemyHull==100; ++I) Game->Tick(1.f/60);
    TestTrue(TEXT("Incoming fire reaches bare hull without a return volley"), Duel.EnemyHull<100 && Duel.Shield==100 &&
        Duel.Loadout.Sentinel.Rack->Installed()==0 && Duel.DuelStats->Shots[0].Hits>0 && Duel.DuelStats->Shots[1].Hits==0);
    if (!Click(TEXT("Replay duel")) || !Click(TEXT("Configure ship")) || !Click(TEXT("Delta Tree")) || !Click(TEXT("Clear fitting")) ||
        !Click(TEXT("Start duel")) || !Click(TEXT("Replay duel"))) return false;
    Game->Tick(.2f);
    Redraw();
    if (!TestTrue(TEXT("Starting and replaying two bare ships never reinstalls starter gear"), Duel.Loadout.Resolute.Rack->Installed()==0 &&
        Duel.Loadout.Sentinel.Rack->Installed()==0 && Duel.Rounds.IsEmpty() && Duel.Shield==0 && Duel.EnemyShield==0 &&
        FindText(Window,TEXT("Both ships have empty mounts.")).IsValid())) return false;
    Game->Command(9);
    Game->Command(11);
    TestTrue(TEXT("Arena ignores save and expedition commands"), Game->Campaign.Sequence == 0 && Game->Campaign.Expedition == EAquilaExpedition::Battle);
    const auto IdleEncounter = Duel.CampaignId;
    const auto IdleTime = Duel.BattleTime;
    if (!Click(TEXT("Configure ship")) || !Click(TEXT("Delta Tree")) || !PickModel(EAquilaWeapon::TwinTurrets) || !PickPort(6) ||
        !Click(TEXT("Glacier Tongue")) || !PickModel(EAquilaWeapon::KineticCannon) || !PickPort(14) || !PickPort(Flight->ArmorPart) ||
        !PickDefense(EAquilaDefense::ReinforcedPlate) || !Click(TEXT("Cancel"))) return false;
    if (!TestTrue(TEXT("Closing fitting leaves the same running encounter after rearming a stalemate"), !Game->bPaused &&
        !Game->bFittingOpen && Duel.CampaignId==IdleEncounter && Duel.BattleTime==IdleTime)) return false;
    for (int32 Frame = 0; Frame < 36000 && Duel.Hull>0; ++Frame) Game->Tick(.125f);
    if (!TestTrue(TEXT("Refitted guns can resolve the existing encounter"), Duel.Hull==0 && Duel.EnemyHull==100)) return false;
    const auto FinishedTime = Duel.BattleTime;
    const auto FinishedStage = Duel.Expedition;
    const auto EnemyHits = Duel.DuelStats->Shots[1].Hits;
    if (!Click(TEXT("Configure ship")) || !Click(TEXT("Delta Tree")) || !Click(TEXT("Clear fitting")) || !Click(TEXT("Cancel")) ||
        !Click(TEXT("Configure ship")) || !Click(TEXT("Delta Tree"))) return false;
    Redraw();
    if (!TestTrue(TEXT("A defeated ship stays cleared on reentry without reviving or restarting either side"), BareShip(false,30) &&
        Duel.CampaignId==IdleEncounter && Duel.BattleTime==FinishedTime && Duel.Expedition==FinishedStage && Duel.Hull==0 &&
        Duel.EnemyHull==100 && Duel.DuelStats->Shots[1].Hits==EnemyHits && Duel.Loadout.Sentinel.Rack->Installed()==1)) return false;
    if (!Click(TEXT("Cancel"))) return false;
    if (!Click(TEXT("Main menu")) || !TestTrue(TEXT("Menu restored"), Game->Session() == EAquilaSession::Menu)) return false;
    if (bMenu)
    {
        FString Directory, Status;
        bool bWritable = false;
        FParse::Value(FCommandLine::Get(), TEXT("AquilaSaveDir="), Directory);
        FAquilaCampaign Saved;
        if (!TestTrue(TEXT("Disposable match fixture"), FAquilaSave::LoadJournal(Directory, Saved, Status, bWritable) && bWritable)) return false;
        if (!Click(TEXT("Play Match"))) return false;
        TestTrue(TEXT("Match resumes with the entire saved state"), Game->Session() == EAquilaSession::Match &&
            FAquilaSave::Encode(Game->Campaign) == FAquilaSave::Encode(Saved));
    }
    return true;
}
#endif
