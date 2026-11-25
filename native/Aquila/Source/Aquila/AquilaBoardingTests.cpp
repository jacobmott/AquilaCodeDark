#if WITH_DEV_AUTOMATION_TESTS && PLATFORM_WINDOWS

#include "AquilaBoarding.h"
#include "AquilaBoardingBay.h"
#include "AquilaBridgeLayout.h"
#include "AquilaGameMode.h"
#include "Camera/CameraTypes.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Input/HittestGrid.h"
#include "InputKeyEventArgs.h"
#include "ImageUtils.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UnrealClient.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
bool DeltaDockRun()
{
    FString Ship;
    return FParse::Value(FCommandLine::Get(),TEXT("AquilaBoardingShip="),Ship) && Ship.Equals(TEXT("DeltaTree"),ESearchCase::IgnoreCase);
}

bool CheckDeltaDock(FAutomationTestBase& Test,UWorld* World,AAquilaBoardingWalker* Walker)
{
    TActorIterator<AAquilaBoardingBay> Bays(World);
    auto* Hull=Bays?FindObjectFast<UStaticMeshComponent>(*Bays,TEXT("DeltaTreeStudy")):nullptr;
    if (!Test.TestNotNull(TEXT("Delta Tree has a cooked Blender mesh"),Hull) || !Hull->GetStaticMesh()) return false;
    const FBox HullBounds=Hull->GetStaticMesh()->GetBoundingBox();
    const double Length=HullBounds.GetSize().X/100.;
    const FTransform Dock=Hull->GetSocketTransform(TEXT("BoardingDock"));
    Test.TestTrue(TEXT("Full-scale hull docks on its original island, facing out along its branch"),
        FMath::IsNearlyEqual(Length,1280.,.05) && Hull->DoesSocketExist(TEXT("BoardingDock"))
        && Dock.GetLocation().Equals({0,-1700,0},1.) && Dock.GetUnitAxis(EAxis::Y).Equals(FVector::RightVector,.001));
    auto LocalHullRay=[Hull](FVector From,FVector To,FHitResult& Hit)
    {
        const FTransform T=Hull->GetComponentTransform();
        return Hull->LineTraceComponent(Hit,T.TransformPosition(From*100.),T.TransformPosition(To*100.),FCollisionQueryParams(NAME_None,true));
    };
    auto Thickness=[&](double X,double Y)
    {
        FHitResult Top,Bottom;
        if (!LocalHullRay({X,Y,420},{X,Y,-30},Top) || !LocalHullRay({X,Y,-30},{X,Y,420},Bottom)) return 0.;
        return (Top.ImpactPoint.Z-Bottom.ImpactPoint.Z)/100.;
    };
    const double Bow=Thickness(475,0), Stern=Thickness(-565,3.8);
    Test.TestTrue(TEXT("Side profile rises from a shallow forebody into the stacked stern"),Bow>55 && Stern>2.*Bow);
    int32 Tiers=0;
    for (double Z : {52.7,141.4,233.9})
    {
        FHitResult Cap;
        if (LocalHullRay({-680,3.8,Z},{-603,3.8,Z},Cap)) ++Tiers;
    }
    Test.TestEqual(TEXT("Three stern drive tiers intersect cooked collision"),Tiers,3);

    FCollisionQueryParams Scene(NAME_None,false,Walker);
    auto* Shuttle=FindObjectFast<USceneComponent>(*Bays,TEXT("Peregrine"));
    if (!Test.TestNotNull(TEXT("Peregrine is the shared boarding shuttle"),Shuttle)) return false;
    for (USceneComponent* Part : Shuttle->GetAttachChildren())
        if (auto* Primitive=Cast<UPrimitiveComponent>(Part)) Scene.AddIgnoredComponent(Primitive);
    const auto Box=FCollisionShape::MakeBox(FVector(3200,3400,800));
    FHitResult Hit;
    Test.TestFalse(TEXT("Whole 64 by 68 by 16 m shuttle clears hull, dock and fittings along the approach"),
        World->SweepSingleByChannel(Hit,{0,24000,808},{0,6100,808},FQuat::Identity,ECC_Visibility,Box,Scene));
    Test.TestTrue(TEXT("Lowered shuttle box contacts its landing apron"),
        World->SweepSingleByChannel(Hit,{0,6100,840},{0,6100,770},FQuat::Identity,ECC_Visibility,Box,Scene));
    auto Ray=[&](FVector A,FVector B,FHitResult& R){ return World->LineTraceSingleByChannel(R,A*100.,B*100.,ECC_Visibility,Scene); };
    for (double Y : {-39.4,-45.4})
    {
        FHitResult Left,Right,Top;
        const FVector Eye(0,Y,1.6);
        Test.TestTrue(TEXT("Airlock aperture is 2.8 m wide and 3 m high"),
            Ray(Eye,{-5,Y,1.6},Left) && Ray(Eye,{5,Y,1.6},Right) && Ray(Eye,{0,Y,6},Top)
            && FMath::IsNearlyEqual(Right.ImpactPoint.X-Left.ImpactPoint.X,280.,1.) && FMath::IsNearlyEqual(Top.ImpactPoint.Z,300.,1.));
    }
    const auto Crew=FCollisionShape::MakeCapsule(34.f,90.f);
    Test.TestFalse(TEXT("Standing capsule clears round dock, airlock and passage"),
        World->SweepSingleByChannel(Hit,{0,1300,102},{0,-7300,102},FQuat::Identity,ECC_Visibility,Crew,Scene));
    Test.TestFalse(TEXT("Both arms of the first junction clear a standing person"),
        World->SweepSingleByChannel(Hit,{-1850,-7220,102},{1850,-7220,102},FQuat::Identity,ECC_Visibility,Crew,Scene));
    Test.AddInfo(FString::Printf(TEXT("Delta hull %.2f m, sampled forebody %.2f m thick, stern %.2f m thick, drive tiers %d."),Length,Bow,Stern,Tiers));
    return true;
}

TSharedPtr<SWidget> BoardingButtonText(const TSharedRef<SWidget>& Widget)
{
    if (!Widget->GetVisibility().IsVisible()) return nullptr;
    if (Widget->GetType() == TEXT("STextBlock") &&
        StaticCastSharedRef<STextBlock>(Widget)->GetText().ToString() == TEXT("Boarding Test")) return Widget;
    FChildren* Children = Widget->GetChildren();
    if (Children)
        for (int I=0; I<Children->Num(); ++I)
            if (auto Found = BoardingButtonText(Children->GetChildAt(I))) return Found;
    return nullptr;
}

