#include "AquilaFlightPawn.h"
#include "AquilaSimulation.h"
#include "Camera/CameraComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
constexpr float ShieldSeconds = 0.68f;
constexpr float ArmorSeconds = 0.44f;
constexpr float HullSeconds = 0.78f;
constexpr float DestructionSeconds = 2.45f;
constexpr int32 MaxBursts = 80;

float Lifetime(EAquilaBurst Kind)
{
    switch (Kind)
    {
    case EAquilaBurst::Shield: return ShieldSeconds;
    case EAquilaBurst::Armor: return ArmorSeconds;
    case EAquilaBurst::Hull: return HullSeconds;
    default: return DestructionSeconds;
    }
}

// First surface on the complete incoming line. The reference point may already be inside.
bool SphereContact(FVector Point, FVector Direction, double Radius, FVector& Contact)
{
    const FVector Nearest = Point - Direction * FVector::DotProduct(Point, Direction);
    const double Chord = Radius * Radius - Nearest.SizeSquared();
    if (Chord < 0) return false;
    Contact = Nearest - Direction * FMath::Sqrt(Chord);
    return true;
}
}

FAquilaShieldEnvelope FAquilaShieldEnvelope::Fit(TConstArrayView<FBox> Parts)
{
    FAquilaShieldEnvelope Shell;
    if (Parts.IsEmpty()) return Shell;
    FBox Bounds(ForceInit);
    for (const FBox& Part : Parts) Bounds += Part;
    Shell.Center = Bounds.GetCenter();
    Shell.Radii = Bounds.GetExtent() + FVector(48);
    // Scale the oval just enough to enclose each part's furthest corner, including off-center wings and turrets.
    double Expansion = 1;
    for (const FBox& Part : Parts)
    {
        const FVector Corner = (Part.GetCenter() - Shell.Center).GetAbs() + Part.GetExtent();
        Expansion = FMath::Max(Expansion, (Corner / Shell.Radii).Size());
    }
    Shell.Radii *= Expansion * 1.08;
    return Shell;
}

FVector FAquilaShieldEnvelope::Contact(FVector Point, FVector Direction, FVector& Normal) const
{
    const FVector UnitPoint = (Point - Center) / Radii;
    const FVector UnitDirection = (Direction / Radii).GetSafeNormal();
    FVector Surface;
    if (!SphereContact(UnitPoint, UnitDirection, 1, Surface))
    {
        // A grazing hit on the simulation sphere can pass above a flat ship. Anchor it at the nearest silhouette.
        Surface = (UnitPoint - UnitDirection * FVector::DotProduct(UnitPoint, UnitDirection)).GetSafeNormal();
    }
    Normal = (Surface / Radii).GetSafeNormal();
    return Center + Radii * Surface;
}

