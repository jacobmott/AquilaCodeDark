#if WITH_DEV_AUTOMATION_TESTS && PLATFORM_WINDOWS

#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
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

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FAquilaObserverTest, "Aquila.Input.Observer",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::NonNullRHI | EAutomationTestFlags::EngineFilter)

void FAquilaObserverTest::GetTests(TArray<FString>& Names, TArray<FString>& Commands) const
{
    FString Demo;
    FParse::Value(FCommandLine::Get(), TEXT("AquilaDemo="), Demo);
    if (Demo == TEXT("bridge")) return;
    const FString Name = FParse::Param(FCommandLine::Get(), TEXT("AquilaTouchUI")) ? TEXT("Touch") : TEXT("Desktop");
    Names.Add(Name);
    Commands.Add(Name);
}

bool FAquilaObserverTest::RunTest(const FString& Parameters)
{
    UGameViewportClient* Viewport = GEngine->GameViewport;
    auto* World = Viewport ? Viewport->GetWorld() : nullptr;
    auto* PC = World ? World->GetFirstPlayerController() : nullptr;
    auto* Pawn = PC ? Cast<AAquilaFlightPawn>(PC->GetPawn()) : nullptr;
    auto* G = World ? World->GetAuthGameMode<AAquilaGameMode>() : nullptr;
    auto* Camera = Pawn ? Pawn->FindComponentByClass<UCameraComponent>() : nullptr;
    if (!TestTrue(TEXT("Rendered observer and campaign"), Camera && G && Viewport->GetWindow())) return false;
    USceneComponent* Ship = nullptr;
    USceneComponent* Enemy = nullptr;
    TInlineComponentArray<USceneComponent*> Parts(Pawn);
    for (auto* Part : Parts)
    {
        if (Part->GetFName() == TEXT("Resolute")) Ship = Part;
        if (Part->GetFName() == TEXT("Sentinel")) Enemy = Part;
    }
    if (!TestTrue(TEXT("Both ship roots exist"), Ship && Enemy)) return false;

    auto& C = G->Campaign;
    TGuardValue<FAquilaCampaign> RestoreCampaign(C, C);
    TGuardValue<bool> RestorePause(G->bPaused, true);
    const int32 Revision = C.Sequence;
    const auto Order = C.Order;
    const bool bTouch = Parameters == TEXT("Touch");
    auto Frame = [&]() { Pawn->Tick(1.f / 60); PC->PlayerCameraManager->UpdateCamera(1.f / 60); };
    auto EyeMeters = [&]() { return C.ShipPosition + (Camera->GetComponentLocation() - Ship->GetComponentLocation()) * 10.; };
    auto FocusMeters = [&]() { return C.ShipPosition - Ship->GetRelativeLocation() * 10.; };
    auto Key = [&](FKey K, bool bDown)
    {
        PC->InputKey(FInputKeyEventArgs::CreateSimulated(K, bDown ? IE_Pressed : IE_Released, bDown ? 1.f : 0.f));
        PC->PlayerTick(1.f / 60);
        Frame();
    };
    int32 W, H;
    PC->GetViewportSize(W, H);
    const FGeometry View = Viewport->GetGameViewportWidget()->GetCachedGeometry();
    auto& Slate = FSlateApplication::Get();
    const int32 User = Slate.FindOrCreateVirtualUser(0)->GetUserIndex();
    const uint32 Pointer = FSlateApplication::CursorPointerIndex;
    const TSet<FKey> Left{EKeys::LeftMouseButton}, Middle{EKeys::MiddleMouseButton}, Empty;
    auto Screen = [&](FVector2D Pixel) { return View.LocalToAbsolute(Pixel / FVector2D(W,H) * View.GetLocalSize()); };
    auto Path = [&](FVector2D P) { return FWidgetPath(Viewport->GetWindow()->GetHittestGrid().GetBubblePath(P,0,false,User)); };
    auto Touch = [&](ETouchType::Type Type, FVector2D Pixel)
    {
        PC->InputTouch(FTouchId(FInputDeviceId::CreateFromInternalId(0), ETouchIndex::Touch1), Type, Pixel,
            Type == ETouchType::Ended ? 0.f : 1.f, FPlatformTime::Cycles64());
        PC->PlayerTick(1.f / 60);
        Frame();
    };
    auto Click = [&](FVector2D Pixel)
    {
        if (bTouch) { Touch(ETouchType::Began, Pixel); Touch(ETouchType::Ended, Pixel); }
        else
        {
            const FVector2D P = Screen(Pixel);
            const auto Under = Path(P);
            Slate.RoutePointerDownEvent(Under, FPointerEvent(User,Pointer,P,P,Left,EKeys::LeftMouseButton,0,FModifierKeysState()));
            Slate.RoutePointerUpEvent(Under, FPointerEvent(User,Pointer,P,P,Empty,EKeys::LeftMouseButton,0,FModifierKeysState()));
            Frame();
        }
    };
    auto ClickShip = [&](USceneComponent* Target)
    {
        FVector2D Pixel;
        if (!PC->ProjectWorldLocationToScreen(Target->GetComponentLocation(), Pixel)
            || Pixel.X <= 0 || Pixel.X >= W || Pixel.Y <= 0 || Pixel.Y >= H) return false;
        Click(Pixel);
        return true;
    };

    Frame();
    const FVector Initial = EyeMeters();
    const FRotator Facing = Camera->GetComponentRotation();
    if (C.Order == EAquilaOrder::Hold)
    {
        TestTrue(TEXT("Opening a stationary save frames its actual flagship position"), FocusMeters().Equals(C.ShipPosition,0.05));
        TestTrue(TEXT("Opening angle uses the saved ship heading once"),
            FMath::IsNearlyZero(FMath::FindDeltaAngleDegrees(Facing.Yaw,32. + FMath::RadiansToDegrees(C.Heading)),0.05));
    }
    C.ShipPosition += FVector(5000, -2000, 0);
    C.Heading += 0.4;
    Frame();
    TestTrue(TEXT("Ship can fly and turn without carrying the default camera"), EyeMeters().Equals(Initial, 0.05) && Camera->GetComponentRotation().Equals(Facing,0.01));

    const FVector2D Corner(W * 0.08, H * 0.08);
    const FVector BeforePan = FocusMeters();
    if (bTouch)
    {
        Touch(ETouchType::Began, Corner);
        Touch(ETouchType::Moved, Corner + FVector2D(45,20));
        Touch(ETouchType::Ended, Corner + FVector2D(45,20));
    }
    else
    {
        Key(EKeys::W,true); Key(EKeys::W,false);
        const auto P = Screen(Corner);
        const auto Under = Path(P);
        const double BeforeZoom = Camera->GetRelativeLocation().Size();
        Slate.RouteMouseWheelOrGestureEvent(Under,FPointerEvent(User,Pointer,P,P,Empty,EKeys::MouseWheelAxis,1.f,FModifierKeysState()));
        Frame();
        TestTrue(TEXT("Wheel zoom changes the actual camera distance"), Camera->GetRelativeLocation().Size() < BeforeZoom);
    }
    TestTrue(TEXT("Free panning moves the focus while the ship simulation is paused"), !FocusMeters().Equals(BeforePan,1.));
    if (!TestTrue(TEXT("Flagship can be picked on screen"), ClickShip(Ship))) return false;
    if (!bTouch)
    {
        const FRotator Chase = Camera->GetRelativeRotation();
        const auto P = Screen(Corner);
        const auto Under = Path(P);
        const TSet<FKey> Right{EKeys::RightMouseButton};
        Slate.RoutePointerDownEvent(Under,FPointerEvent(User,Pointer,P,P,Right,EKeys::RightMouseButton,0,FModifierKeysState()));
        Slate.RoutePointerMoveEvent(Under,FPointerEvent(User,Pointer,P,P,FVector2D(160,0),Right,FModifierKeysState()),false);
        Slate.RoutePointerUpEvent(Under,FPointerEvent(User,Pointer,P,P,Empty,EKeys::RightMouseButton,0,FModifierKeysState()));
        Frame();
        TestFalse(TEXT("The selected ship can still be orbited"), Camera->GetRelativeRotation().Equals(Chase,1.));
        G->bPaused = false;
        for (int32 I=0; I<220; ++I) Frame();
        G->bPaused = true;
        TestTrue(TEXT("Follow eases behind the selected ship after release"), Camera->GetRelativeRotation().Equals(Chase,0.15));
    }
    FVector Eye = EyeMeters();
    const FVector Travel(2100, -450, 0);
    C.ShipPosition += Travel;
    Frame();
    TestTrue(TEXT("Clicking Resolute follows its motion"), (EyeMeters()-Eye).Equals(Travel,0.05));

    Click(Corner);
    Eye = EyeMeters();
    C.ShipPosition += Travel;
    Frame();
    TestTrue(TEXT("Clicking empty space releases the follow"), EyeMeters().Equals(Eye,0.05));
    C.bBattle = true;
    C.EnemyHull = 100;
    C.EnemyPosition = C.ShipPosition + Camera->GetRightVector() * 8400;
    Frame();
    if (!TestTrue(TEXT("Second ship can be picked on screen"), ClickShip(Enemy))) return false;
    Eye = EyeMeters();
    C.EnemyPosition += Travel;
    Frame();
    TestTrue(TEXT("Clicking Sentinel follows Sentinel"), (EyeMeters()-Eye).Equals(Travel,0.05));
    Eye = EyeMeters();
    C.EnemyHull = 0;
    C.EnemyPosition += Travel;
    C.ShipPosition -= Travel;
    Frame();
    TestTrue(TEXT("Lost contact leaves the camera at its last world position"), EyeMeters().Equals(Eye,0.05));

    Key(EKeys::Home,true); Key(EKeys::Home,false);
    Eye = EyeMeters();
    C.ShipPosition += Travel;
    Frame();
    TestTrue(TEXT("Home finds the flagship from a detached view"), (EyeMeters()-Eye).Equals(Travel,0.05));
    if (!bTouch)
    {
        const auto P = Screen(Corner);
        const auto Under = Path(P);
        Slate.RoutePointerDownEvent(Under,FPointerEvent(User,Pointer,P,P,Middle,EKeys::MiddleMouseButton,0,FModifierKeysState()));
        Slate.RoutePointerMoveEvent(Under,FPointerEvent(User,Pointer,P,P,FVector2D(48,16),Middle,FModifierKeysState()),false);
        Slate.RoutePointerUpEvent(Under,FPointerEvent(User,Pointer,P,P,Empty,EKeys::MiddleMouseButton,0,FModifierKeysState()));
    }
    else Click(Corner);
    Frame();
    Eye = EyeMeters();
    C.ShipPosition += Travel;
    Frame();
    TestTrue(TEXT("Released camera stays free"), EyeMeters().Equals(Eye,0.05));
    const FRotator ExteriorFacing = Camera->GetRelativeRotation();
    Key(EKeys::C,true); Key(EKeys::C,false);
    TestFalse(TEXT("Bridge is aboard Resolute"), Ship->IsVisible());
    const FVector BridgeEye = Camera->GetRelativeLocation();
    G->bPaused = false;
    Key(EKeys::D,true); Key(EKeys::D,false);
    G->bPaused = true;
    TestFalse(TEXT("First-person walking moves the bridge eye"), Camera->GetRelativeLocation().Equals(BridgeEye,1.));
    Key(EKeys::C,true); Key(EKeys::C,false);
    TestTrue(TEXT("Leaving the bridge restores the exterior observer"), Ship->IsVisible() && EyeMeters().Equals(Eye,0.05) && Camera->GetRelativeRotation().Equals(ExteriorFacing,0.01));
    TestTrue(TEXT("Observing leaves helm, save and pause untouched"), C.Sequence == Revision && C.Order == Order && G->bPaused);
    return true;
}
#endif