class FBoardingWalk : public IAutomationLatentCommand
{
    enum EStage { Ramp, Canopy, Exterior, Berth, Deck, Lock, Lobby, Wall, OuterDeck, ShrunkDeck, Menu, Reentered };
    EStage Stage = Ramp;
    FAutomationTestBase& Test;
    FString Frames, Pending;
    const double Start = FPlatformTime::Seconds();
    double Resume = 0, SideTime = 0;
    FVector StartLocation;
    FRotator RampHeading;
    int32 Length = 1280;
    bool bOnDeck = false;
    bool bLookingUp = false;
    const bool bDeltaTest=DeltaDockRun();
    double DeltaOrbitMetres=0;
    double RotundaY=0, RotundaTime=0;
    enum class EDeltaStage { Start, Exterior, Compare, ReturnToDelta, Berth, OnFoot, Rotunda, Airlock, Junction, Command, Research, Reset };
    EDeltaStage DeltaStage=EDeltaStage::Start;
    const bool bBridgeRun=FParse::Param(FCommandLine::Get(),TEXT("AquilaBridgeTest"));
    enum class EBridgeStage { Enter, LiftDoor, Ride, Passage, Room, Station, Bow, BeforeShot, FirstHit, Exterior, Inspect, SecondHit, Return, Recharge, Descend, Home };
    EBridgeStage BridgeStage=EBridgeStage::Enter;
    FVector StationFooting;
    bool bMidRide=false;
    const bool bManualBridgeTest=FParse::Param(FCommandLine::Get(),TEXT("AquilaManualBridge"));
    enum class EWatchStage { Arrive, Flight, Hit, Outside, OutsideHit, Return, Deplete, Recharge, ResumeFire };
    EWatchStage WatchStage=EWatchStage::Arrive;
    bool bWatching=false;
    uint64 ArrivalShots=0, SeenHits=0;
    FRotator WindowHeading;

    void Capture(const TCHAR* Name)
    {
        Pending = Frames / (bDeltaTest ? FString(TEXT("delta-"))+Name : FString::Printf(TEXT("%d-%s"),Length,Name));
        FScreenshotRequest::RequestScreenshot(Pending,true,false);
    }
    void Key(APlayerController* PC, FKey Which, bool bDown)
    {
        PC->InputKey(FInputKeyEventArgs::CreateSimulated(Which,bDown ? IE_Pressed : IE_Released,bDown ? 1.f : 0.f));
        PC->PlayerTick(1.f/60);
    }
    void Tap(APlayerController* PC, FKey Which) { Key(PC,Which,true); Key(PC,Which,false); }

    void CheckMouseSensitivity(AAquilaBoardingWalker* Walk,APlayerController* PC)
    {
        const bool bOrbit=Walk->IsOutside();
        const FRotator Footing=PC->GetControlRotation();
        auto ViewHeading=[&]() {
            if (!bOrbit) return PC->GetControlRotation();
            FMinimalViewInfo View;
            Walk->CalcCamera(0,View);
            return View.Rotation;
        };
        const FRotator Before=ViewHeading();
        PC->InputKey(FInputKeyEventArgs::CreateSimulated(EKeys::MouseX,IE_Axis,24.f));
        PC->InputKey(FInputKeyEventArgs::CreateSimulated(EKeys::MouseY,IE_Axis,12.f));
        PC->PlayerTick(1.f/60);
        float DX=0,DY=0;
        PC->GetInputMouseDelta(DX,DY);
        Walk->Tick(1.f/60);
        const FRotator Turn=(ViewHeading()-Before).GetNormalized();
        const double Gain=bOrbit ? -1.05 : .84;
        Test.TestTrue(bOrbit ? TEXT("Exterior mouse yaw and pitch have triple their previous gain") :
            TEXT("On-foot mouse yaw and pitch have triple their previous gain"),
            FMath::Abs(DX)>.1 && FMath::Abs(DY)>.1 && FMath::IsNearlyEqual(Turn.Yaw,DX*Gain,.005) &&
            FMath::IsNearlyEqual(Turn.Pitch,DY*Gain,.005));
        Test.AddInfo(FString::Printf(TEXT("Boarding %s mouse: delta %.3f, %.3f; turn %.3f, %.3f degrees."),
            bOrbit?TEXT("exterior"):TEXT("on-foot"),DX,DY,Turn.Yaw,Turn.Pitch));
        PC->PlayerTick(1.f/60);
        PC->SetControlRotation(Footing);
    }

    bool CheckObservation(APlayerController* PC,AAquilaBoardingWalker* Walk,AAquilaBoardingBay* Bay)
    {
        using namespace AquilaBridge;
        const auto* Trails=FindObjectFast<UInstancedStaticMeshComponent>(Bay,TEXT("BridgeRoundTrails"));
        const auto* Contacts=FindObjectFast<UInstancedStaticMeshComponent>(Bay,TEXT("BridgeShieldContacts"));
        if (!Test.TestTrue(TEXT("World range owns tracer and shield contact meshes"),Trails && Contacts)) return true;
        switch (WatchStage)
        {
        case EWatchStage::Arrive:
        {
            if (!Walk->GetCharacterMovement()->IsMovingOnGround() || Walk->GetVelocity().Size()>1.) break;
            Test.TestTrue(TEXT("Visitor can watch away from the station with a clear HUD"),Walk->IsWatchingBridge() &&
                !Walk->AtTactical() && !Walk->TracksDrone() && Walk->BoardingHint().IsEmpty());
            Test.TestFalse(TEXT("Normal boarding has no target or trigger binding"),
                Walk->InputComponent->KeyBindings.ContainsByPredicate([](const FInputKeyBinding& B)
                { return B.Chord.Key==EKeys::T || B.Chord.Key==EKeys::SpaceBar; }));
            StationFooting=Walk->GetActorLocation();
            WindowHeading=PC->GetControlRotation();
            FMinimalViewInfo View;
            Walk->CalcCamera(0,View);
            auto* Hull=FindObjectFast<UStaticMeshComponent>(Bay,TEXT("DeltaTreeStudy"));
            FHitResult Hit;
            Test.TestTrue(TEXT("Observer window has a clear sightline through the cooked hull"),
                Hull && !Hull->LineTraceComponent(Hit,View.Location,Drone*100.,FCollisionQueryParams(NAME_None,true)));
            int32 Width,Height;
            PC->GetViewportSize(Width,Height);
            for (const FVector& Landmark : {Turret,Drone})
            {
                FVector2D Screen;
                Test.TestTrue(TEXT("Cannon and target appear within the visitor's view"),
                    PC->ProjectWorldLocationToScreen(Landmark*100.,Screen) && Screen.X>0 && Screen.X<Width &&
                    Screen.Y>0 && Screen.Y<Height);
            }
            ArrivalShots=Bay->TrainingShots();
            SeenHits=Bay->TrainingHits();
            WatchStage=EWatchStage::Flight;
            break;
        }
        case EWatchStage::Flight:
            if (Bay->TrainingShots()<=ArrivalShots || !Trails->GetInstanceCount()) break;
            Capture(TEXT("window-firing.png"));
            SeenHits=Bay->TrainingHits();
            WatchStage=EWatchStage::Hit;
            break;
        case EWatchStage::Hit:
            if (Bay->TrainingHits()<=SeenHits || !Contacts->GetInstanceCount()) break;
            Capture(TEXT("window-hit.png"));
            WatchStage=EWatchStage::Outside;
            break;
        case EWatchStage::Outside:
        {
            const int32 Shield=Bay->DroneShield();
            SeenHits=Bay->TrainingHits();
            Tap(PC,EKeys::Tab);
            Test.TestTrue(TEXT("Exterior observes the same shield and impact without a target lock"),Walk->IsDroneView() &&
                Walk->IsWatchingBridge() && !Walk->TracksDrone() && Walk->ShieldReading()==Shield && Bay->TrainingHits()==SeenHits);
            WatchStage=EWatchStage::OutsideHit;
            break;
        }
        case EWatchStage::OutsideHit:
            if (Bay->TrainingHits()<=SeenHits || !Contacts->GetInstanceCount()) break;
            Capture(TEXT("outside-hit.png"));
            WatchStage=EWatchStage::Return;
            break;
        case EWatchStage::Return:
            Tap(PC,EKeys::Tab);
            Test.TestTrue(TEXT("Returning restores the visitor's footing and free look"),!Walk->IsOutside() &&
                Walk->GetActorLocation().Equals(StationFooting,2.) && PC->GetControlRotation().Equals(WindowHeading,.02));
            WatchStage=EWatchStage::Deplete;
            break;
        case EWatchStage::Deplete:
            if (Bay->DroneShield()!=0) break;
            SeenHits=Bay->TrainingHits();
            WatchStage=EWatchStage::Recharge;
            break;
        case EWatchStage::Recharge:
            if (Bay->DroneShield()!=100) break;
            Test.TestTrue(TEXT("The same drone recharges in place"),Bay->TrainingHits()>=SeenHits);
            WatchStage=EWatchStage::ResumeFire;
            break;
        case EWatchStage::ResumeFire:
            if (Bay->TrainingHits()<=SeenHits || Bay->DroneShield()>=100) break;
            Test.TestTrue(TEXT("Repeated automatic fire leaves the visitor's view alone"),!Bay->DroneLocked() &&
                PC->GetControlRotation().Equals(WindowHeading,.02) && Walk->GetActorLocation().Equals(StationFooting,2.));
            Test.AddInfo(FString::Printf(TEXT("Delta bridge watch: %llu shots on arrival, %llu shots and %llu contacts after recharge, shield %d. No targeting or firing input."),
                ArrivalShots,Bay->TrainingShots(),Bay->TrainingHits(),Bay->DroneShield()));
            return true;
        }
        return false;
    }

