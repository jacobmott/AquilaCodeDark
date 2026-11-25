#if WITH_DEV_AUTOMATION_TESTS && PLATFORM_WINDOWS
#include "AquilaBoarding.h"
#include "AquilaBoardingBay.h"
#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "AquilaUITest.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UnrealClient.h"
#include "Widgets/SWindow.h"

class FAquilaDuelShipJump : public IAutomationLatentCommand
{
    enum class EStage { Start, Switch, Glacier, Hit, Walk, Back, Revisit, Outside, Board, Defeat, Survivor, Reject };
    EStage Stage=EStage::Start;
    FAutomationTestBase& Test;
    FString Frames,Pending;
    TWeakObjectPtr<AAquilaFlightPawn> Arena;
    FTransform DeltaPlace,GlacierPlace;
    FVector GlacierStart;
    double Deadline=FPlatformTime::Seconds()+75.,StartedWalking=0,PausedAt=0;

    void Key(APlayerController* PC,FKey Which,bool bDown)
    {
        PC->InputKey(FInputKeyEventArgs::CreateSimulated(Which,bDown ? IE_Pressed : IE_Released,bDown ? 1.f : 0.f));
        PC->PlayerTick(1.f/60);
    }
    void Press(APlayerController* PC,FKey Which)
    {
        Key(PC,Which,true);
        Key(PC,Which,false);
    }
    FTransform Place(APlayerController* PC) const
    {
        return FTransform(PC->GetControlRotation(),PC->GetPawn()->GetActorLocation());
    }
    bool SamePlace(APlayerController* PC,const FTransform& Before)
    {
        const FTransform Now=Place(PC);
        return Test.TestTrue(TEXT("Each ship remembers the visitor's footing and look"),
            Now.GetRotation().Equals(Before.GetRotation(),.0005) && Now.GetLocation().Equals(Before.GetLocation(),4.));
    }
    void Capture(const TCHAR* Name)
    {
        Pending=Frames/Name;
        FScreenshotRequest::RequestScreenshot(Pending,true,false);
    }
    bool Jump(APlayerController* PC,AAquilaGameMode* Game,bool bGlacier)
    {
        const auto& C=Game->Campaign;
        const auto Rounds=C.Rounds;
        const auto ID=C.CampaignId;
        const double When=C.BattleTime;
        const auto Paused=Game->bPaused,Fast=Game->bFastDuel;
        const FVector4 Health(C.Hull,C.EnemyHull,C.Shield,C.EnemyShield);
        APawn* Walker=PC->GetPawn();
        Press(PC,EKeys::N);
        if (!Test.TestTrue(TEXT("N transfers the same walking visitor directly to the other live bridge"),
            Walker==PC->GetPawn() && Walker==Arena->DuelVisitor && Arena->bOnGlacier==bGlacier &&
            !Arena->DuelDecks[bGlacier]->IsHidden() && Arena->DuelDecks[bGlacier]->GetActorEnableCollision() &&
            Arena->DuelDecks[!bGlacier]->IsHidden() && !Arena->DuelDecks[!bGlacier]->GetActorEnableCollision())) return false;
        if (!Test.TestTrue(TEXT("Transfer preserves this duel, both defenses, clock, pause, speed and airborne round count"),
            ID==C.CampaignId && When==C.BattleTime && Paused==Game->bPaused && Fast==Game->bFastDuel &&
            Health==FVector4(C.Hull,C.EnemyHull,C.Shield,C.EnemyShield) && Rounds.Num()==C.Rounds.Num() && !Rounds.IsEmpty())) return false;
        for (int32 I=0; I<Rounds.Num(); ++I)
            if (!Test.TestTrue(TEXT("Round identity, position and velocity survive a bridge transfer"),Rounds[I].Id==C.Rounds[I].Id &&
                Rounds[I].Position==C.Rounds[I].Position && Rounds[I].Velocity==C.Rounds[I].Velocity)) return false;
        return true;
    }
public:
    FAquilaDuelShipJump(FAutomationTestBase& InTest,FString Out) : Test(InTest),Frames(MoveTemp(Out)) {}
    bool Update() override
    {
        if (FPlatformTime::Seconds()>Deadline) { Test.AddError(FString::Printf(TEXT("Bridge switch timed out at stage %d"),int32(Stage))); return true; }
        if (!Pending.IsEmpty() && (FScreenshotRequest::IsScreenshotRequested() || IFileManager::Get().FileSize(*Pending)<=0)) return false;
        Pending.Reset();
        UGameViewportClient* Viewport=GEngine->GameViewport;
        auto* World=Viewport ? Viewport->GetWorld() : nullptr;
        auto* Game=World ? World->GetAuthGameMode<AAquilaGameMode>() : nullptr;
        auto* PC=World ? World->GetFirstPlayerController() : nullptr;
        if (!Game || !PC) return false;
        auto* Visitor=Cast<AAquilaBoardingWalker>(PC->GetPawn());
        auto& C=Game->Campaign;
        switch (Stage)
        {
        case EStage::Start:
            if (!Visitor || !Visitor->GetCharacterMovement()->IsMovingOnGround() || C.Rounds.IsEmpty()) break;
            for (TActorIterator<AAquilaFlightPawn> It(World); It; ++It)
                if (It->DuelVisitor==Visitor) { Arena=*It; break; }
            if (!Test.TestTrue(TEXT("Direct duel opens at Delta Tree"),Arena.IsValid() && !Arena->bOnGlacier && C.Hull>0 && C.EnemyHull>0)) return true;
            DeltaPlace=Place(PC);
            Capture(TEXT("delta-window.png"));
            Stage=EStage::Switch;
            break;
        case EStage::Switch:
            if (C.Rounds.IsEmpty()) break;
            if (!Jump(PC,Game,true)) return true;
            Stage=EStage::Glacier;
            break;
        case EStage::Glacier:
        {
            if (!Visitor || !Visitor->GetCharacterMovement()->IsMovingOnGround()) break;
            auto* Deck=Arena->DuelDecks[1].Get();
            const FVector Anchor=Arena->HullBodies[1]->GetSocketLocation(TEXT("CommandBridge"));
            const FVector Eye=Visitor->GetPawnViewLocation(), Other=Arena->ShipRoot->GetComponentLocation();
            Test.TestTrue(TEXT("Glacier's floor is registered to its cooked observation crown"),!Deck->IsDeltaTree() && Deck->GetActorLocation().Equals(Anchor,2.));
            PC->PlayerCameraManager->UpdateCamera(0);
            int32 W,H; PC->GetViewportSize(W,H);
            FVector2D Spot;
            Test.TestTrue(TEXT("Delta Tree is framed inside the forward window"),PC->ProjectWorldLocationToScreen(Other,Spot) &&
                Spot.X>W*.15 && Spot.X<W*.85 && Spot.Y>H*.1 && Spot.Y<H*.9);
            FHitResult Block;
            Test.TestFalse(TEXT("The visitor can see the other ship over Glacier's own roof"),
                Arena->HullBodies[1]->LineTraceComponent(Block,Eye,Other,FCollisionQueryParams(NAME_None,true)));
            Test.AddInfo(FString::Printf(TEXT("Glacier bridge: opponent at %.1f %.1f of %d x %d; crown registration %.3f cm."),
                Spot.X,Spot.Y,W,H,FVector::Dist(Deck->GetActorLocation(),Anchor)));
            Capture(TEXT("glacier-window.png"));
            Stage=EStage::Hit;
            break;
        }
        case EStage::Hit:
        {
            const auto* Hit=C.Impacts.FindByPredicate([](const FAquilaImpact& H) {return H.bTargetEnemy && H.bShield && H.Age>.075 && H.Age<.36;});
            if (!Hit || C.Rounds.IsEmpty()) break;
            Game->Command(8);
            PausedAt=C.BattleTime;
            auto* Deck=Arena->DuelDecks[1].Get();
            const auto Color=[&]() {return Deck->Materials[AAquilaBoardingBay::ImpactRail]->K2_GetVectorParameterValue(TEXT("Color"));};
            Deck->UpdateDuelInstruments(C);
            Test.TestTrue(TEXT("Incoming Glacier shield strike lights Glacier's sill"),Color().B>.25f && C.EnemyShield<100);
            FAquilaCampaign Opposite;
            Opposite.Impacts.Add(*Hit);
            Opposite.Impacts.Last().bTargetEnemy=false;
            Deck->UpdateDuelInstruments(Opposite);
            Test.TestTrue(TEXT("A strike on Delta cannot light Glacier's sill"),Color().B<.05f);
            Deck->UpdateDuelInstruments(C);
            Test.AddInfo(FString::Printf(TEXT("Glacier shield contact %llu at %.3f s; shield %.3f; %d airborne rounds."),
                Hit->Serial,PausedAt,C.EnemyShield,C.Rounds.Num()));
            Capture(TEXT("glacier-hit.png"));
            GlacierStart=Visitor->GetActorLocation();
            Stage=EStage::Walk;
            break;
        }
        case EStage::Walk:
            if (!StartedWalking) { Key(PC,EKeys::S,true); StartedWalking=World->GetTimeSeconds(); }
            if (World->GetTimeSeconds()-StartedWalking<.32) break;
            if (PC->IsInputKeyDown(EKeys::S)) Key(PC,EKeys::S,false);
            if (Visitor->GetVelocity().Size()>1.) break;
            Test.AddInfo(FString::Printf(TEXT("Glacier walk: %.1f cm, grounded %d, start %s, end %s."),
                FVector::Dist(GlacierStart,Visitor->GetActorLocation()),Visitor->GetCharacterMovement()->IsMovingOnGround(),
                *GlacierStart.ToCompactString(),*Visitor->GetActorLocation().ToCompactString()));
            Test.TestTrue(TEXT("Glacier has a walkable floor while the duel is paused"),
                FVector::Dist(GlacierStart,Visitor->GetActorLocation())>35 && Visitor->GetCharacterMovement()->IsMovingOnGround());
            PC->SetControlRotation(PC->GetControlRotation()+FRotator(0,13,0));
            GlacierPlace=Place(PC);
            Stage=EStage::Back;
            break;
        case EStage::Back:
            if (!Jump(PC,Game,false) || !SamePlace(PC,DeltaPlace)) return true;
            Stage=EStage::Revisit;
            break;
        case EStage::Revisit:
            if (!Jump(PC,Game,true) || !SamePlace(PC,GlacierPlace)) return true;
            Capture(TEXT("glacier-return.png"));
            Stage=EStage::Outside;
            break;
        case EStage::Outside:
            Press(PC,EKeys::Tab);
            if (!Test.TestTrue(TEXT("Tab from Glacier opens the exterior at the same instant"),
                PC->GetPawn()==Arena.Get() && Game->bPaused && PausedAt==C.BattleTime)) return true;
            Stage=EStage::Board;
            break;
        case EStage::Board:
        {
            auto Window=Viewport->GetWindow().ToSharedRef();
            FSlateApplication::Get().ForceRedrawWindow(Window);
            if (!AquilaUITest::ClickText(Test,Window,TEXT("Watch from bridge"))) return true;
            if (!Test.TestTrue(TEXT("Watch from bridge returns to Glacier"),Arena->DuelVisitor && Arena->bOnGlacier) || !SamePlace(PC,GlacierPlace)) return true;
            Stage=EStage::Defeat;
            break;
        }
        case EStage::Defeat:
            // Shorten the remaining fight in this disposable client and let a real fitted round finish it.
            C.EnemyHull=.01; C.EnemyShield=0; C.Loadout.Sentinel.Armor=EAquilaArmor::BareHull;
            Game->Command(8);
            for (int32 I=0; I<3000 && C.EnemyHull>0; ++I) Game->Tick(.125f);
            if (!Test.TestTrue(TEXT("A real round defeats the ship the visitor is watching from"),C.EnemyHull==0 && C.Hull>0)) return true;
            Stage=EStage::Survivor;
            break;
        case EStage::Survivor:
            if (PC->GetPawn()!=Arena.Get()) break;
            Arena->ToggleBridge();
            if (PC->GetPawn()==Arena.Get()) break;
            if (!Test.TestTrue(TEXT("Returning from a wreck places the visitor on the surviving ship"),
                !Arena->bOnGlacier && Arena->DuelVisitor && C.EnemyHull==0)) return true;
            Stage=EStage::Reject;
            break;
        case EStage::Reject:
            Press(PC,EKeys::N);
            Test.TestTrue(TEXT("A destroyed bridge cannot be boarded"),!Arena->bOnGlacier && PC->GetPawn()==Arena->DuelVisitor);
            return true;
        }
        return false;
    }
};

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FAquilaDuelShipJumpTest,"Aquila.Input.DuelBridges",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::NonNullRHI | EAutomationTestFlags::EngineFilter)
void FAquilaDuelShipJumpTest::GetTests(TArray<FString>& Names,TArray<FString>& Commands) const
{
    FString Dir,Frames;
    if (FParse::Param(FCommandLine::Get(),TEXT("AquilaDuelAboard")) && FParse::Value(FCommandLine::Get(),TEXT("AquilaSaveDir="),Dir) &&
        FParse::Value(FCommandLine::Get(),TEXT("AquilaBridgeSwitchFrames="),Frames)) {Names.Add(TEXT("Switch")); Commands.Add(Frames);}
}
bool FAquilaDuelShipJumpTest::RunTest(const FString& Frames)
{
    IFileManager::Get().MakeDirectory(*Frames,true);
    ADD_LATENT_AUTOMATION_COMMAND(FAquilaDuelShipJump(*this,Frames));
    return true;
}
#endif
