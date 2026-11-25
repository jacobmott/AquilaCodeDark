#if WITH_DEV_AUTOMATION_TESTS && PLATFORM_WINDOWS
#include "AquilaBoarding.h"
#include "AquilaBoardingBay.h"
#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "AquilaUITest.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UnrealClient.h"
#include "Widgets/SWindow.h"

class FAquilaDuelBridgeWalk : public IAutomationLatentCommand
{
    enum class EStage { Fit, FitGlacier, ShowGlacier, Start, Walk, Volley, Leave, Contact, BoardAgain, Window,
        InspectDelta, InspectGlacier, ReturnAboard, Finish, Replay };
    EStage Stage=EStage::Fit;
    FAutomationTestBase& Test;
    FString Frames, Pending, Battle;
    TWeakObjectPtr<AAquilaFlightPawn> Arena;
    double Deadline=FPlatformTime::Seconds()+80., WalkStarted=0, ShieldBefore=0, PausedAt=0, ShieldAfter=0;
    uint64 RoundId=0, ImpactSerial=0;
    FVector Start,Footing;
    FRotator Look;

    void Capture(const TCHAR* Name)
    {
        Pending=Frames/Name;
        FScreenshotRequest::RequestScreenshot(Pending,true,false);
    }
    bool PlatingReady(int32 Side) const
    {
        auto* Body=Arena->HullBodies[Side].Get();
        if (Body->IsPSOPrecaching() || !Body->IsMipStreamingForced()) return false;
        auto* Plating=Body->GetMaterial(Body->GetMaterialIndex(TEXT("Armor")));
        if (!Plating) return false;
        for (const FName Channel : {FName(TEXT("BaseMap")),FName(TEXT("NormalMap")),FName(TEXT("SurfaceMap"))})
        {
            UTexture* Map=nullptr;
            if (!Plating->GetTextureParameterValue(Channel,Map) || !Map || !Map->IsFullyStreamedIn()) return false;
        }
        return true;
    }
    static void Key(APlayerController* PC,FKey Which,bool bDown)
    {
        PC->InputKey(FInputKeyEventArgs::CreateSimulated(Which,bDown ? IE_Pressed : IE_Released,bDown ? 1.f : 0.f));
        PC->PlayerTick(1.f/60);
    }
    bool CheckSeats()
    {
        double WorstGap=-100.;
        int32 Seated=0;
        for (int32 Side=0; Side<2; ++Side)
        {
            auto* Body=Arena->HullBodies[Side].Get();
            const FName Expected(Side ? TEXT("SM_GlacierBoardingHull") : TEXT("SM_DeltaBoardingHull"));
            if (!Test.TestTrue(TEXT("Flight loads the actual boarding study"),Body && Body->GetStaticMesh()->GetFName()==Expected)) return false;
            const auto& Ship=Body->GetAttachParent()->GetComponentTransform();
            for (int32 Mount=0; Mount<FAquilaTurret::MountCount; ++Mount)
            {
                const FTransform Pose=FAquilaTurret{Mount}.MountedPose(Side!=0)*Ship;
                const FVector Up=Pose.GetUnitAxis(EAxis::Z);
                const FName Socket=AquilaHulls::GunSocket(Mount);
                if (!Test.TestTrue(TEXT("Numbered port exists on the cooked hull"),Body->DoesSocketExist(Socket))) return false;
                const FVector Face=Body->GetSocketLocation(Socket);
                FHitResult Center;
                if (!Test.TestTrue(TEXT("The socket lies on a hull triangle"),Body->LineTraceComponent(Center,Face+Up*6.,Face-Up*6.,
                    FCollisionQueryParams(NAME_None,true)) && Center.ImpactPoint.Equals(Face,.08))) return false;
                for (int32 K=0; K<8; ++K)
                {
                    const double Angle=UE_TWO_PI*K/8.;
                    const FVector Foot=Pose.TransformPosition(FVector(-9.+25.*FMath::Cos(Angle),25.*FMath::Sin(Angle),-8.));
                    FHitResult Hit;
                    if (!Test.TestTrue(TEXT("The hull supports the full gun foot"),Body->LineTraceComponent(Hit,Foot+Up*16.,Foot-Up*16.,
                        FCollisionQueryParams(NAME_None,true)))) return false;
                    WorstGap=FMath::Max(WorstGap,FVector::DotProduct(Foot-Hit.ImpactPoint,Up));
                }
                ++Seated;
            }
        }
        Test.AddInfo(FString::Printf(TEXT("Delta and Glacier hulls: %d seats, %d rim probes, largest signed foot gap %.3f arena units."),
            Seated,Seated*8,WorstGap));
        return Test.TestTrue(TEXT("No gun foot floats above its support"),WorstGap<=.10);
    }
    void FrameMountedGun(AAquilaGameMode* Game,APlayerController* PC,bool bEnemy)
    {
        const auto& C=Game->Campaign;
        const auto& Gun=C.Loadout.ForShip(bEnemy).Rack->Stock[0];
        const FRotator Heading(0,bEnemy ? 180. : FMath::RadiansToDegrees(C.Heading),0);
        const FVector Focus=Gun.MountedPose(bEnemy).TransformPosition(FVector(-3,0,7));
        Arena->OrbitYaw=Heading.Yaw+(bEnemy ? 104. : 116.);
        Arena->OrbitPitch=19.;
        Arena->OrbitDistance=125.;
        Arena->FrameFreeView((bEnemy ? C.EnemyPosition : C.ShipPosition)+Heading.RotateVector(Focus)/Arena->ExteriorScale);
        Arena->UpdateCamera(0);
        Arena->RenderCampaign(0);
        PC->PlayerCameraManager->UpdateCamera(0);
    }
public:
    FAquilaDuelBridgeWalk(FAutomationTestBase& T,FString Path):Test(T),Frames(MoveTemp(Path)) {}
    virtual bool Update() override
    {
        if (FPlatformTime::Seconds()>Deadline)
        {
            Test.AddError(FString::Printf(TEXT("Duel bridge timed out at stage %d"),int32(Stage)));
            return true;
        }
        if (!Pending.IsEmpty() && (FScreenshotRequest::IsScreenshotRequested() || IFileManager::Get().FileSize(*Pending)<=0)) return false;
        Pending.Reset();
        UGameViewportClient* Viewport=GEngine->GameViewport;
        auto* World=Viewport ? Viewport->GetWorld() : nullptr;
        auto* Game=World ? World->GetAuthGameMode<AAquilaGameMode>() : nullptr;
        auto* PC=World ? World->GetFirstPlayerController() : nullptr;
        if (!Game || !PC || !Viewport->GetWindow()) return false;
        auto& C=Game->Campaign;
        auto* Visitor=Cast<AAquilaBoardingWalker>(PC->GetPawn());
        auto Click=[&](const TCHAR* Text)
        {
            const auto Window=Viewport->GetWindow().ToSharedRef();
            auto& Slate=FSlateApplication::Get();
            if (Arena.IsValid() && PC->GetPawn()==Arena.Get()) Arena->Tick(0);
            PC->PlayerCameraManager->UpdateCamera(0);
            Slate.ForceRedrawWindow(Window);
            Slate.ForceRedrawWindow(Window);
            return AquilaUITest::ClickText(Test,Window,Text);
        };
        switch (Stage)
        {
        case EStage::Fit:
            Arena=Cast<AAquilaFlightPawn>(PC->GetPawn());
            if (!Arena.IsValid()) return false;
            if (!Test.TestTrue(TEXT("Direct playtest waits in fitting with no elapsed fight"),
                Game->Session()==EAquilaSession::Simulator && Game->bFittingOpen && C.BattleTime==0)) return true;
            if (!PlatingReady(0)) return false;
            if (!CheckSeats()) return true;
            Capture(TEXT("delta-fitting.png"));
            Stage=EStage::FitGlacier;
            break;
        case EStage::FitGlacier:
            if (!Click(TEXT("Glacier Tongue")) || !Click(TEXT("Piercing"))) return true;
            Stage=EStage::ShowGlacier;
            break;
        case EStage::ShowGlacier:
            if (!PlatingReady(1)) return false;
            Test.AddInfo(TEXT("Both fitting hulls streamed full color, normal and surface maps before close inspection."));
            Capture(TEXT("fitting.png"));
            Stage=EStage::Start;
            break;
        case EStage::Start:
            if (!Click(TEXT("Start duel aboard"))) return true;
            if (!Test.TestTrue(TEXT("Real launch opens the bridge and uses both fittings"),
                PC->GetPawn()==Arena->DuelVisitor && Arena->DuelVisitor && !Game->bFittingOpen && !Game->bFastDuel &&
                C.Loadout.Sentinel.Rack->Stock[0].Ammo==EAquilaAmmo::Piercing && C.Loadout.Resolute.Rack->Installed()==2)) return true;
            Battle=C.CampaignId;
            Stage=EStage::Walk;
            break;
        case EStage::Walk:
            if (!Visitor || !Visitor->GetCharacterMovement()->IsMovingOnGround()) break;
            Test.TestTrue(TEXT("Bridge uses physical floor and hides the arena overlay"),Arena->DuelDecks[0] &&
                !Arena->Overlay->GetVisibility().IsVisible() && !PC->bShowMouseCursor);
            Test.TestTrue(TEXT("The occupied command deck shares the real boarding island datum"),
                Arena->HullBodies[0]->GetSocketLocation(TEXT("BoardingDock")).Equals(
                    Arena->DuelDecks[0]->GetActorTransform().TransformPosition(FVector(0,-1700,0)),2.));
            Test.TestFalse(TEXT("Visitor has no weapon input"),Visitor->InputComponent->KeyBindings.ContainsByPredicate(
                [](const FInputKeyBinding& B){return B.Chord.Key==EKeys::T || B.Chord.Key==EKeys::SpaceBar;}));
            Start=Visitor->GetActorLocation();
            WalkStarted=World->GetTimeSeconds();
            Key(PC,EKeys::A,true);
            Stage=EStage::Volley;
            break;
        case EStage::Volley:
        {
            if (!Visitor) { Test.AddError(TEXT("Lost bridge visitor before volley")); return true; }
            if (World->GetTimeSeconds()-WalkStarted<.15) break;
            if (PC->IsInputKeyDown(EKeys::A)) Key(PC,EKeys::A,false);
            const auto* Shot=C.Rounds.FindByPredicate([](const FAquilaRound& R){return R.bEnemy && R.Age<1.2;});
            if (!Shot || Visitor->GetVelocity().Size()>1. || !C.Rounds.ContainsByPredicate([](const FAquilaRound& R){return !R.bEnemy;})) break;
            if (!Test.TestTrue(TEXT("Both fitted batteries fire while the visitor walks"),
                Shot->Ammo==EAquilaAmmo::Piercing && FVector::Dist(Start,Visitor->GetActorLocation())>40.)) return true;
            Footing=Visitor->GetActorLocation();
            Look=PC->GetControlRotation();
            RoundId=Shot->Id;
            ShieldBefore=C.Shield;
            Capture(TEXT("bridge-volley.png"));
            Stage=EStage::Leave;
            break;
        }
        case EStage::Leave:
        {
            const auto* R=C.Rounds.FindByPredicate([this](const FAquilaRound& Shot){return Shot.Id==RoundId;});
            if (!Test.TestNotNull(TEXT("Selected incoming round is still in flight"),R)) return true;
            const FVector Where=R->Position;
            const double When=C.BattleTime;
            Key(PC,EKeys::Tab,true);
            Key(PC,EKeys::Tab,false);
            const auto* Same=C.Rounds.FindByPredicate([this](const FAquilaRound& Shot){return Shot.Id==RoundId;});
            if (!Test.TestTrue(TEXT("Tab changes the observer without restarting or moving the projectile"),
                PC->GetPawn()==Arena.Get() && !Arena->DuelVisitor && Same && Same->Position==Where && C.CampaignId==Battle &&
                C.BattleTime==When && C.Shield==ShieldBefore && !Game->bPaused)) return true;
            Stage=EStage::Contact;
            break;
        }
        case EStage::Contact:
        {
            const auto* Hit=C.Impacts.FindByPredicate([this](const FAquilaImpact& H){return H.RoundId==RoundId;});
            if (!Hit || Hit->Age<.14) break;
            if (!Test.TestTrue(TEXT("Tracked enemy round hits Delta Tree's shield in the arena"),
                !Hit->bTargetEnemy && Hit->bShield && C.Shield<ShieldBefore)) return true;
            Game->Command(8);
            PausedAt=C.BattleTime;
            ShieldAfter=C.Shield;
            ImpactSerial=Hit->Serial;
            Capture(TEXT("arena-shield.png"));
            Stage=EStage::BoardAgain;
            break;
        }
        case EStage::BoardAgain:
            if (!Click(TEXT("Watch from bridge"))) return true;
            Stage=EStage::Window;
            break;
        case EStage::Window:
        {
            if (!Test.TestTrue(TEXT("Returning preserves paused clock, shield charge, free look and footing"),
                Visitor && C.CampaignId==Battle && C.BattleTime==PausedAt && Game->bPaused && C.Shield==ShieldAfter &&
                PC->GetControlRotation().Equals(Look,.04) && Visitor->GetActorLocation().Equals(Footing,4.))) return true;
            const auto* Burst=Arena->Bursts.FindByPredicate([this](const FAquilaVisualBurst& B){
                return B.Serial==ImpactSerial && !B.bEnemy && B.Kind==EAquilaBurst::Shield;
            });
            if (!Test.TestNotNull(TEXT("The same impact powers the bridge flare"),Burst)) return true;
            const FVector Hit=Arena->ShipRoot->GetComponentTransform().TransformPosition(Burst->MeshPoint);
            FVector2D Screen;
            int32 W,H;
            PC->GetViewportSize(W,H);
            Test.TestTrue(TEXT("Actual own-shield contact projects through the forward windows"),
                PC->ProjectWorldLocationToScreen(Hit,Screen) && Screen.X>0 && Screen.X<W && Screen.Y>0 && Screen.Y<H);
            Test.AddInfo(FString::Printf(TEXT("Delta duel bridge: incoming round %llu, contact %llu, shield %.3f to %.3f, paused %.3f s, window %.1f %.1f in %d by %d."),
                RoundId,ImpactSerial,ShieldBefore,ShieldAfter,PausedAt,Screen.X,Screen.Y,W,H));
            Capture(TEXT("bridge-shield.png"));
            Stage=EStage::InspectDelta;
            break;
        }
        case EStage::InspectDelta:
            Key(PC,EKeys::Tab,true);
            Key(PC,EKeys::Tab,false);
            if (!Test.TestTrue(TEXT("The exterior inspector keeps the paused duel"),PC->GetPawn()==Arena.Get() && Game->bPaused)) return true;
            FrameMountedGun(Game,PC,false);
            Capture(TEXT("delta-mount.png"));
            Stage=EStage::InspectGlacier;
            break;
        case EStage::InspectGlacier:
            FrameMountedGun(Game,PC,true);
            Capture(TEXT("glacier-mount.png"));
            Stage=EStage::ReturnAboard;
            break;
        case EStage::ReturnAboard:
            if (!Click(TEXT("Watch from bridge"))) return true;
            Stage=EStage::Finish;
            break;
        case EStage::Finish:
            Game->Command(8);
            for (int32 Step=0; Step<30000 && C.Hull>0; ++Step) Game->Tick(.125f);
            if (!Test.TestTrue(TEXT("Opponent's fitted battery defeats the visitor's ship"),C.Hull==0)) return true;
            Stage=EStage::Replay;
            break;
        case EStage::Replay:
            if (PC->GetPawn()!=Arena.Get()) break;
            Test.TestTrue(TEXT("Defeat returns the visitor to the same finished fight"),!Arena->DuelVisitor && C.CampaignId==Battle &&
                Arena->Overlay->GetVisibility().IsVisible() && Arena->GetActorScale3D().Equals(FVector::OneVector));
            if (!Click(TEXT("Replay aboard"))) return true;
            Test.TestTrue(TEXT("Replay creates a fresh duel using the fitted ammunition"),PC->GetPawn()==Arena->DuelVisitor &&
                C.CampaignId!=Battle && C.Hull==100 && C.Shield==100 && C.Loadout.Sentinel.Rack->Stock[0].Ammo==EAquilaAmmo::Piercing);
            return true;
        }
        return false;
    }
};

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FAquilaDuelBridgeTest,"Aquila.Input.DuelBridge",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::NonNullRHI | EAutomationTestFlags::EngineFilter)
void FAquilaDuelBridgeTest::GetTests(TArray<FString>& Names,TArray<FString>& Commands) const
{
    FString Frames,Dir;
    if (!FParse::Value(FCommandLine::Get(),TEXT("AquilaSaveDir="),Dir)) return;
    if (FParse::Param(FCommandLine::Get(),TEXT("AquilaDuelAboard")))
    { Names.Add(TEXT("Direct")); Commands.Add(TEXT("Direct")); }
    if (FParse::Param(FCommandLine::Get(),TEXT("AquilaDuelSetup")) &&
        FParse::Value(FCommandLine::Get(),TEXT("AquilaDuelBridgeFrames="),Frames))
    { Names.Add(TEXT("Window")); Commands.Add(Frames); }
}
bool FAquilaDuelBridgeTest::RunTest(const FString& Frames)
{
    if (Frames==TEXT("Direct"))
    {
        const auto* World=GEngine->GameViewport->GetWorld();
        const auto* Game=World->GetAuthGameMode<AAquilaGameMode>();
        return TestTrue(TEXT("Launch reaches the walkable duel at normal speed without a menu or input"),
            Game && Game->Session()==EAquilaSession::Simulator && !Game->bFittingOpen && !Game->bFastDuel &&
            Cast<AAquilaBoardingWalker>(World->GetFirstPlayerController()->GetPawn()) && Game->Campaign.BattleTime>0);
    }
    IFileManager::Get().MakeDirectory(*Frames,true);
    ADD_LATENT_AUTOMATION_COMMAND(FAquilaDuelBridgeWalk(*this,Frames));
    return true;
}
#endif