    bool CheckBridge(APlayerController* PC,AAquilaBoardingWalker* Walk,double Now)
    {
        using namespace AquilaBridge;
        TActorIterator<AAquilaBoardingBay> Bays(Walk->GetWorld());
        if (!Bays) { Test.AddError(TEXT("Bridge has no ship")); return true; }
        auto* Bay=*Bays;
        if (bWatching) return CheckObservation(PC,Walk,Bay);
        const FVector P=Walk->GetActorLocation()/100.;
        switch (BridgeStage)
        {
        case EBridgeStage::Enter:
            if (P.X<Low.X-.5) break;
            Key(PC,EKeys::D,false);
            PC->SetControlRotation(FRotator(0,0,0));
            if (bManualBridgeTest)
            {
                Tap(PC,EKeys::T);
                Tap(PC,EKeys::SpaceBar);
                Test.TestTrue(TEXT("Station keys cannot operate the turret from the lower lift"),!Bay->DroneLocked() && Bay->TrainingShots()==0);
            }
            else Test.TestTrue(TEXT("Ship has already fired and hit the drone before the visitor boards the lift"),
                Bay->TrainingShots()>1 && Bay->TrainingHits()>0 && !Bay->DroneLocked());
            Capture(TEXT("command-lift.png"));
            BridgeStage=EBridgeStage::LiftDoor;
            break;
        case EBridgeStage::LiftDoor:
            Tap(PC,EKeys::F);
            Tap(PC,EKeys::Tab);
            if (!Test.TestTrue(TEXT("Lift departs with the rider aboard"),Walk->IsRiding() && !Walk->IsOutside())) return true;
            BridgeStage=EBridgeStage::Ride;
            break;
        case EBridgeStage::Ride:
            if (Walk->IsRiding()) { bMidRide|=P.Z>15. && P.Z<42.; break; }
            if (!Test.TestTrue(TEXT("Continuous lift carries capsule to Command deck 48 m higher"),
                bMidRide && FVector::Dist2D(P,High)<2.4 && FMath::Abs(P.Z-High.Z-.92)<.12)) return true;
            Key(PC,EKeys::W,true);
            BridgeStage=EBridgeStage::Passage;
            break;
        case EBridgeStage::Passage:
            if (P.X>Center.X+13.5) break;
            Test.TestTrue(TEXT("Upper passage opens into a circular walkable bridge"),Walk->LocationName()==TEXT("Command bridge")
                && Walk->GetCharacterMovement()->IsMovingOnGround());
            Capture(TEXT("bridge-room.png"));
            BridgeStage=EBridgeStage::Room;
            break;
        case EBridgeStage::Room:
            if (P.X>Center.X+.35) break;
            BridgeStage=EBridgeStage::Station;
            [[fallthrough]];
        case EBridgeStage::Station:
            if (!bManualBridgeTest)
            {
                PC->SetControlRotation(FRotator(0,BowYaw,0));
                if (FVector::DotProduct(P-Center,Fore)>10.8)
                {
                    Key(PC,EKeys::W,false);
                    bWatching=true;
                }
                break;
            }
            if (!Walk->AtTactical())
            {
                PC->SetControlRotation(FRotator(0,(Station-P).Rotation().Yaw,0));
                break;
            }
            Key(PC,EKeys::W,false);
            Tap(PC,EKeys::SpaceBar);
            Test.TestTrue(TEXT("Unlocked station cannot fire"),Bay->TrainingShots()==0);
            PC->SetControlRotation(FRotator(-16.,BowYaw-2.,0));
            Capture(TEXT("bridge-bow.png"));
            BridgeStage=EBridgeStage::Bow;
            break;
        case EBridgeStage::Bow:
            Tap(PC,EKeys::T);
            Test.TestTrue(TEXT("T locks one fresh training drone"),Walk->TracksDrone() && Walk->ShieldReading()==100);
            StationFooting=Walk->GetActorLocation();
            // Check the clear window against the source hull; the invisible pressure pane still stops a walker.
            {
                FMinimalViewInfo View;
                Walk->CalcCamera(0,View);
                auto* Hull=FindObjectFast<UStaticMeshComponent>(Bay,TEXT("DeltaTreeStudy"));
                FHitResult Hit;
                FCollisionQueryParams Params(NAME_None,true);
                Params.AddIgnoredActor(Walk);
                Test.TestTrue(TEXT("Bow sightline clears the cooked hull"),Hull && !Hull->LineTraceComponent(Hit,View.Location,Drone*100.,Params));
                Test.TestTrue(TEXT("Panoramic glass still blocks the player"),Walk->GetWorld()->LineTraceSingleByChannel(Hit,View.Location,Drone*100.,ECC_Visibility,Params)
                    && Hit.GetComponent() && Hit.GetComponent()->IsA<UBoxComponent>());
                TInlineComponentArray<UBoxComponent*> Panes(Bay);
                for (const auto* Pane : Panes) Params.AddIgnoredComponent(Pane);
                // Cap samples on six separate round islands in the exported boarding frame.
                int32 VisibleDocks=0;
                for (const FVector& Dock : {FVector(-652.5,154.77,-25.16),FVector(-568.19,56.12,-18.48),
                    FVector(-479.17,-20.29,-11.79),FVector(-569.35,253.6,-25.16),
                    FVector(-454.66,195.54,-18.48),FVector(-326.63,156.87,-11.79)})
                {
                    const FVector End=Dock*100.;
                    if (Walk->GetWorld()->LineTraceSingleByChannel(Hit,View.Location,End+(End-View.Location).GetSafeNormal()*180.,ECC_Visibility,Params)
                        && Hit.GetComponent()==Hull && FVector::Dist(Hit.ImpactPoint,End)<250.) ++VisibleDocks;
                }
                Test.TestTrue(TEXT("Tactical station overlooks at least two original bow docking islands"),VisibleDocks>=2);
                Test.AddInfo(FString::Printf(TEXT("Delta tactical sightlines reach %d of six sampled original round docks."),VisibleDocks));
            }
            BridgeStage=EBridgeStage::BeforeShot;
            Resume=Now+.18;
            break;
        case EBridgeStage::BeforeShot:
            Capture(TEXT("bridge-ready.png"));
            Tap(PC,EKeys::SpaceBar);
            Tap(PC,EKeys::SpaceBar);
            Test.TestTrue(TEXT("Existing cannon fires once and enforces reload"),Bay->TrainingShots()==1 && Bay->TrainingHits()==0 && Bay->DroneShield()==100);
            BridgeStage=EBridgeStage::FirstHit;
            break;
        case EBridgeStage::FirstHit:
            if (Bay->TrainingHits()==0) break;
            if (!Test.TestEqual(TEXT("First real round removes ten shield points through the bridge window"),Bay->DroneShield(),90)) return true;
            Capture(TEXT("bridge-hit.png"));
            BridgeStage=EBridgeStage::Exterior;
            Resume=Now+.18;
            break;
        case EBridgeStage::Exterior:
            Tap(PC,EKeys::Tab);
            if (!Test.TestTrue(TEXT("Outside inspection shares the already damaged target"),Walk->IsDroneView() && Walk->TracksDrone() &&
                Walk->ShieldReading()==90 && Bay->TrainingHits()==1)) return true;
            Tap(PC,EKeys::SpaceBar);
            Test.TestTrue(TEXT("Exterior inspection cannot fire the station"),Bay->TrainingShots()==1);
            BridgeStage=EBridgeStage::Inspect;
            Resume=Now+.08;
            break;
        case EBridgeStage::Inspect:
            Capture(TEXT("drone-exterior.png"));
            BridgeStage=EBridgeStage::SecondHit;
            Resume=Now+.55;
            break;
        case EBridgeStage::SecondHit:
            Tap(PC,EKeys::Tab);
            Test.TestTrue(TEXT("Return leaves rider at the same controls"),!Walk->IsOutside() && Walk->AtTactical() &&
                Walk->GetActorLocation().Equals(StationFooting,50));
            Tap(PC,EKeys::SpaceBar);
            Tap(PC,EKeys::Tab);
            if (!Test.TestTrue(TEXT("Second round travels while viewing from outside"),Bay->TrainingShots()==2 && Bay->TrainingHits()==1)) return true;
            BridgeStage=EBridgeStage::Return;
            break;
        case EBridgeStage::Return:
            if (Bay->TrainingHits()<2) break;
            Test.TestTrue(TEXT("Same projectile reduces the same shield in the exterior camera"),Walk->ShieldReading()==80);
            Capture(TEXT("drone-hit.png"));
            Test.AddInfo(TEXT("Delta bridge: 48 m lift, two manual rounds, shield 100 to 90 to 80 across both cameras."));
            BridgeStage=EBridgeStage::Recharge;
            break;
        case EBridgeStage::Recharge:
            if (Walk->IsOutside())
            {
                Tap(PC,EKeys::Tab);
                Test.TestEqual(TEXT("Bridge retains outside result"),Walk->ShieldReading(),80);
            }
            if (Bay->DroneShield()>0)
            {
                Tap(PC,EKeys::SpaceBar);
                Resume=Now+.2;
                break;
            }
            if (!Test.TestTrue(TEXT("Ten single shots deplete the training shield"),Bay->TrainingShots()==10 && Bay->TrainingHits()==10)) return true;
            Tap(PC,EKeys::T);
            Test.TestTrue(TEXT("T can recharge the target without leaving the console"),Walk->TracksDrone() &&
                Bay->DroneShield()==100 && Bay->TrainingShots()==0);
            Key(PC,EKeys::W,true);
            BridgeStage=EBridgeStage::Descend;
            break;
        case EBridgeStage::Descend:
        {
            // Walk back through the actual opening; align on the room centerline first.
            const FVector Waypoint=FMath::Abs(P.Y-High.Y)> .35 && P.X<Center.X+1 ? Center : High;
            PC->SetControlRotation(FRotator(0,(Waypoint-P).Rotation().Yaw,0));
            if (FVector::Dist2D(P,High)>1.25) break;
            Key(PC,EKeys::W,false);
            Tap(PC,EKeys::F);
            if (!Test.TestTrue(TEXT("Upper station accepts a return ride"),Walk->IsRiding())) return true;
            BridgeStage=EBridgeStage::Home;
            break;
        }
        case EBridgeStage::Home:
            if (Walk->IsRiding()) break;
            Test.TestTrue(TEXT("Return ride reaches original lift floor"),FVector::Dist2D(P,Low)<2.2 && FMath::Abs(P.Z-.92)<.10);
            Tap(PC,EKeys::R);
            Test.TestTrue(TEXT("Restart restores a fresh target and the ramp"),Bay->DroneShield()==100 && !Bay->DroneLocked() &&
                Bay->TrainingShots()==0 && Walk->GetActorLocation().Equals(StartLocation,5.));
            Tap(PC,EKeys::Tab);
            {
                FMinimalViewInfo View;
                Walk->CalcCamera(0,View);
                Test.TestTrue(TEXT("After visiting the drone, the ramp still frames the full 1280 m ship"),
                    !Walk->IsDroneView() && FMath::IsNearlyEqual(FVector::Dist(View.Location,Bay->HullViewCenter())/100.,940.,.01));
            }
            return true;
        }
        return false;
    }

