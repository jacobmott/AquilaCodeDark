#include "AquilaFlightPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
constexpr const TCHAR* Sphere = TEXT("/Engine/BasicShapes/Sphere.Sphere");
const FVector TargetAxes(900,460,350);

FLinearColor TracerColor(const FAquilaTurret& Gun)
{
    if (Gun.Weapon == EAquilaWeapon::PlasmaCaster) return FLinearColor(.84f,.19f,1.f);
    if (Gun.Weapon == EAquilaWeapon::PulseRepeater) return FLinearColor(.13f,1.f,.56f);
    return AquilaAmmunition::Models[int32(Gun.Ammo)].Tracer;
}
}

const FAquilaTurret* AAquilaFlightPawn::FittedTurret() const
{
    auto* Fit = EditingShip();
    if (!Fit || !Fit->Rack || SelectedPart < 0) return nullptr;
    const int32 I = Fit->Rack->At(SelectedPart);
    return I == INDEX_NONE ? nullptr : &Fit->Rack->Stock[I];
}

TOptional<EAquilaWeapon> AAquilaFlightPawn::AmmoFittingWeapon() const
{
    if (SelectedPart < 0 || !EditingShip()) return {};
    auto Weapon = PlacementWeapon;
    if (!Weapon)
        if (const auto* Gun = FittedTurret()) Weapon = Gun->Weapon;
    return Weapon && !FAquilaTurret::IsPlasma(Weapon.GetValue()) ? Weapon : TOptional<EAquilaWeapon>();
}

TOptional<EAquilaWeapon> AAquilaFlightPawn::InspectedWeapon() const
{
    if (SelectedPart >= 0 && EditingShip())
    {
        if (HoveredWeapon) return HoveredWeapon;
        if (PlacementWeapon) return PlacementWeapon;
        if (bInspectPort)
            if (const auto* Gun = FittedTurret()) return Gun->Weapon;
    }
    return {};
}

FAquilaTurret AAquilaFlightPawn::SampleGun(EAquilaWeapon Weapon) const
{
    const FAquilaTurret* Source = FittedTurret();
    const auto* Fit = EditingShip();
    if (!Source && Fit && Fit->Rack)
    {
        const int32 Spare = Fit->Rack->At(INDEX_NONE);
        if (Spare != INDEX_NONE) Source = &Fit->Rack->Stock[Spare];
    }
    FAquilaTurret Gun = Source ? *Source : FAquilaTurret();
    Gun.Weapon = Weapon;
    if (PlacementWeapon == Weapon && PlacementAmmo) Gun.Ammo = PlacementAmmo.GetValue();
    if (FAquilaTurret::IsPlasma(Weapon)) Gun.Ammo = EAquilaAmmo::Standard;
    return Gun;
}

void AAquilaFlightPawn::ClearFiringSelection()
{
    HoveredWeapon.Reset();
    PlacementWeapon.Reset();
    PlacementAmmo.Reset();
    bInspectPort = false;
}

void AAquilaFlightPawn::UpdateFiringSelection(bool bFitting, float Seconds)
{
    const auto Weapon = bFitting ? InspectedWeapon() : TOptional<EAquilaWeapon>();
    if (!Weapon)
    {
        if (!bFitting) ClearFiringSelection();
        if (FiringPreview)
        {
            FiringPreview.Reset();
            if (PreviewTarget) PreviewTarget->SetVisibility(false, true);
            for (auto* Batch : {PreviewTracers.Get(), PreviewSparks.Get(), PreviewDeflections.Get(), PreviewCounters.Get()})
                if (Batch) Batch->ClearInstances();
        }
        return;
    }
    const FAquilaTurret Gun = SampleGun(Weapon.GetValue());
    if (!FiringPreview || FiringPreview->Gun.Weapon != Gun.Weapon || FiringPreview->Gun.Ammo != Gun.Ammo ||
        FiringPreview->Gun.bIntercept != Gun.bIntercept)
    {
        FiringPreview.Emplace(Gun);
        if (!PreviewTarget) BuildPreviewTarget();
        PreviewTarget->SetVisibility(true, true);
        PreviewShotMaterial->SetVectorParameterValue(TEXT("Color"), TracerColor(Gun));
    }
    FiringPreview->Advance(Seconds);
}

