#if WITH_DEV_AUTOMATION_TESTS && PLATFORM_WINDOWS

#include "AquilaFlightPawn.h"
#include "AquilaGameMode.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UnrealClient.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAquilaShieldFitTest, "Aquila.Combat.FittedShieldGeometry",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAquilaShieldFitTest::RunTest(const FString&)
{
    const FVector Center(64, -30, 23);
    for (const FVector Extent : {FVector(150), FVector(100, 80, 480), FVector(600, 150, 80)})
    {
        const TArray<FBox> Parts = {FBox(Center - Extent, Center + Extent)};
        const auto Shell = FAquilaShieldEnvelope::Fit(Parts);
        TestTrue(TEXT("Fits off-center hull with clearance at every corner"), Shell.Center.Equals(Center) &&
            (Extent / Shell.Radii).Size() < 1);
        TestTrue(TEXT("Compact, tall and long hulls keep their proportions"),
            (Extent.X == Extent.Z ? Shell.Radii.X == Shell.Radii.Z && Shell.Radii.Y == Shell.Radii.X :
                (Extent.Z > Extent.X) == (Shell.Radii.Z > Shell.Radii.X)));
        FVector Normal;
        const FVector Tip = Shell.Contact(Center, FVector::ForwardVector, Normal);
        TestTrue(TEXT("Incoming shot finds the facing end"), Tip.Equals(Center - FVector(Shell.Radii.X, 0, 0), 0.001) &&
            Normal.Equals(-FVector::ForwardVector, 0.001));
        const FVector Graze = Shell.Contact(Center + FVector(0, 0, 2 * Shell.Radii.Z), FVector::ForwardVector, Normal);
        TestTrue(TEXT("A grazing hit stays attached to the oval"), Graze.Equals(Center + FVector(0, 0, Shell.Radii.Z), 0.001));
    }
    return true;
}

// Drive the packaged duel at a known wall-clock cadence. Optional frames come from its real viewport.
class FAquilaCombatFeedbackFrames : public IAutomationLatentCommand
{
    enum class EShot { Fitting, Quiet, Shield, Hull, SentinelLoss, Armor, ResoluteLoss, Done };
    FAutomationTestBase& Test;
    AAquilaGameMode* Game;
    AAquilaFlightPawn* Pawn;
    FString Directory, WaitingPath;
    double RequestedAt = 0;
    EShot Shot = EShot::Fitting;
    int32 Frame = 0, SeekSteps = 0;
    bool bFound = false;
    bool bCapturedOval = false;
    double ShapeChangedAt = 0;

    bool ShieldSurfacesReady() const
    {
        auto Ready = [](const UPrimitiveComponent* Part) { return Part->GetSceneProxy() && !Part->IsPSOPrecaching(); };
        for (int32 I = 0; I < 2; ++I)
        {
            if (!Pawn->bDuelMeshShield)
            {
                if (!Ready(Pawn->ShieldShells[I])) return false;
                continue;
            }
            for (const USceneComponent* Part : Pawn->HullShieldRoots[I]->GetAttachChildren())
                if (!Ready(CastChecked<UPrimitiveComponent>(Part))) return false;
        }
        return true;
    }

    void Duel(bool bPiercing)
    {
        FAquilaDuelLoadout Loadout;
        Loadout.Resolute.bPiercing = bPiercing;
        Game->StartDuel(Loadout);
        Game->bFastDuel = true;
        Pawn->ResetSessionView();
        Pawn->Tick(0);
    }

    void Step()
    {
        constexpr float Elapsed = 1.f / 48;
        Game->Tick(Elapsed);
        Pawn->Tick(Elapsed);
    }

    const FAquilaVisualBurst* Current() const
    {
        return Pawn->Bursts.FindByPredicate([this](const FAquilaVisualBurst& Burst)
        {
            if (Burst.Age != 0) return false;
            if (Shot == EShot::ResoluteLoss) return !Burst.bEnemy && Burst.Kind == EAquilaBurst::Destruction;
            if (!Burst.bEnemy) return false;
            switch (Shot)
            {
            case EShot::Shield: return Burst.Kind == EAquilaBurst::Shield;
            case EShot::Hull: return Burst.Kind == EAquilaBurst::Hull && Game->Campaign.EnemyHull > 0 && !Pawn->HullShieldRoots[1]->IsVisible();
            case EShot::Armor: return Burst.Kind == EAquilaBurst::Armor;
            case EShot::SentinelLoss: return Burst.Kind == EAquilaBurst::Destruction;
            default: return false;
            }
        });
    }