    bool UpdateDelta(UWorld* World,APlayerController* PC,AAquilaBoardingWalker* Walk,double Now)
    {
        const FVector P=Walk->GetActorLocation();
        if (P.Z < -25) { Test.AddError(TEXT("Delta walker fell off the boarding route")); return true; }
        switch (DeltaStage)
        {
        case EDeltaStage::Start:
            if (!Walk->GetCharacterMovement()->IsMovingOnGround()) return false;
            if (bBridgeRun && Walk->IsWatchingBridge())
            {
                Test.TestTrue(TEXT("Direct bridge entry starts on foot at the window"),!Walk->IsOutside() && Walk->CompletedBoardingRoute());
                DeltaStage=EDeltaStage::Command;
                bWatching=true;
                return CheckBridge(PC,Walk,Now);
            }
            if (!Test.TestTrue(TEXT("Direct entry starts on Delta Tree at the shuttle sill"),Walk->IsDeltaTree() && P.Equals({0,2730,292},4.))
                || !CheckDeltaDock(Test,World,Walk)) return true;
            StartLocation=P;
            Capture(TEXT("ramp.png"));
            DeltaStage=EDeltaStage::Exterior;
            break;
        case EDeltaStage::Exterior:
            if (!Walk->IsOutside()) { Tap(PC,EKeys::Tab); Resume=Now+.45; break; }
            Test.TestEqual(TEXT("Exterior frames Delta Tree"),Walk->LocationName(),FString(TEXT("Delta Tree exterior")));
            {
                TActorIterator<AAquilaBoardingBay> Bays(World);
                if (!Bays) { Test.AddError(TEXT("Delta exterior lost its dock")); return true; }
                FMinimalViewInfo View;
                Walk->CalcCamera(0,View);
                DeltaOrbitMetres=FVector::Distance(View.Location,Bays->HullViewCenter())/100.;
                Test.TestTrue(TEXT("Delta whole-hull orbit is 940 m from its focus"),FMath::IsNearlyEqual(DeltaOrbitMetres,940.,.01));
            }
            Capture(TEXT("exterior.png"));
            DeltaStage=EDeltaStage::Compare;
            break;
        case EDeltaStage::Compare:
            if (Walk->IsDeltaTree()) { Tap(PC,EKeys::N); Resume=Now+.45; break; }
            {
                FMinimalViewInfo View;
                Walk->CalcCamera(0,View);
                const double GlacierOrbitMetres=FVector::Distance(View.Location,{0,0,10800})/100.;
                Test.TestTrue(TEXT("N shows approved Glacier Tongue at the same orbit distance"),Walk->IsOutside() &&
                    Walk->HullLengthMetres()==1280 && FMath::IsNearlyEqual(DeltaOrbitMetres,GlacierOrbitMetres,.01));
                Test.AddInfo(FString::Printf(TEXT("Delta and Glacier exterior orbits %.2f and %.2f m."),DeltaOrbitMetres,GlacierOrbitMetres));
            }
            Capture(TEXT("glacier-comparison.png"));
            DeltaStage=EDeltaStage::ReturnToDelta;
            break;
        case EDeltaStage::ReturnToDelta:
            Tap(PC,EKeys::N);
            Tap(PC,EKeys::V);
            Resume=Now+.45;
            DeltaStage=EDeltaStage::Berth;
            break;
        case EDeltaStage::Berth:
            Test.TestEqual(TEXT("V frames the round dock and shuttle"),Walk->LocationName(),FString(TEXT("Outer dock and shuttle")));
            Capture(TEXT("berth.png"));
            DeltaStage=EDeltaStage::OnFoot;
            break;
        case EDeltaStage::OnFoot:
            Tap(PC,EKeys::Tab);
            Test.TestTrue(TEXT("Return from orbit to the same Delta ramp"),Walk->IsDeltaTree() && Walk->GetActorLocation().Equals(StartLocation,5.));
            Key(PC,EKeys::W,true);
            DeltaStage=EDeltaStage::Rotunda;
            break;
        case EDeltaStage::Rotunda:
            if (P.Y>-650) break;
            RotundaY=P.Y;
            RotundaTime=World->GetTimeSeconds();
            Test.TestTrue(TEXT("Walk enters the round terminal on level ground"),Walk->LocationName()==TEXT("Round arrivals dock")
                && Walk->GetCharacterMovement()->IsMovingOnGround() && FMath::Abs(P.Z-92)<4);
            Capture(TEXT("rotunda.png"));
            DeltaStage=EDeltaStage::Airlock;
            break;
        case EDeltaStage::Airlock:
        {
            if (P.Y>-4130) break;
            const double DistanceMetres=(RotundaY-P.Y)/100.;
            const double Elapsed=World->GetTimeSeconds()-RotundaTime;
            const double Pace=DistanceMetres/Elapsed;
            Test.TestTrue(TEXT("Walking input moves the capsule at 13.5 metres per second along the level deck"),
                FMath::IsNearlyEqual(Pace,13.5,.2));
            Test.AddInfo(FString::Printf(TEXT("Boarding walk: %.2f m in %.3f s, measured %.3f m/s."),DistanceMetres,Elapsed,Pace));
            Test.TestTrue(TEXT("Walk passes through the branch airlock"),Walk->LocationName()==TEXT("Branch airlock") && !Walk->CompletedBoardingRoute());
            Capture(TEXT("airlock.png"));
            DeltaStage=EDeltaStage::Junction;
            break;
        }
        case EDeltaStage::Junction:
            if (P.Y>-7180) break;
            Key(PC,EKeys::W,false);
            Test.TestTrue(TEXT("The uninterrupted walk reaches the first junction"),Walk->CompletedBoardingRoute() &&
                Walk->LocationName()==TEXT("First branch junction") && Walk->GetCharacterMovement()->IsMovingOnGround());
            Test.AddInfo(FString::Printf(TEXT("Delta ramp to first junction %.2f m; drop %.2f m."),(StartLocation.Y-P.Y)/100.,(StartLocation.Z-P.Z)/100.));
            Capture(TEXT("junction.png"));
            Key(PC,EKeys::D,true);
            DeltaStage=EDeltaStage::Command;
            break;
        case EDeltaStage::Command:
            if (bBridgeRun) return CheckBridge(PC,Walk,Now);
            if (P.X<1650) break;
            Key(PC,EKeys::D,false);
            Test.TestTrue(TEXT("Command arm is walkable"),Walk->GetCharacterMovement()->IsMovingOnGround());
            Key(PC,EKeys::A,true);
            DeltaStage=EDeltaStage::Research;
            break;
        case EDeltaStage::Research:
            if (P.X>-1650) break;
            Key(PC,EKeys::A,false);
            Test.TestTrue(TEXT("Research arm is walkable"),Walk->GetCharacterMovement()->IsMovingOnGround());
            Tap(PC,EKeys::R);
            DeltaStage=EDeltaStage::Reset;
            Resume=Now+.4;
            break;
        case EDeltaStage::Reset:
            Test.TestTrue(TEXT("R replays at Peregrine with fresh route progress"),P.Equals(StartLocation,5.) && !Walk->CompletedBoardingRoute());
            Tap(PC,EKeys::N);
            Tap(PC,EKeys::B);
            Test.TestTrue(TEXT("On-foot ship switch also retains the Glacier size comparison"),!Walk->IsDeltaTree() && Walk->HullLengthMetres()==640);
            Tap(PC,EKeys::N);
            Test.TestTrue(TEXT("Delta returns to its approved length"),Walk->IsDeltaTree() && Walk->HullLengthMetres()==1280);
            return true;
        }
        return false;
    }