void AAquilaFlightPawn::BuildPreviewTarget()
{
    PreviewTarget = NewObject<USceneComponent>(this);
    PreviewTarget->SetupAttachment(Camera);
    PreviewTarget->RegisterComponent();
    BuildShip(PreviewTarget,true)->SetMaterialByName(TEXT("Armor"),ArmorFinish(true,EAquilaArmor::Reinforced));
    PreviewShield = Shape(PreviewTarget, Sphere, FVector::ZeroVector, TargetAxes*2., 0);
    PreviewShieldMaterial = UMaterialInstanceDynamic::Create(ShieldMaterials[0]->Parent, this);
    PreviewShield->SetMaterial(0, PreviewShieldMaterial);
    const FVector Axes = TargetAxes/TargetAxes.GetMax();
    PreviewShieldMaterial->SetVectorParameterValue(TEXT("Axes"), FLinearColor(Axes.X,Axes.Y,Axes.Z));
    PreviewShieldMaterial->SetScalarParameterValue(TEXT("IdleGain"), 7.f);
    PreviewShotMaterial = UMaterialInstanceDynamic::Create(TurretLights[0]->Parent, this);
    PreviewShotMaterial->SetScalarParameterValue(TEXT("Glow"), 2.8f);
    auto Marks = [&](UMaterialInterface* Material) {
        auto* Batch = Instances(Material,Sphere);
        Batch->AttachToComponent(Camera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        return Batch;
    };
    PreviewTracers = Marks(PreviewShotMaterial);
    PreviewSparks = Marks(Materials[5]);
    PreviewDeflections = Marks(Materials[3]);
    PreviewCounters = Marks(Materials[2]);
}

void AAquilaFlightPawn::RenderFiringPreview(FVector Target, double TargetSpan)
{
    const auto& Demo = FiringPreview.GetValue();
    const auto& Sim = Demo.Range;
    const auto& Gun = Demo.Gun;
    const FQuat Facing = FRotationMatrix::MakeFromXZ(FVector(.12,1,.12),FVector(-1,0,.42)).ToQuat();
    const double Scale = TargetSpan / 1360.;
    PreviewTarget->SetRelativeTransform(FTransform(Facing,Target,FVector(Scale)));
    const FVector EyeLocal = Facing.UnrotateVector(-Target) / (TargetAxes.GetMax()*Scale);
    PreviewShieldMaterial->SetVectorParameterValue(TEXT("Eye"), FLinearColor(EyeLocal.X,EyeLocal.Y,EyeLocal.Z));
    for (auto* Batch : {PreviewTracers.Get(), PreviewSparks.Get(), PreviewDeflections.Get(), PreviewCounters.Get()}) Batch->ClearInstances();
    auto* Model = TurretPreviews[int32(Gun.Weapon)].Get();
    const FTransform Mount = Model->GetRelativeTransform();
    const FVector Muzzle = Mount.TransformPosition(FAquilaTurret::BarrelTip(Gun.Weapon,1));
    const FVector Inbound = (Target-Muzzle).GetSafeNormal();
    const FVector LocalRay = Facing.UnrotateVector(-Inbound);
    const FVector ShellSurface = LocalRay / (LocalRay/TargetAxes).Size();
    const FVector ShellHit = Target + Facing.RotateVector(ShellSurface*Scale);
    FVector Normal;
    const FVector PlateSurface = HullContact(1,FVector::ZeroVector,-LocalRay,Normal,false,true);
    const FVector HullHit = Target + Facing.RotateVector(PlateSurface*Scale);
    auto Position = [&](FVector PositionInSim, uint64 Shot) {
        const double ToTarget = Sim.EnemyPosition.X - PositionInSim.X;
        const FVector Barrel = Mount.TransformPosition(FAquilaTurret::BarrelTip(Gun.Weapon,Shot));
        if (ToTarget >= Sim.ShieldRadius)
            return FMath::Lerp(Barrel,ShellHit,FMath::Clamp((Demo.Distance-ToTarget)/(Demo.Distance-Sim.ShieldRadius),0.,1.));
        return FMath::Lerp(ShellHit,HullHit,FMath::Clamp((Sim.ShieldRadius-ToTarget)/(Sim.ShieldRadius-Sim.HullRadius),0.,1.));
    };
    auto Stroke = [](UInstancedStaticMeshComponent* Batch, FVector A, FVector B, double Width) {
        const FVector Line = B-A;
        Batch->AddInstance(FTransform(Line.Rotation(),(A+B)*.5,FVector(Line.Size(),Width,Width)/100.));
    };
    const double Unit = TargetSpan*.017;
    for (const auto& R : Sim.Rounds)
    {
        const FVector P = Position(R.Position,R.Id);
        if (R.bEnemy) { Stroke(PreviewSparks,P,P+Inbound*(Unit*5),Unit*1.25); continue; }
        const FVector Barrel = Mount.TransformPosition(FAquilaTurret::BarrelTip(Gun.Weapon,R.Id));
        const FVector Direction = (Target-Barrel).GetSafeNormal();
        const bool bOrb = Gun.Weapon == EAquilaWeapon::PlasmaCaster;
        const double Length = Unit * (bOrb ? 4.3 : Gun.Ammo == EAquilaAmmo::Piercing ? 9.5 : 5.6);
        Stroke(PreviewTracers,P-Direction*Length,P,bOrb ? Unit*3.5 : Unit*1.22);
        if (Gun.Ammo == EAquilaAmmo::Phase)
            Stroke(PreviewTracers,P-Direction*(Length+Unit*3.7),P-Direction*(Length+Unit*1.6),Unit*.9);
        if (R.Age < .12) Stroke(PreviewTracers,Barrel,Barrel+Direction*Unit*7.,Unit*(1.+2.5*(1.-R.Age/.12)));
    }
    FVector Wave = FVector::ZeroVector;
    for (const auto& Hit : Sim.Impacts)
    {
        if (!Hit.bTargetEnemy || Hit.Age > .58) continue;
        const double T = Hit.Age/.58;
        if (Hit.bShield) Wave = (ShellSurface/TargetAxes).GetSafeNormal() * (2.-T);
        const FVector Contact = Hit.bShield && !Hit.bHull && !Hit.bBlocked ? ShellHit : HullHit;
        auto* Sparks = Hit.bBlocked ? PreviewDeflections.Get() : Hit.bHull ? PreviewSparks.Get() : PreviewCounters.Get();
        const int32 N = Hit.bBlocked ? 4 : 7;
        for (int32 I=0; I<N; ++I)
        {
            const double A = UE_TWO_PI*I/N + .45*Hit.Serial;
            const FVector Radial(0,FMath::Cos(A),FMath::Sin(A));
            const FVector Ray = Hit.bBlocked ? (-Inbound + Radial*.78).GetSafeNormal() : Radial;
            const FVector P = Contact+Ray*(Unit*(1.+11.*T));
            Stroke(Sparks,P,P+Ray*Unit*(3.5-2.5*T),Unit*(1.15-.85*T));
        }
        if (Hit.bHull)
        {
            Stroke(PreviewSparks,HullHit,HullHit+Inbound*Unit*(5.5-3.*T),Unit*(2.5-1.5*T));
            if (Gun.Ammo == EAquilaAmmo::Piercing && Hit.Age < .3)
            {
                const FVector Tip = HullHit + Inbound*Unit*(2.+Hit.Age*38.);
                Stroke(PreviewTracers,Tip-Inbound*Unit*4.,Tip,Unit*1.5);
            }
        }
    }
    PreviewShield->SetVisibility(Sim.EnemyShield > 0 || Demo.ShieldBreakHold > 0);
    PreviewShieldMaterial->SetScalarParameterValue(TEXT("ShellStrength"),Sim.EnemyShield > 0 ? 1.f : float(Demo.ShieldBreakHold/.42));
    PreviewShieldMaterial->SetVectorParameterValue(TEXT("HitA"),FLinearColor(Wave.X,Wave.Y,Wave.Z));
    for (const auto& Bolt : Sim.DuelStats->ScreenShots)
    {
        const FVector P = Position(Bolt.Position,1);
        Stroke(PreviewCounters,P-Inbound*Unit*7.,P,Unit*1.4);
    }
    for (const auto& Flash : Sim.DuelStats->Flashes)
    {
        const FVector P = Position(Flash.Position,1);
        const double Reach = Unit*5.*(1.-Flash.Age/.8);
        Stroke(PreviewCounters,P-FVector(0,Reach,0),P+FVector(0,Reach,0),Unit);
        Stroke(PreviewCounters,P-FVector(0,0,Reach),P+FVector(0,0,Reach),Unit);
    }
}
