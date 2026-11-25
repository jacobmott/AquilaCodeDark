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
    const bool bArena = G->Session() == EAquilaSession::Simulator;
    if (bArena)
    {
        // The test may attach seconds after launch. Sample the opening camera before either ship advances.
        G->StartDuel(C.Loadout);
        Pawn->ResetSessionView();
        G->bPaused = true;
    }
    const int32 Revision = C.Sequence;
    const auto Order = C.Order;
    const bool bTouch = Parameters == TEXT("Touch");
    auto Frame = [&]() { Pawn->Tick(1.f / 60); PC->PlayerCameraManager->UpdateCamera(1.f / 60); };
    auto EyeMeters = [&]() { return C.ShipPosition + (Camera->GetComponentLocation() - Ship->GetComponentLocation()) * 10.; };
    auto Key = [&](FKey K, bool bDown)
    {
        PC->InputKey(FInputKeyEventArgs::CreateSimulated(K, bDown ? IE_Pressed : IE_Released, bDown ? 1.f : 0.f));
        PC->PlayerTick(1.f / 60);
        Frame();
    };
    auto TravelKey = [&](FKey K)
    {
        const FVector Start = EyeMeters();
        Key(K, true);
        Key(K, false);
        return EyeMeters() - Start;
    };
    int32 W, H;
    PC->GetViewportSize(W, H);
    const FGeometry View = Viewport->GetGameViewportWidget()->GetCachedGeometry();
    auto& Slate = FSlateApplication::Get();
    const int32 User = Slate.FindOrCreateVirtualUser(0)->GetUserIndex();
    const uint32 Pointer = FSlateApplication::CursorPointerIndex;
    const TSet<FKey> Left{EKeys::LeftMouseButton}, Empty;
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
    const FVector2D Corner(W * 0.08, H * 0.08);
    auto Drag = [&](FVector2D Delta, FKey Button)
    {
        const FVector2D P = Screen(Corner);
        const auto Under = Path(P);
        const TSet<FKey> Held{Button};
        Slate.RoutePointerDownEvent(Under,FPointerEvent(User,Pointer,P,P,Held,Button,0,FModifierKeysState()));
        Slate.RoutePointerMoveEvent(Under,FPointerEvent(User,Pointer,P,P,Delta,Held,FModifierKeysState()),false);
        Slate.RoutePointerUpEvent(Under,FPointerEvent(User,Pointer,P,P,Empty,Button,0,FModifierKeysState()));
        Frame();
    };

    Frame();
    const FVector Initial = EyeMeters();
    const FRotator Facing = Camera->GetComponentRotation();
    if (bArena || C.Order == EAquilaOrder::Hold)
    {
        const FVector Framed = bArena ? (C.ShipPosition + C.EnemyPosition) * 0.5 : C.ShipPosition;
        TestTrue(TEXT("Opening frames the arena midpoint or restored flagship"),
            (Initial + Facing.Vector() * 48000.).Equals(Framed,0.05));
        TestTrue(TEXT("Each mode keeps its opening angle"), FMath::IsNearlyZero(
            FMath::FindDeltaAngleDegrees(Facing.Yaw,bArena ? 100. : 32. + FMath::RadiansToDegrees(C.Heading)),0.05));
    }
    C.ShipPosition += FVector(5000, -2000, 0);
    C.Heading += 0.4;
    Frame();
    TestTrue(TEXT("Ship can fly and turn without carrying the default camera"), EyeMeters().Equals(Initial, 0.05) && Camera->GetComponentRotation().Equals(Facing,0.01));

    const FVector BeforePan = EyeMeters();
    if (bTouch)
    {
        Touch(ETouchType::Began, Corner);
        Touch(ETouchType::Moved, Corner + FVector2D(45,20));
        Touch(ETouchType::Ended, Corner + FVector2D(45,20));
    }
    else
    {
        Drag(FVector2D(240,-40), EKeys::RightMouseButton);
        const FRotator Turned = Camera->GetComponentRotation();
        const double FreeTravel = FVector::Dist(EyeMeters(), BeforePan);
        TestTrue(TEXT("Free yaw and pitch keep the eye fixed in simulation space"),
            FMath::IsNearlyEqual(FMath::FindDeltaAngleDegrees(Facing.Yaw, Turned.Yaw),90.,0.05) &&
            FMath::IsNearlyEqual(Turned.Pitch - Facing.Pitch,12.,0.05) && FreeTravel < 0.001);
        AddInfo(FString::Printf(TEXT("Free look: %.2f degrees yaw, %.2f degrees pitch, %.6f meters traveled."),
            FMath::FindDeltaAngleDegrees(Facing.Yaw,Turned.Yaw), Turned.Pitch-Facing.Pitch, FreeTravel));
        Drag(FVector2D(-240,40), EKeys::LeftMouseButton);
        TestTrue(TEXT("Left drag returns to the original view from the same eye"),
            Facing.Equals(Camera->GetComponentRotation(),0.05) && EyeMeters().Equals(BeforePan,0.001));
        const FVector Down = TravelKey(EKeys::W);
        TestTrue(TEXT("W dives along the downward sight line"), Down.Z < 0 && Down.GetSafeNormal().Equals(Camera->GetForwardVector(),1.e-6));
        TestTrue(TEXT("S reverses the dive"), TravelKey(EKeys::S).Equals(-Down,0.01));
        Drag(FVector2D(160,-200), EKeys::RightMouseButton);
        const FVector Up = TravelKey(EKeys::W);
        TestTrue(TEXT("W climbs along the turned and pitched sight line"), Up.Z > 0 && Up.GetSafeNormal().Equals(Camera->GetForwardVector(),1.e-6));
        TestTrue(TEXT("S reverses the climb"), TravelKey(EKeys::S).Equals(-Up,0.01));
        const FVector Strafe = TravelKey(EKeys::D);
        const FVector StrafeBack = TravelKey(EKeys::A);
        TestTrue(TEXT("D and A strafe across the view"), Strafe.GetSafeNormal().Equals(Camera->GetRightVector(),1.e-6) &&
            StrafeBack.Equals(-Strafe,0.01));
        const FVector Rise = TravelKey(EKeys::R);
        const FVector Fall = TravelKey(EKeys::F);
        TestTrue(TEXT("R and F use world vertical at a pitch"), Rise.GetSafeNormal().Equals(FVector::UpVector,1.e-6) &&
            Fall.Equals(-Rise,0.01));
        Key(EKeys::LeftShift,true);
        TestTrue(TEXT("Boost follows the same sight line at four times speed"), TravelKey(EKeys::W).Equals(Up * 4.,0.05));
        TravelKey(EKeys::S);
        Key(EKeys::LeftShift,false);
        AddInfo(FString::Printf(TEXT("Forward flight: %.2f m vertical looking down, %.2f m vertical looking up, %.2f m traveled per step."),
            Down.Z, Up.Z, Up.Size()));
        Drag(FVector2D(-160,200), EKeys::LeftMouseButton);
        TravelKey(EKeys::W);
        const auto P = Screen(Corner);
        const auto Under = Path(P);
        const FVector BeforeZoom = EyeMeters();
        Slate.RouteMouseWheelOrGestureEvent(Under,FPointerEvent(User,Pointer,P,P,Empty,EKeys::MouseWheelAxis,1.f,FModifierKeysState()));
        Frame();
        TestTrue(TEXT("Wheel moves the free eye along its sight line"),
            (EyeMeters() - BeforeZoom).Equals(Camera->GetForwardVector() * 9600.,0.05));
    }
    TestTrue(TEXT("Free movement translates the eye while paused"), !EyeMeters().Equals(BeforePan,1.));
    if (!TestTrue(TEXT("Flagship can be picked on screen"), ClickShip(Ship))) return false;
    if (!bTouch)
    {
        const FRotator Chase = Camera->GetRelativeRotation();
        const FVector BeforeOrbit = EyeMeters();
        Drag(FVector2D(160,0), EKeys::RightMouseButton);
        TestTrue(TEXT("Follow orbits the selected ship at the same radius"),
            !EyeMeters().Equals(BeforeOrbit,1.) && FMath::IsNearlyEqual(
                FVector::Dist(BeforeOrbit,C.ShipPosition), FVector::Dist(EyeMeters(),C.ShipPosition),0.05));
        const FVector BeforeRelease = EyeMeters();
        const FRotator ReleaseFacing = Camera->GetComponentRotation();
        Click(Corner);
        TestTrue(TEXT("Releasing a rotated follow preserves position and facing"),
            EyeMeters().Equals(BeforeRelease,0.05) && Camera->GetComponentRotation().Equals(ReleaseFacing,0.01));
        Drag(FVector2D(-160,0), EKeys::RightMouseButton);
        TestTrue(TEXT("Turning immediately after release keeps the new free eye fixed"), EyeMeters().Equals(BeforeRelease,0.05));
        Drag(FVector2D(160,0), EKeys::RightMouseButton);
        if (!TestTrue(TEXT("The flagship can be followed again"), ClickShip(Ship))) return false;
        Drag(FVector2D(160,0), EKeys::RightMouseButton);
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
        const FVector FollowSight = Camera->GetForwardVector();
        TestTrue(TEXT("W releases follow along the selected ship view"), TravelKey(EKeys::W).GetSafeNormal().Equals(FollowSight,1.e-6));
        Eye = EyeMeters();
        C.ShipPosition += Travel;
        Frame();
        TestTrue(TEXT("Flying off a moving ship detaches the eye"), EyeMeters().Equals(Eye,0.05));
        Key(EKeys::Home,true); Key(EKeys::Home,false);
        const FVector BeforeMiddleDrag = EyeMeters();
        Drag(FVector2D(48,16), EKeys::MiddleMouseButton);
        const FVector Pan = EyeMeters() - BeforeMiddleDrag;
        TestTrue(TEXT("Middle drag pans across the horizontal plane"), Pan.Size() > 1 && FMath::Abs(Pan.Z) < 0.01);
    }
    else Click(Corner);
    Frame();
    Eye = EyeMeters();
    C.ShipPosition += Travel;
    Frame();
    TestTrue(TEXT("Released camera stays free"), EyeMeters().Equals(Eye,0.05));
    if (!bArena)
    {
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
    }
    TestTrue(TEXT("Observing leaves helm, save and pause untouched"), C.Sequence == Revision && C.Order == Order && G->bPaused);
    return true;
}
#endif