    void CheckViewSwitch(AAquilaBoardingWalker* Walker, APlayerController* PC, bool bFrameWholeHull)
    {
        FMinimalViewInfo Before,After;
        Walker->CalcCamera(0,Before);
        const FTransform Footing = Walker->GetActorTransform();
        const bool bReached = Walker->CompletedBoardingRoute();
        Tap(PC,EKeys::B);
        const int32 Other = Walker->HullLengthMetres();
        Test.TestEqual(TEXT("B selects the other hull length"),Other,1920-Length);
        Walker->CalcCamera(0,After);
        const FVector ExpectedEye = bFrameWholeHull ? Before.Location*(double(Other)/Length) : Before.Location;
        Test.TestTrue(TEXT("Size switch retains camera aim, lens and intended eye position"),
            After.Location.Equals(ExpectedEye,.05) && After.Rotation.Equals(Before.Rotation,.001) && After.FOV==Before.FOV);
        Test.TestTrue(TEXT("Same footing and boarding progress across sizes"),Walker->GetActorTransform().Equals(Footing,.01) && Walker->CompletedBoardingRoute()==bReached);
        Tap(PC,EKeys::B);
        Walker->CalcCamera(0,After);
        Test.TestTrue(TEXT("Switch back restores original view"),Walker->HullLengthMetres()==Length && After.Location.Equals(Before.Location,.05));
    }