void AAquilaFlightPawn::BuildCombatVisuals()
{
    auto* Shell = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Foundation/M_ShieldContact.M_ShieldContact"));
    auto* Flare = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Foundation/M_CombatBurst.M_CombatBurst"));
    if (!ensureMsgf(Shell && Flare, TEXT("Prepare the Aquila combat materials before packaging."))) return;
    for (int32 I = 0; I < 2; ++I)
    {
        ShieldMaterials[I] = UMaterialInstanceDynamic::Create(Shell, this);
        auto* Mesh = NewObject<UStaticMeshComponent>(this, I ? TEXT("SentinelShield") : TEXT("ResoluteShield"));
        Mesh->SetupAttachment(RootComponent);
        Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")));
        Mesh->SetMaterial(0, ShieldMaterials[I]);
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Mesh->SetCastShadow(false);
        ShieldEnvelopes[I] = FAquilaShieldEnvelope::Fit(HullParts[I]);
        Mesh->SetRelativeScale3D(ShieldEnvelopes[I].Radii / 50.);
        const FVector Axes = ShieldEnvelopes[I].Radii / ShieldEnvelopes[I].Radii.GetMax();
        ShieldMaterials[I]->SetVectorParameterValue(TEXT("Axes"), FLinearColor(Axes.X, Axes.Y, Axes.Z, 1));
        Mesh->SetVisibility(false);
        Mesh->RegisterComponent();
        ShieldShells[I] = Mesh;
    }
    HotFragments = Instances(Materials[5], TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    BurstSprites = Instances(Flare, TEXT("/Engine/BasicShapes/Plane.Plane"));
    BurstSprites->SetNumCustomDataFloats(3);
}

void AAquilaFlightPawn::BuildHullShields()
{
    auto* Material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Foundation/M_HullShield.M_HullShield"));
    if (!ensureMsgf(Material, TEXT("Prepare the Aquila hull shield material before packaging."))) return;
    for (int32 I = 0; I < 2; ++I)
    {
        if (HullShieldRoots[I]) continue;
        auto* Shell = NewObject<USceneComponent>(this);
        Shell->SetupAttachment(RootComponent);
        Shell->SetRelativeScale3D(FVector(HullShieldScale));
        Shell->RegisterComponent();
        HullShieldRoots[I] = Shell;
        auto* Surface = UMaterialInstanceDynamic::Create(Material, this);
        HullShieldMaterials[I] = Surface;
        Surface->SetScalarParameterValue(TEXT("Reach"), ShieldEnvelopes[I].Radii.GetMax());

        // Expanding a gun about the ship's pivot would leave a second gun hovering away from its mount.
        auto* Skin = NewObject<UStaticMeshComponent>(this);
        Skin->SetupAttachment(Shell);
        Skin->SetStaticMesh(HullBodies[I]->GetStaticMesh());
        Skin->SetRelativeTransform(HullBodies[I]->GetRelativeTransform());
        for (int32 Slot = 0; Slot < Skin->GetNumMaterials(); ++Slot) Skin->SetMaterial(Slot, Surface);
        Skin->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Skin->SetCastShadow(false);
        Skin->RegisterComponent();
        Shell->SetVisibility(false, true);
    }
}

void AAquilaFlightPawn::SetDuelMeshShield(bool bMesh)
{
    bDuelMeshShield = bMesh;
    RenderCampaign(0);
}

FVector AAquilaFlightPawn::HullContact(int32 Side,FVector RayPoint,FVector Direction,FVector& Normal,bool bShield,bool bHullOnly) const
{
    const auto* Root=(Side ? EnemyRoot : ShipRoot).Get();
    const auto& WorldFromShip=Root->GetComponentTransform();
    const double Scale=bShield ? HullShieldScale : 1.;
    const FVector Middle=(RayPoint-Direction*RayPoint.Dot(Direction))/Scale;
    const FCollisionQueryParams Query(SCENE_QUERY_STAT(AquilaHullContact),true);
    // A grazing simulation hit can cross an open arch. Search parallel rays toward the body for the nearest visible contact.
    for (double Offset : {1.,.75,.5,.25,0.})
    {
        const FVector Start=WorldFromShip.TransformPosition(Middle*Offset-Direction*1600.);
        const FVector End=WorldFromShip.TransformPosition(Middle*Offset+Direction*1600.);
        FHitResult First;
        for (USceneComponent* Child : Root->GetAttachChildren())
        {
            auto* Part=Cast<UStaticMeshComponent>(Child);
            if (!Part || Part==PortUpgradeGlow || ((bHullOnly || bShield) && Part!=HullBodies[Side]) ||
                !FMath::LineBoxIntersection(Part->Bounds.GetBox(),Start,End,End-Start)) continue;
            FHitResult Hit;
            if (Part->LineTraceComponent(Hit,Start,End,Query) && Hit.Time<First.Time) First=Hit;
        }
        if (First.Time<1.f)
        {
            Normal=WorldFromShip.InverseTransformVectorNoScale(First.ImpactNormal).GetSafeNormal();
            return WorldFromShip.InverseTransformPosition(First.ImpactPoint)*Scale;
        }
    }
    // Every socket is measured on the hull. An end-on ray through an opening may miss even at the pivot.
    const auto& Layout=AquilaHulls::Layout(Side!=0);
    const FTransform* Nearest=&Layout.Shield;
    auto Distance=[&](const FTransform& Seat) {
        return FVector::CrossProduct(Seat.GetLocation()-Middle,Direction).SizeSquared();
    };
    for (const FTransform& Seat : Layout.Guns) if (Distance(Seat)<Distance(*Nearest)) Nearest=&Seat;
    Normal=Nearest->GetUnitAxis(EAxis::Z);
    return Nearest->GetLocation()*Scale;
}

FVector AAquilaFlightPawn::RenderRoundPosition(const FAquilaCampaign& C, const FAquilaRound& R, const FVector& Origin, bool bMeshShield) const
{
    const FVector Center = R.bEnemy ? C.ShipPosition : C.EnemyPosition;
    const FVector Raw = (R.Position - Origin) * ExteriorScale;
    const FVector Offset = (R.Position - Center) * ExteriorScale;
    if (Offset.SizeSquared() > FMath::Square(1600.) || (R.bEnemy ? C.Hull : C.EnemyHull) <= 0) return Raw;
    const FRotator Facing(0, R.bEnemy ? FMath::RadiansToDegrees(C.Heading) : 180., 0);
    const FVector Point = Facing.UnrotateVector(Offset);
    const FVector Direction = Facing.UnrotateVector(R.Velocity.GetSafeNormal());
    const bool bShield = R.HitsShield(R.bEnemy ? C.Shield : C.EnemyShield);
    const int32 TargetIndex = R.bEnemy ? 0 : 1;
    FVector Entry, Surface, Normal;
    if (!SphereContact(Point, Direction, (bShield ? C.ShieldRadius : C.HullRadius) * ExteriorScale, Entry)) return Raw;
    const double Remaining = FVector::DotProduct(Entry - Point, Direction);
    if (Remaining < 0 || Remaining >= 1200) return Raw;
    if (bShield && !bMeshShield)
        Surface = ShieldEnvelopes[TargetIndex].Contact(Point, Direction, Normal);
    else
        Surface = HullContact(TargetIndex,Point,Direction,Normal,bShield);
    // The trail reaches the visible plating or bubble on the same tick the model resolves damage.
    return Raw + Facing.RotateVector(Surface - Entry) * (1 - Remaining / 1200.);
}

void AAquilaFlightPawn::RenderShieldShells(const FAquilaCampaign& C, const FVector* Centers, const FQuat* Rotations,
    bool bMeshShield, bool bFitting)
{
    for (int32 I = 0; I < 2; ++I)
    {
        const auto& Envelope = ShieldEnvelopes[I];
        FVector Waves[3] = {FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector};
        FVector MeshHits[3] = {FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector};
        FVector Ages(1);
        int32 Count = 0;
        float LastAge = -1;
        for (int32 K = bFitting ? INDEX_NONE : Bursts.Num() - 1; K >= 0 && Count < 3; --K)
        {
            const auto& B = Bursts[K];
            if (B.Kind != EAquilaBurst::Shield || int32(B.bEnemy) != I) continue;
            const FVector OnSphere = (B.Point - Envelope.Center) / Envelope.Radii;
            const FVector MeshHit = bMeshShield ? RootComponent->GetComponentTransform().TransformPosition(
                Centers[I] + Rotations[I].RotateVector(B.MeshPoint)) : FVector::ZeroVector;
            const bool bNearby = Count > 0 && (bMeshShield ? FVector::DistSquared(MeshHit, MeshHits[Count-1]) < FMath::Square(35.*GetActorScale3D().X) :
                FVector::DotProduct(Waves[Count-1].GetSafeNormal(), OnSphere) > 0.98);
            if (bNearby && B.Age - LastAge < 0.10f) continue;
            LastAge = B.Age;
            if (bMeshShield)
            {
                MeshHits[Count] = MeshHit;
                Ages[Count] = B.Age / ShieldSeconds;
            }
            Waves[Count++] = OnSphere * (2 - B.Age / ShieldSeconds);
        }
        auto* Shell = ShieldShells[I].Get();
        Shell->SetRelativeLocationAndRotation(Centers[I] + Rotations[I].RotateVector(Envelope.Center), Rotations[I]);
        const bool bShipVisible = (I ? EnemyRoot : ShipRoot)->IsVisible();
        const bool bPowered = (I ? C.EnemyShield : C.Shield) > 0;
        const bool bVisible = bShipVisible && C.Loadout.ForShip(I != 0).bShield &&
            (bFitting ? SelectedPart == ShieldPart : bPowered || Count > 0);
        // Keep a powered oval ready at zero opacity so a paused switch restores its rim and current hit together.
        Shell->SetVisibility(bVisible);
        ShieldMaterials[I]->SetScalarParameterValue(TEXT("ShellStrength"), bMeshShield ? 0.f : 1.f);
        if (auto* MeshShell = HullShieldRoots[I].Get())
        {
            const bool bShowMesh = bVisible && bMeshShield;
            if (MeshShell->IsVisible() != bShowMesh) MeshShell->SetVisibility(bShowMesh, true);
            if (bShowMesh) MeshShell->SetRelativeLocationAndRotation(Centers[I], Rotations[I]);
        }
        if (!bVisible) continue;
        const FVector EyeLocal = Shell->GetComponentQuat().UnrotateVector(Camera->GetComponentLocation() - Shell->GetComponentLocation())
            / (Envelope.Radii.GetMax()*GetActorScale3D().X);
        ShieldMaterials[I]->SetVectorParameterValue(TEXT("Eye"), FLinearColor(EyeLocal.X, EyeLocal.Y, EyeLocal.Z, 1));
        if (bMeshShield)
        {
            HullShieldMaterials[I]->SetScalarParameterValue(TEXT("Reach"), Envelope.Radii.GetMax()*GetActorScale3D().X);
            HullShieldMaterials[I]->SetVectorParameterValue(TEXT("Ages"), FLinearColor(Ages.X, Ages.Y, Ages.Z, 1));
        }
        static const FName Names[] = {TEXT("HitA"), TEXT("HitB"), TEXT("HitC")};
        for (int32 K = 0; K < 3; ++K)
        {
            const FVector& Wave = Waves[K];
            ShieldMaterials[I]->SetVectorParameterValue(Names[K], FLinearColor(Wave.X, Wave.Y, Wave.Z, 1));
            if (bMeshShield)
            {
                const FVector& Hit = MeshHits[K];
                HullShieldMaterials[I]->SetVectorParameterValue(Names[K], FLinearColor(Hit.X, Hit.Y, Hit.Z, 1));
            }
        }
    }
}

void AAquilaFlightPawn::RenderCombatVisuals(const FAquilaCampaign& C, const FVector& Origin, float Seconds, bool bMeshShield)
{
    if (!BurstSprites) return;
    if (SeenEncounter != C.EncounterGeneration || VisualCampaignId != C.CampaignId)
    {
        Bursts.Reset();
        SeenImpact = 0;
        SeenEncounter = C.EncounterGeneration;
        VisualCampaignId = C.CampaignId;
    }
    for (auto& Burst : Bursts) Burst.Age += Seconds;
    Bursts.RemoveAll([](const FAquilaVisualBurst& B) { return B.Age >= Lifetime(B.Kind); });
    for (const FAquilaImpact& Hit : C.Impacts)
    {
        if (Hit.Serial <= SeenImpact) continue;
        SeenImpact = Hit.Serial;
        auto Add = [&](EAquilaBurst Kind, FVector Point, FVector Normal) -> FAquilaVisualBurst&
        {
            if (Bursts.Num() >= MaxBursts)
            {
                const int32 Oldest = Bursts.IndexOfByPredicate([](const FAquilaVisualBurst& B) { return B.Kind != EAquilaBurst::Destruction; });
                if (Oldest != INDEX_NONE) Bursts.RemoveAt(Oldest, 1, EAllowShrinking::No);
            }
            return Bursts.Add_GetRef({Kind, Hit.bTargetEnemy, Point, Normal, Hit.Serial});
        };
        const FVector Contact = Hit.LocalContact * ExteriorScale;
        const int32 TargetIndex = Hit.bTargetEnemy ? 1 : 0;
        if (Hit.bShield)
        {
            FVector Normal;
            const FVector Surface = ShieldEnvelopes[TargetIndex].Contact(Contact, Hit.LocalDirection, Normal);
            auto& Burst = Add(EAquilaBurst::Shield, Surface, Normal);
            if (HullShieldRoots[TargetIndex])
                Burst.MeshPoint = HullContact(TargetIndex,Contact,Hit.LocalDirection,Burst.MeshNormal,true);
        }
        if (Hit.bBlocked || Hit.bHull)
        {
            FVector Normal;
            const FVector Plating = HullContact(TargetIndex,Contact,Hit.LocalDirection,Normal);
            Add(Hit.bBlocked ? EAquilaBurst::Armor : EAquilaBurst::Hull, Plating, Normal);
        }
        if (Hit.bDestroyed) Add(EAquilaBurst::Destruction, FVector::ZeroVector, FVector::UpVector);
    }

    const FVector Centers[] = {(C.ShipPosition - Origin) * ExteriorScale, (C.EnemyPosition - Origin) * ExteriorScale};
    const FQuat Rotations[] = {ShipRoot->GetRelativeRotation().Quaternion(), EnemyRoot->GetRelativeRotation().Quaternion()};
    RenderShieldShells(C, Centers, Rotations, bMeshShield);

    BurstSprites->ClearInstances();
    HotFragments->ClearInstances();
    const FQuat Billboard = RootComponent->GetComponentQuat().Inverse() *
        FRotationMatrix::MakeFromXY(Camera->GetRightVector(), Camera->GetUpVector()).ToQuat();
    auto Flare = [&](FVector Position, double Diameter, float Phase, EAquilaBurst Kind, float Seed, bool bSecondary = false)
    {
        const int32 Index = BurstSprites->AddInstance(FTransform(Billboard, Position, FVector(Diameter / 100.)));
        const float Data[] = {Phase, float(Kind) + (bSecondary ? 1.f : 0.f), Seed};
        BurstSprites->SetCustomData(Index, MakeArrayView(Data));
    };
    for (const auto& B : Bursts)
    {
        const int32 I = int32(B.bEnemy);
        const bool bMeshHit = bMeshShield && B.Kind == EAquilaBurst::Shield;
        const FVector Normal = Rotations[I].RotateVector(bMeshHit ? B.MeshNormal : B.Normal);
        const FVector P = Centers[I] + Rotations[I].RotateVector(bMeshHit ? B.MeshPoint : B.Point);
        const float Phase = B.Age / Lifetime(B.Kind);
        const bool bDeath = B.Kind == EAquilaBurst::Destruction;
        FRandomStream Random(int32(B.Serial * 7919u + uint32(B.Kind) * 397u));
        const float Seed = Random.FRandRange(0, 24);
        if (bDeath)
        {
            // Secondary detonations open along the keel, inside one larger expanding shock front.
            Flare(P, 3700., Phase, B.Kind, Seed);
            for (int32 K = 0; K < 5; ++K)
            {
                const float Delay = 0.035f + K * 0.047f;
                const FVector Offset(-530 + K * 260, Random.FRandRange(-135,135), Random.FRandRange(-50,90));
                const float Size = Random.FRandRange(750, 1320);
                if (Phase > Delay)
                    Flare(P + Rotations[I].RotateVector(Offset), Size, (Phase - Delay) / (1 - Delay), B.Kind, Seed + K + 1, true);
            }
        }
        else
        {
            const double Size = B.Kind == EAquilaBurst::Hull ? 330. : (B.Kind == EAquilaBurst::Armor ? 205. : 225.);
            Flare(P + Normal * 6, Size, Phase, B.Kind, Seed);
        }
        if (B.Kind == EAquilaBurst::Shield) continue;
        const int32 Pieces = bDeath ? 36 : (B.Kind == EAquilaBurst::Hull ? 9 : 5);
        for (int32 K = 0; K < Pieces; ++K)
        {
            const FVector Dir = bDeath ? Random.VRand() : (Normal * 1.25 + Random.VRand()).GetSafeNormal();
            const double Speed = Random.FRandRange(0.48f, 1.f) * (bDeath ? 1150 : 420);
            const double Span = (bDeath ? 90. : 36.) * (1 - Phase) * Random.FRandRange(0.5f, 1.f);
            const FVector Travel = Dir * (Speed * B.Age * (1 - 0.35 * Phase));
            HotFragments->AddInstance(FTransform(Dir.Rotation(), P + Travel, FVector(Span, Span * 0.12, Span * 0.12) / 100.));
        }
    }
}