    bool Finish()
    {
        Game->SetActorTickEnabled(true);
        Pawn->SetActorTickEnabled(true);
        return true;
    }

    void CheckMeshCopies(int32 Side)
    {
        const USceneComponent* Original = (Side ? Pawn->EnemyRoot : Pawn->ShipRoot).Get();
        const USceneComponent* Shell = Pawn->HullShieldRoots[Side];
        Test.TestTrue(TEXT("One uniform scale at the original ship pivot"),
            Shell->GetRelativeScale3D().Equals(FVector(1.08), 1.e-6) &&
            Shell->GetRelativeLocation().Equals(Original->GetRelativeLocation(), .001) &&
            Shell->GetRelativeRotation().Equals(Original->GetRelativeRotation(), .001));
        if (!Test.TestEqual(TEXT("The shield has one hull skin and no duplicate guns"), Shell->GetNumChildrenComponents(), 1)) return;
        const auto* Skin = Cast<UStaticMeshComponent>(Shell->GetChildComponent(0));
        const auto* Body = Pawn->HullBodies[Side].Get();
        if (!Test.TestTrue(TEXT("The skin uses this ship's exact hull and offset"), Skin &&
            Skin->GetStaticMesh() == Body->GetStaticMesh() && Skin->GetRelativeTransform().Equals(Body->GetRelativeTransform(), .001))) return;
        Test.TestEqual(TEXT("The shield cannot intercept selection or projectiles"), Skin->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
        for (int32 Slot = 0; Slot < Skin->GetNumMaterials(); ++Slot)
            Test.TestTrue(TEXT("Every hull section uses the shield material"), Skin->GetMaterial(Slot) == Pawn->HullShieldMaterials[Side]);
    }

    bool OnSurface(FVector Local,FVector Normal,bool bShield) const
    {
        if (bShield) Local/=1.08;
        const auto& Xf=Pawn->EnemyRoot->GetComponentTransform();
        const FVector Start=Xf.TransformPosition(Local+Normal*2.),End=Xf.TransformPosition(Local-Normal*2.);
        for (USceneComponent* Child : Pawn->EnemyRoot->GetAttachChildren())
        {
            auto* Mesh=Cast<UStaticMeshComponent>(Child);
            FHitResult Hit;
            if (Mesh && (!bShield || Mesh==Pawn->HullBodies[1]) && Mesh->LineTraceComponent(Hit,Start,End,FCollisionQueryParams(NAME_None,true)) &&
                Xf.InverseTransformPosition(Hit.ImpactPoint).Equals(Local,.15)) return true;
        }
        return false;
    }

    void Capture(const FString& Name)
    {
        if (Directory.IsEmpty()) return;
        WaitingPath = Directory / Name;
        RequestedAt = FPlatformTime::Seconds();
        FScreenshotRequest::RequestScreenshot(WaitingPath, true, false);
    }

public:
    FAquilaCombatFeedbackFrames(FAutomationTestBase& InTest, AAquilaGameMode* InGame, AAquilaFlightPawn* InPawn, FString InDirectory)
        : Test(InTest), Game(InGame), Pawn(InPawn), Directory(MoveTemp(InDirectory))
    {
        Game->SetActorTickEnabled(false);
        Pawn->SetActorTickEnabled(false);
        Duel(true);
        Game->bFittingOpen = true;
    }

    bool Update() override
    {
        if (!WaitingPath.IsEmpty())
        {
            if (FScreenshotRequest::IsScreenshotRequested() || IFileManager::Get().FileSize(*WaitingPath) <= 0)
            {
                if (FPlatformTime::Seconds() - RequestedAt < 12) return false;
                Test.AddError(FString::Printf(TEXT("Viewport capture missing: %s"), *WaitingPath));
                return Finish();
            }
            WaitingPath.Reset();
        }
        if (ShapeChangedAt > 0)
        {
            Pawn->Tick(0); // A paused duel still presents its camera, components and material parameters every frame.
            const double Elapsed = FPlatformTime::Seconds() - ShapeChangedAt;
            if (Elapsed < 1.0 || !ShieldSurfacesReady())
            {
                if (Elapsed < 20) return false;
                Test.AddError(TEXT("Timed out waiting for the shield surface to render."));
                return Finish();
            }
            ShapeChangedAt = 0;
            if (!Pawn->bDuelMeshShield)
            {
                Capture(Shot == EShot::Quiet ? TEXT("oval-rest.png") : TEXT("oval-hit.png"));
                bCapturedOval = true;
                return false;
            }
        }
        const bool bLossStage = Shot == EShot::SentinelLoss || Shot == EShot::ResoluteLoss;
        const int32 Count = Shot <= EShot::Quiet ? 1 : (bLossStage ? 62 : 24);
        if (Frame == Count)
        {
            if (bLossStage)
                Test.TestFalse(TEXT("The explosion fades out once"), Pawn->Bursts.ContainsByPredicate(
                    [](const FAquilaVisualBurst& B) { return B.Kind == EAquilaBurst::Destruction; }));
            Shot = EShot(int32(Shot) + 1);
            if (Shot == EShot::Quiet) Game->bFittingOpen = false;
            if (Shot == EShot::Armor) Duel(false);
            Frame = SeekSteps = 0;
            bFound = false;
            bCapturedOval = false;
        }
        if (Shot == EShot::Done)
        {
            Duel(false);
            Test.TestTrue(TEXT("Replay clears contacts and restores both powered shells"),
                Pawn->Bursts.IsEmpty() && Pawn->ShipRoot->IsVisible() && Pawn->EnemyRoot->IsVisible() &&
                Pawn->HullShieldRoots[0]->IsVisible() && Pawn->HullShieldRoots[1]->IsVisible());
            auto Fit = Game->Campaign.Loadout;
            Fit.Resolute.bShield = false;
            Game->StartDuel(Fit);
            Pawn->Tick(0);
            Test.TestTrue(TEXT("An empty shield slot hides only its own bubble"),
                !Pawn->HullShieldRoots[0]->IsVisible() && Pawn->HullShieldRoots[1]->IsVisible());
            return Finish();
        }
        if (Shot == EShot::Fitting) bFound = true;
        if (!bFound && Shot == EShot::Quiet)
        {
            if (!Test.TestTrue(TEXT("Both shield renderers and combat effects are initialized"),
                Pawn->BurstSprites && Pawn->ShieldShells[0] && Pawn->ShieldShells[1] &&
                Pawn->HullShieldRoots[0] && Pawn->HullShieldRoots[1])) return Finish();
            Test.TestTrue(TEXT("Both quiet ships show a fitted shell without an impact flare"), Pawn->Bursts.IsEmpty() &&
                Pawn->BurstSprites->GetInstanceCount() == 0 && Pawn->HullShieldRoots[0]->IsVisible() && Pawn->HullShieldRoots[1]->IsVisible());
            CheckMeshCopies(0);
            CheckMeshCopies(1);
            for (const auto& Shell : Pawn->ShieldEnvelopes)
                Test.TestTrue(TEXT("The two long, low hulls get long, low shields"), Shell.Radii.X > 2 * Shell.Radii.Y && Shell.Radii.Y > Shell.Radii.Z);
            bFound = true;
        }
        if (!bFound)
        {
            // Skip unrecorded frames by advancing the same two actors that run in normal play.
            for (int32 I = 0; I < 30 && !bFound; ++I)
            {
                Step();
                if (++SeekSteps > 5400) { Test.AddError(TEXT("Expected combat effect did not appear.")); return Finish(); }
                const auto* B = Current();
                if (!B) continue;
                bFound = true;
                if (Shot == EShot::Shield)
                {
                    const auto& Shell = Pawn->ShieldEnvelopes[1];
                    Test.TestTrue(TEXT("The same strike has a distinct anchor on each shield"),
                        FMath::IsNearlyEqual(((B->Point - Shell.Center) / Shell.Radii).Size(), 1., 0.0001) &&
                        !B->MeshPoint.Equals(B->Point,1) && OnSurface(B->MeshPoint,B->MeshNormal,true));
                }
                if (Shot == EShot::Hull || Shot == EShot::Armor)
                    Test.TestTrue(TEXT("Warm contact lies on a cooked surface"),OnSurface(B->Point,B->Normal,false));
                if (Shot == EShot::SentinelLoss || Shot == EShot::ResoluteLoss)
                {
                    const bool bEnemy = Shot == EShot::SentinelLoss;
                    Test.TestFalse(TEXT("The destroyed ship disappears at detonation"), (bEnemy ? Pawn->EnemyRoot : Pawn->ShipRoot)->IsVisible());
                    Test.TestFalse(TEXT("A wreck keeps no mesh shield"), Pawn->HullShieldRoots[int32(bEnemy)]->IsVisible());
                    Game->bPaused = true;
                    Pawn->RenderCampaign(0.5f);
                    Test.TestTrue(TEXT("Pause holds the new explosion at its starting frame"), Current() != nullptr);
                    Game->bPaused = false;
                }
                Test.AddInfo(FString::Printf(TEXT("Contact stage %d at %.2f simulated seconds."), int32(Shot), Game->Campaign.BattleTime));
            }
            if (!bFound) return false;
        }
        else if (Frame > 0)
        {
            Step();
            Step();
        }
        const bool bDeath = Shot == EShot::SentinelLoss || Shot == EShot::ResoluteLoss;
        if (bDeath && Frame == 24)
            Test.TestTrue(TEXT("The detonation is visible one real second into an 8x duel"), Pawn->Bursts.ContainsByPredicate(
                [](const FAquilaVisualBurst& B) { return B.Kind == EAquilaBurst::Destruction && FMath::IsNearlyEqual(B.Age, 1.f, 0.001f); }));
        if (Frame == 0 && (Shot == EShot::Quiet || Shot == EShot::Shield))
        {
            // Render the alternate from the same camera at the same instant, including the age of a live strike.
            if (!bCapturedOval)
            {
                Pawn->SetDuelMeshShield(false);
                Test.TestTrue(TEXT("Switching at this instant displays only the oval"),
                    Pawn->ShieldShells[1]->IsVisible() && Pawn->ShieldMaterials[1]->K2_GetScalarParameterValue(TEXT("ShellStrength")) == 1 &&
                    !Pawn->HullShieldRoots[1]->IsVisible());
                ShapeChangedAt = FPlatformTime::Seconds();
                return false;
            }
            if (!Pawn->bDuelMeshShield)
            {
                Pawn->SetDuelMeshShield(true);
                ShapeChangedAt = FPlatformTime::Seconds();
                return false;
            }
            Test.TestTrue(TEXT("Switching back restores the mesh at the same instant"),
                Pawn->ShieldMaterials[1]->K2_GetScalarParameterValue(TEXT("ShellStrength")) == 0 && Pawn->HullShieldRoots[1]->IsVisible() &&
                (Shot != EShot::Shield || Current() != nullptr));
        }
        const TCHAR* Names[] = {TEXT("fitting"), TEXT("quiet"), TEXT("shield"), TEXT("hull"), TEXT("sentinel"), TEXT("armor"), TEXT("resolute")};
        Capture(FString::Printf(TEXT("%s-%03d.png"), Names[int32(Shot)], Frame));
        ++Frame;
        return false;
    }
};

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FAquilaCombatFeedbackTest, "Aquila.Input.CombatFeedback",
    EAutomationTestFlags::ClientContext | EAutomationTestFlags::NonNullRHI | EAutomationTestFlags::EngineFilter)