    bool CheckHull(UWorld* World, AAquilaBoardingWalker* Walker)
    {
        TActorIterator<AAquilaBoardingBay> Bays(World);
        auto* Hull = Bays ? FindObjectFast<UStaticMeshComponent>(*Bays,TEXT("GlacierTongueStudy")) : nullptr;
        if (!Test.TestNotNull(TEXT("The Blender exterior is loaded"),Hull) || !Hull->GetStaticMesh()) return false;
        const double Factor=Length/640.;
        const FVector MeshSize=Hull->GetStaticMesh()->GetBoundingBox().GetSize();
        const FVector CookedSize=Hull->Bounds.GetBox().GetSize();
        Test.TestTrue(TEXT("Cooked hull doubles length, beam and height together"),
            FMath::IsNearlyEqual(CookedSize.X/100.,double(Length),.02) && CookedSize.Equals(MeshSize*Factor,1.));
        auto Trace = [&](FVector A,FVector B,FHitResult& Hit) {
            return Hull->LineTraceComponent(Hit,A*100,B*100,FCollisionQueryParams(NAME_None,true)); };
        FHitResult Roof, Sill, Path;
        const bool bRoof = Trace(FVector(0,40,20)*Factor,FVector(0,40,170)*Factor,Roof);
        const bool bSill = Trace(FVector(0,113,80)*Factor,FVector(0,113,-40)*Factor,Sill);
        Test.TestTrue(TEXT("Roof and separate blade bound the bay"),bRoof && bSill && Roof.ImpactPoint.Z-Sill.ImpactPoint.Z>4000);
        Test.TestTrue(TEXT("Corresponding roof and blade heights follow the hull scale"),
            FMath::IsNearlyEqual(Roof.ImpactPoint.Z,5994.*Factor,4.) && FMath::IsNearlyEqual(Sill.ImpactPoint.Z,-372.*Factor,4.));
        Test.TestFalse(TEXT("Open air between port lip and bay"),Trace(FVector(0,110,25)*Factor,FVector(0,25,25)*Factor,Path));
        int32 Blocked = 0;
        for (int X=-32; X<=32; X+=4)
            for (double Z : {.1,4.,8.,12.,16.})
                if (Trace({double(X),170*Factor,Z},{double(X),27,Z},Path)) ++Blocked;
        Test.TestEqual(TEXT("Shuttle approach rays clear hull and covered crossings"),Blocked,0);
        const FCollisionQueryParams Params(NAME_None,false,Walker);
        auto DeckTrace = [&](FVector A,FVector B,FHitResult& Hit) {
            return World->LineTraceSingleByChannel(Hit,A*100,B*100,ECC_Visibility,Params); };
        FHitResult Left,Right,Lintel,Tread,Overhead;
        const FVector Door(0,2.6,1.6);
        const bool bOpening = DeckTrace(Door,{-4,2.6,1.6},Left) && DeckTrace(Door,{4,2.6,1.6},Right) &&
            DeckTrace(Door,{0,2.6,6},Lintel) && DeckTrace({0,20.5,4},{0,20.5,-1},Tread);
        Test.TestTrue(TEXT("Same physical 2.8 by 3 m doorway and ramp midpoint in each version"),
            bOpening && FMath::IsNearlyEqual(Right.ImpactPoint.X-Left.ImpactPoint.X,280.,1.) &&
            FMath::IsNearlyEqual(Lintel.ImpactPoint.Z,300.,1.) && FMath::IsNearlyEqual(Tread.ImpactPoint.Z,100.,2.));
        Test.TestTrue(TEXT("Standing character stays 1.80 m tall"),FMath::IsNearlyEqual(Walker->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),90.f));
        const bool bOverhead=Trace({0,27.3,18},{0,27.3,180},Overhead);
        Test.TestTrue(TEXT("Hull roof above the same shuttle sill"),bOverhead && Overhead.ImpactPoint.Z>5000*Factor);

        // Measure the unchanged shuttle against the engine meshes drawn in this world.
        TInlineComponentArray<UInstancedStaticMeshComponent*> Parts(*Bays);
        double PortTip=0,StarboardTip=0,Nose=0,Hatch=0,Crown=0,Foot=0;
        int32 Landmarks=0;
        for (auto* Batch : Parts)
        {
            const FBox Bounds=Batch->GetStaticMesh()->GetBoundingBox();
            for (int32 I=0; I<Batch->GetInstanceCount(); ++I)
            {
                FTransform T;
                Batch->GetInstanceTransform(I,T,true);
                const FVector C=T.GetLocation()/100.;
                if (C.Equals({16,62,6.7},.01)) { PortTip=Bounds.TransformBy(T).Max.X; ++Landmarks; }
                else if (C.Equals({-16,62,6.7},.01)) { StarboardTip=Bounds.TransformBy(T).Min.X; ++Landmarks; }
                else if (C.Equals({0,65,8.4},.01)) { Nose=Bounds.TransformBy(T).Max.Y; ++Landmarks; }
                else if (C.Equals({0,31,5.68},.01)) { Hatch=Bounds.TransformBy(T).Min.Y; ++Landmarks; }
                else if (C.Equals({0,73,13.65},.01)) { Crown=Bounds.TransformBy(T).Max.Z; ++Landmarks; }
                else if (C.Equals({7,43,.22},.01)) { Foot=Bounds.TransformBy(T).Min.Z; ++Landmarks; }
            }
        }
        Test.TestTrue(TEXT("Fixed Peregrine length, span and height measured from six visible landmarks"),
            Landmarks==6 && FMath::IsNearlyEqual(Nose-Hatch,6800.,1.) && FMath::IsNearlyEqual(PortTip-StarboardTip,6400.,1.) && FMath::IsNearlyEqual(Crown-Foot,1600.,1.));
        const double HalfWidth=35.5*Factor, FarY=7.+90.*Factor;
        FHitResult PadLeft,PadRight,PadInner,PadOuter,PadFloor,Outside;
        const bool bEdges = DeckTrace({-HalfWidth-2,51,-.45},{-HalfWidth+2,51,-.45},PadLeft)
            && DeckTrace({HalfWidth+2,51,-.45},{HalfWidth-2,51,-.45},PadRight)
            && DeckTrace({14,5,-.45},{14,9,-.45},PadInner)
            && DeckTrace({14,FarY+2,-.45},{14,FarY-2,-.45},PadOuter);
        const double DeckWidth=(PadRight.ImpactPoint.X-PadLeft.ImpactPoint.X)/100.;
        const double DeckLength=(PadOuter.ImpactPoint.Y-PadInner.ImpactPoint.Y)/100.;
        const double PortRoom=(StarboardTip-PadLeft.ImpactPoint.X)/100.;
        const double StarboardRoom=(PadRight.ImpactPoint.X-PortTip)/100.;
        Test.TestTrue(TEXT("All four physical pad edges scale from the fixed walkway joint"), bEdges &&
            FMath::IsNearlyEqual(DeckWidth,71.*Factor,.02) && FMath::IsNearlyEqual(DeckLength,90.*Factor,.02)
            && FMath::IsNearlyEqual(PadInner.ImpactPoint.Y,700.,1.));
        Test.TestTrue(TEXT("Both fixed shuttle wings gain equal clearance on the larger deck"),
            FMath::IsNearlyEqual(PortRoom,HalfWidth-32.,.02) && FMath::IsNearlyEqual(StarboardRoom,HalfWidth-32.,.02));
        Test.TestTrue(TEXT("Outer deck is walkable up to its new perimeter"),
            DeckTrace({HalfWidth-3,FarY-3,3},{HalfWidth-3,FarY-3,-.5},PadFloor) &&
            FMath::IsNearlyZero(PadFloor.ImpactPoint.Z,1.) && PadFloor.ImpactNormal.Z>.99);
        Test.TestFalse(TEXT("No stale pad collision beyond the side"),DeckTrace({HalfWidth+2,51,.5},{HalfWidth+2,51,-.5},Outside));
        Test.TestFalse(TEXT("No stale pad collision beyond the far end"),DeckTrace({14,FarY+2,.5},{14,FarY+2,-.5},Outside));
        Test.AddInfo(FString::Printf(TEXT("Landing pad at %d m: width %.2f m, length %.2f m, side clearances %.2f and %.2f m."),
            Length,DeckWidth,DeckLength,PortRoom,StarboardRoom));
        Test.AddInfo(FString::Printf(TEXT("Comparison: %d m hull; XYZ %.2f %.2f %.2f m; roof at same ramp %.2f m; shuttle %.2f %.2f %.2f m; doorway %.2f x %.2f m."),
            Length,CookedSize.X/100.,CookedSize.Y/100.,CookedSize.Z/100.,Overhead.ImpactPoint.Z/100.,(Nose-Hatch)/100.,(PortTip-StarboardTip)/100.,
            (Crown-Foot)/100.,(Right.ImpactPoint.X-Left.ImpactPoint.X)/100.,Lintel.ImpactPoint.Z/100.));
        Test.AddInfo(FString::Printf(TEXT("Cooked surfaces at %d m, roof %.2f m, blade %.2f m. Approach clear %d/85."),Length,Roof.ImpactPoint.Z/100.,Sill.ImpactPoint.Z/100.,85-Blocked));
        return true;
    }

public:
    FBoardingWalk(FAutomationTestBase& T,FString Path) : Test(T), Frames(MoveTemp(Path)) {}
    virtual bool Update() override
    {
        const double Now=FPlatformTime::Seconds();
        if (Now-Start>180)
        {
            const auto* PC=GEngine->GameViewport->GetWorld()->GetFirstPlayerController();
            Test.AddError(FString::Printf(TEXT("Boarding stage %d, bridge stage %d, timed out at %s"),bDeltaTest?int32(DeltaStage):int32(Stage),
                int32(BridgeStage),PC && PC->GetPawn()?*PC->GetPawn()->GetActorLocation().ToString():TEXT("menu")));
            return true;
        }
        if (Now<Resume || (!Pending.IsEmpty() && (FScreenshotRequest::IsScreenshotRequested() || IFileManager::Get().FileSize(*Pending)<=0))) return false;
        if (!Pending.IsEmpty())
        {
            FImage Frame;
            if (!Test.TestTrue(TEXT("Open captured Windows frame"),FImageUtils::LoadImage(*Pending,Frame))) return true;
            const FImageView Pixels=Frame;
            int32 Samples=0, Visible=0;
            for (int32 Y=Frame.SizeY/5; Y<Frame.SizeY*4/5; Y+=8)
                for (int32 X=Frame.SizeX/5; X<Frame.SizeX*4/5; X+=8)
                {
                    const FLinearColor Pixel=Pixels.GetOnePixelLinear(X,Y);
                    if (Pixel.R+Pixel.G+Pixel.B>.055f) ++Visible;
                    ++Samples;
                }
            Test.TestTrue(FString::Printf(TEXT("World geometry visible in %s"),*FPaths::GetCleanFilename(Pending)),Visible>Samples/25);
            if (bDeltaTest && Pending.EndsWith(TEXT("delta-exterior.png")))
            {
                int32 First=Frame.SizeX, Last=0;
                for (int32 X=0; X<Frame.SizeX; X+=4)
                {
                    int32 Lit=0;
                    for (int32 Y=Frame.SizeY*15/100; Y<Frame.SizeY*85/100; Y+=4)
                    {
                        const FLinearColor Pixel=Pixels.GetOnePixelLinear(X,Y);
                        if (Pixel.R+Pixel.G+Pixel.B>.055f) ++Lit;
                    }
                    if (Lit>=5) { First=FMath::Min(First,X); Last=X; }
                }
                const double Left=100.*First/Frame.SizeX, Right=100.*(Frame.SizeX-1-Last)/Frame.SizeX;
                Test.TestTrue(TEXT("Complete lit silhouette fills over half the screen with space at bow and stern"),
                    Left>=3 && Right>=3 && 100.-Left-Right>55);
                Test.AddInfo(FString::Printf(TEXT("Delta rendered hull side margins %.2f and %.2f percent."),Left,Right));
            }
            Pending.Reset();
        }
        UGameViewportClient* V=GEngine->GameViewport;
        auto* World=V ? V->GetWorld() : nullptr;
        auto* PC=World ? World->GetFirstPlayerController() : nullptr;
        auto* Mode=World ? World->GetAuthGameMode<AAquilaGameMode>() : nullptr;
        if (!PC || !Mode) return false;
        auto* Walk=Cast<AAquilaBoardingWalker>(PC->GetPawn());
        if (Stage == Menu)
        {
            if (Mode->Session()!=EAquilaSession::Menu) return false;
            Test.TestEqual(TEXT("Menu return has no loaded match"),Mode->Campaign.Sequence,0);
            auto& Slate=FSlateApplication::Get();
            const auto Window=V->GetWindow().ToSharedRef();
            Slate.ForceRedrawWindow(Window);
            Slate.ForceRedrawWindow(Window);
            const auto Text=BoardingButtonText(Window);
            if (!Test.TestTrue(TEXT("Boarding Test visible in main menu"),Text.IsValid())) return true;
            const auto VirtualUser=Slate.FindOrCreateVirtualUser(0);
            const int User=VirtualUser->GetUserIndex();
            const FVector2D Pos=Text->GetCachedGeometry().GetAbsolutePositionAtCoordinates({.5,.5});
            const FWidgetPath Path(Window->GetHittestGrid().GetBubblePath(Pos,0,false,User));
            if (!Path.Widgets.GetInternalArray().ContainsByPredicate([](const FArrangedWidget& W) {return W.Widget->GetType()==TEXT("SButton");})) return false;
            Slate.RoutePointerMoveEvent(Path,FPointerEvent(User,FSlateApplication::CursorPointerIndex,Pos,Pos,FVector2D::ZeroVector,{},FModifierKeysState()),false);
            for (bool Down : {true,false})
            {
                TSet<FKey> Held;
                if (Down) Held.Add(EKeys::LeftMouseButton);
                FPointerEvent E(User,FSlateApplication::CursorPointerIndex,Pos,Pos,Held,EKeys::LeftMouseButton,0,FModifierKeysState());
                if (Down) Slate.RoutePointerDownEvent(Path,E); else Slate.RoutePointerUpEvent(Path,E);
            }
            Stage=Reentered;
            return false;
        }
        if (!Walk || Mode->Session()!=EAquilaSession::Boarding) return false;
        if (bDeltaTest) return UpdateDelta(World,PC,Walk,Now);
        const FVector P=Walk->GetActorLocation();
        switch (Stage)
        {
        case Ramp:
            if (!Walk->GetCharacterMovement()->IsMovingOnGround()) return false;
            if (!Test.TestEqual(TEXT("Expected hull on ramp"),Walk->HullLengthMetres(),Length) || !CheckHull(World,Walk)) return true;
            Test.TestTrue(TEXT("Walker stands on two-metre shuttle sill"),FMath::Abs(P.Z-292)<4);
            StartLocation=P;
            CheckViewSwitch(Walk,PC,false);
            CheckMouseSensitivity(Walk,PC);
            RampHeading=PC->GetControlRotation();
            Capture(TEXT("ramp.png"));
            Stage=Canopy;
            break;
        case Canopy:
            if (!bLookingUp)
            {
                PC->SetControlRotation(FRotator(24,-118,0));
                bLookingUp=true;
                Resume=Now+.25;
                break;
            }
            CheckViewSwitch(Walk,PC,false);
            Capture(TEXT("canopy.png"));
            Stage=Exterior;
            break;
        case Exterior:
            if (!Walk->IsOutside())
            {
                PC->SetControlRotation(RampHeading);
                Tap(PC,EKeys::Tab);
                if (!Test.TestTrue(TEXT("Tab enters exterior orbit"),Walk->IsOutside())) return true;
                if (Walk->LocationName()==TEXT("Shuttle berth")) Tap(PC,EKeys::V);
                Resume=Now+.35;
                break;
            }
            CheckViewSwitch(Walk,PC,true);
            Capture(TEXT("exterior.png"));
            Stage=Berth;
            break;
        case Berth:
            if (Walk->LocationName()!=TEXT("Shuttle berth"))
            {
                Tap(PC,EKeys::V);
                if (!Test.TestEqual(TEXT("V frames the dock at shuttle scale"),Walk->LocationName(),FString(TEXT("Shuttle berth")))) return true;
                Resume=Now+.25;
                break;
            }
            CheckViewSwitch(Walk,PC,false);
            Capture(TEXT("berth.png"));
            Stage=Deck;
            break;
        case Deck:
            if (Walk->IsOutside())
            {
                FMinimalViewInfo Before, After;
                Walk->CalcCamera(0,Before);
                CheckMouseSensitivity(Walk,PC);
                Walk->CalcCamera(0,After);
                Test.TestTrue(TEXT("Mouse input orbits the shuttle"),FVector::Dist(Before.Location,After.Location)>10);
                Before=After;
                Tap(PC,EKeys::MouseScrollUp);
                Walk->CalcCamera(0,After);
                Test.TestTrue(TEXT("Wheel moves exterior camera"),FVector::Dist(Before.Location,After.Location)>1000);
                Tap(PC,EKeys::Tab);
                Test.TestTrue(TEXT("Exterior returns to original footing"),Walk->GetActorLocation().Equals(StartLocation,3));
                Key(PC,EKeys::W,true);
            }
            if (P.Y<1240)
            {
                bOnDeck=true;
                Test.TestTrue(TEXT("Capsule descends ramp onto continuous deck"),FMath::Abs(P.Z-92)<4 && Walk->GetCharacterMovement()->IsMovingOnGround());
                Capture(TEXT("deck.png"));
                Stage=Lock;
            }
            break;
        case Lock:
            if (P.Y<110)
            {
                Test.TestTrue(TEXT("Walk passes the outer airlock frame"),Walk->LocationName()==TEXT("Airlock 01") && !Walk->CompletedBoardingRoute());
                Capture(TEXT("airlock.png"));
                Stage=Lobby;
            }
            break;
        case Lobby:
            if (P.Y < -1170)
            {
                Key(PC,EKeys::W,false);
                Test.TestTrue(TEXT("Walk reaches lift lobby through both doorways"),bOnDeck && Walk->CompletedBoardingRoute());
                Test.AddInfo(FString::Printf(TEXT("Continuous ramp to lobby at %d m: travel %.2f m, sill drop %.2f m."),Length,(StartLocation.Y-P.Y)/100.,(StartLocation.Z-P.Z)/100.));
                CheckViewSwitch(Walk,PC,false);
                Capture(TEXT("lobby.png"));
                Key(PC,EKeys::D,true);
                SideTime=World->GetTimeSeconds();
                Stage=Wall;
            }
            break;
        case Wall:
            if (World->GetTimeSeconds()-SideTime>3.2)
            {
                Key(PC,EKeys::D,false);
                Test.TestTrue(TEXT("Physical lobby wall stops sideways movement"),P.X>390 && P.X<462 && Walk->GetCharacterMovement()->IsMovingOnGround());
                Tap(PC,EKeys::R);
                Test.TestTrue(TEXT("R resets the walk"),Walk->GetActorLocation().Equals(StartLocation,5) && !Walk->CompletedBoardingRoute());
                if (Length==1280)
                {
                    Walk->SetActorLocation({5700,15000,180},false,nullptr,ETeleportType::TeleportPhysics);
                    Walk->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
                    Stage=OuterDeck;
                    break;
                }
                Tap(PC,EKeys::Escape);
                Resume=Now+.7;
                Stage=Menu;
            }
            break;
        case OuterDeck:
            if (!Walk->GetCharacterMovement()->IsMovingOnGround()) return false;
            if (!Test.TestTrue(TEXT("Walker stands on the added outer corner"),P.Equals({5700,15000,92},5))) return true;
            Tap(PC,EKeys::B);
            Length=640;
            Stage=ShrunkDeck;
            Resume=Now+.35;
            break;
        case ShrunkDeck:
            Test.TestTrue(TEXT("Shrinking the pad keeps the walker safely on the smaller deck"),
                P.X>3300 && P.X<3450 && P.Y>9500 && P.Y<9650 && FMath::Abs(P.Z-92)<4
                && Walk->GetCharacterMovement()->IsMovingOnGround() && Walk->HullLengthMetres()==640);
            Tap(PC,EKeys::R);
            bLookingUp=bOnDeck=false;
            Stage=Ramp;
            Resume=Now+.35;
            break;
        case Reentered:
            Test.TestTrue(TEXT("Main menu button starts a new 1,280 m boarding walk"),
                P.Y>2600 && Walk->HullLengthMetres()==1280 && !Walk->CompletedBoardingRoute() && Mode->Campaign.Sequence==0);
            return true;
        default: break;
        }
        return false;
    }
};
}

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FAquilaBoardingTest,"Aquila.Input.Boarding",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::NonNullRHI | EAutomationTestFlags::EngineFilter)
void FAquilaBoardingTest::GetTests(TArray<FString>& Names,TArray<FString>& Commands) const
{
    FString Frames,Save,Mode;
    const TCHAR* Args=FCommandLine::Get();
    if (FParse::Value(Args,TEXT("AquilaBoardingFrames="),Frames) && FParse::Value(Args,TEXT("AquilaSaveDir="),Save) &&
        FParse::Value(Args,TEXT("AquilaMode="),Mode) && Mode==TEXT("Boarding"))
    {
        FString Name=DeltaDockRun() ? TEXT("DeltaDock") : TEXT("Walk");
        if (DeltaDockRun() && FParse::Param(Args,TEXT("AquilaBridgeTest")))
            Name=FParse::Param(Args,TEXT("AquilaManualBridge")) ? TEXT("DeltaBridge") : TEXT("BridgeObserver");
        Names.Add(Name);
        Commands.Add(Frames);
    }
}
bool FAquilaBoardingTest::RunTest(const FString& Frames)
{
    IFileManager::Get().MakeDirectory(*Frames,true);
    ADD_LATENT_AUTOMATION_COMMAND(FBoardingWalk(*this,Frames));
    return true;
}
#endif