void FAquilaCombatFeedbackTest::GetTests(TArray<FString>& Names, TArray<FString>& Commands) const
{
    FString Mode, Isolated;
    const TCHAR* Args = FCommandLine::Get();
    if (FParse::Param(Args, TEXT("AquilaDuelTest")) && FParse::Value(Args, TEXT("AquilaMode="), Mode) &&
        Mode.Equals(TEXT("Simulator"), ESearchCase::IgnoreCase) && FParse::Value(Args, TEXT("AquilaSaveDir="), Isolated))
    {
        Names.Add(TEXT("BothShips"));
        Commands.Add(TEXT("BothShips"));
    }
}
bool FAquilaCombatFeedbackTest::RunTest(const FString&)
{
    auto* World = GEngine->GameViewport ? GEngine->GameViewport->GetWorld() : nullptr;
    auto* Game = World ? World->GetAuthGameMode<AAquilaGameMode>() : nullptr;
    auto* PC = World ? World->GetFirstPlayerController() : nullptr;
    auto* Pawn = PC ? Cast<AAquilaFlightPawn>(PC->GetPawn()) : nullptr;
    if (!TestTrue(TEXT("Packaged simulator viewport"), Game && Pawn && Game->Session() == EAquilaSession::Simulator)) return false;
    FString Directory;
    if (FParse::Value(FCommandLine::Get(), TEXT("AquilaFeedbackFrames="), Directory))
        IFileManager::Get().MakeDirectory(*Directory, true);
    ADD_LATENT_AUTOMATION_COMMAND(FAquilaCombatFeedbackFrames(*this, Game, Pawn, Directory));
    return true;
}
#endif
