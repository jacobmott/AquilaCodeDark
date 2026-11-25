#include "AquilaFlightPawn.h"
#include "AquilaSimulation.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SceneView.h"

namespace
{
UStaticMesh* LoadFittingAsset(const TCHAR* Name)
{
    const FString Path = FString::Printf(TEXT("/Game/Foundation/%s.%s"), Name, Name);
    if (auto* Mesh = LoadObject<UStaticMesh>(nullptr, *Path)) return Mesh;
    UE_LOG(LogTemp, Error, TEXT("Missing fitting model %s. Prepare and cook the native content."), *Path);
    return LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
}
}

void AAquilaFlightPawn::LoadTurretModels(UMaterialInterface* Glow)
{
    auto* Hardware=AquilaHulls::Surface(false,TEXT("Hardware"));
    auto* Silver=AquilaHulls::Surface(false,TEXT("Silver"));
    TurretPlate = UMaterialInstanceDynamic::Create(Silver, this);
    TurretPlate->SetVectorParameterValue(TEXT("Color"), FLinearColor(.48f,.63f,.79f));
    TurretFrame = UMaterialInstanceDynamic::Create(Hardware, this);
    TurretEdge = UMaterialInstanceDynamic::Create(Silver, this);
    const FLinearColor Accents[] = {{.06f,.69f,1.f},{1.f,.40f,.07f},{.65f,.15f,1.f},{.05f,1.f,.58f}};
    for (const auto& Model : AquilaWeapons::Models)
    {
        const int32 I = int32(Model.Weapon);
        TurretMeshes[I] = LoadFittingAsset(Model.Asset);
        TurretLights[I] = UMaterialInstanceDynamic::Create(Glow, this);
        TurretLights[I]->SetVectorParameterValue(TEXT("Color"), Accents[I]);
        TurretLights[I]->SetScalarParameterValue(TEXT("Glow"), 1.7f);
    }
    struct FPalette { FLinearColor Plate, Edge, Light; };
    const FPalette Finishes[] = {
        {{.50f,.67f,.72f},{.74f,.85f,.90f},{.09f,.72f,.84f}},
        {{.115f,.14f,.17f},{.57f,.27f,.10f},{1.f,.39f,.10f}},
        {{.31f,.12f,.52f},{.58f,.35f,.82f},{.17f,.80f,.82f}},
        {{.12f,.30f,.46f},{.44f,.70f,.79f},{.07f,.66f,1.f}},
        {{.29f,.17f,.09f},{.84f,.56f,.23f},{1.f,.42f,.055f}},
        {{.21f,.27f,.32f},{.45f,.53f,.59f},{.13f,.20f,.24f}}
    };
    auto Finish = [this](UMaterialInterface* Base, FLinearColor Color) {
        auto* M = UMaterialInstanceDynamic::Create(Base, this);
        M->SetVectorParameterValue(TEXT("Color"), Color);
        return M;
    };
    const FLinearColor PlatingTints[]={{1.06f,1.02f,.97f},{.56f,.66f,.78f},{.57f,.27f,.91f},{.42f,.46f,.49f}};
    for (int Side=0; Side<2; ++Side)
    {
        for (int I=0; I<UE_ARRAY_COUNT(PlatingTints); ++I)
        {
            auto* Stock=AquilaHulls::Surface(Side!=0,I==int32(EAquilaArmor::BareHull) ? TEXT("Hardware") : TEXT("Armor"));
            ShipPlating[Side*4+I]=Finish(Stock,PlatingTints[I]);
        }
    }
    for (const auto& Model : AquilaDefenses::Models)
    {
        const int32 I = int32(Model.Type);
        DefenseMeshes[I] = LoadFittingAsset(Model.Asset);
        DefensePlates[I] = Finish(Silver, Finishes[I].Plate);
        DefenseEdges[I] = Finish(Silver, Finishes[I].Edge);
        DefenseLights[I] = Finish(Glow, Finishes[I].Light);
        DefenseLights[I]->SetScalarParameterValue(TEXT("Glow"), 1.45f);
    }
}

UStaticMeshComponent* AAquilaFlightPawn::FittingMesh(USceneComponent* Parent, UStaticMesh* Mesh, UMaterialInterface* Plate,
    UMaterialInterface* Edge, UMaterialInterface* Light, UMaterialInterface* Chip)
{
    auto* Part = NewObject<UStaticMeshComponent>(this);
    Part->SetupAttachment(Parent);
    Part->SetStaticMesh(Mesh);
    Part->SetMaterialByName(TEXT("Plate"), Plate);
    Part->SetMaterialByName(TEXT("Frame"), TurretFrame);
    Part->SetMaterialByName(TEXT("Edge"), Edge);
    Part->SetMaterialByName(TEXT("Emitter"), Light);
    Part->SetMaterialByName(TEXT("Chip"), Chip);
    Part->SetGenerateOverlapEvents(false);
    Part->SetCollisionResponseToAllChannels(ECR_Ignore);
    Part->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
    Part->SetCollisionEnabled(Parent == Camera ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
    Part->SetCastShadow(false);
    Part->RegisterComponent();
    return Part;
}

UStaticMeshComponent* AAquilaFlightPawn::TurretModel(USceneComponent* Parent, EAquilaWeapon Weapon, bool bChip)
{
    return FittingMesh(Parent, TurretMeshes[int32(Weapon)], TurretPlate, TurretEdge, TurretLights[int32(Weapon)], bChip ? Materials[2] : Materials[1]);
}

UMaterialInterface* AAquilaFlightPawn::ArmorFinish(bool bGlacier, EAquilaArmor Armor) const
{
    return ShipPlating[int32(bGlacier)*4+int32(Armor)];
}

UStaticMeshComponent* AAquilaFlightPawn::DefenseModel(USceneComponent* Parent, EAquilaDefense Type, bool bGlacier)
{
    const int32 I = int32(Type);
    auto* Plate=AquilaDefenses::IsPlating(Type) ? ArmorFinish(bGlacier,static_cast<EAquilaArmor>(Type)) : DefensePlates[I].Get();
    return FittingMesh(Parent, DefenseMeshes[I], Plate, DefenseEdges[I], DefenseLights[I], Materials[1]);
}

UStaticMeshComponent* AAquilaFlightPawn::AmmoModel(const AquilaAmmunition::FModel& Style)
{
    auto* Casing = UMaterialInstanceDynamic::Create(TurretPlate->Parent, this);
    Casing->SetVectorParameterValue(TEXT("Color"), Style.Finish);
    auto* Light = UMaterialInstanceDynamic::Create(TurretLights[0]->Parent, this);
    Light->SetVectorParameterValue(TEXT("Color"), Style.Tracer);
    Light->SetScalarParameterValue(TEXT("Glow"), 1.45f);
    return FittingMesh(Camera, LoadFittingAsset(Style.Asset), Casing, TurretEdge, Light, Materials[1]);
}

void AAquilaFlightPawn::FitDefenseModels(int32 Side, const FAquilaShipLoadout& Fit)
{
    auto* Root = (Side ? EnemyRoot : ShipRoot).Get();
    const auto Plate = AquilaDefenses::Plate(Fit.Armor);
    HullBodies[Side]->SetMaterialByName(TEXT("Armor"),ArmorFinish(Side!=0,Fit.Armor));
    const auto& Layout=AquilaHulls::Layout(Side!=0);
    auto Mount = [&](EAquilaDefense Type,const FTransform& Seat) {
        auto* Model = DefenseModel(Root, Type, Side!=0);
        Model->ComponentTags.Add(TEXT("FittedDefense"));
        Model->SetRelativeTransform(FTransform(Seat.GetRotation(),Seat.TransformPosition(FVector(0,0,-1.)),FVector(.30)));
        HullParts[Side].Add(Model->GetStaticMesh()->GetBoundingBox().TransformBy(Model->GetRelativeTransform()));
        return Model;
    };
    if (Plate) Mount(Plate.GetValue(),Layout.Armor);
    MountedReserve[Side] = nullptr;
    if (Fit.bShield)
    {
        Mount(EAquilaDefense::ShieldEmitter,Layout.Shield);
        if (Fit.bReserveCapacitor)
            MountedReserve[Side] = Mount(EAquilaDefense::ReserveCapacitor,Layout.Reserve);
    }
}

FVector2D AAquilaFlightPawn::TurretPreviewUV(EAquilaWeapon Weapon)
{
    const FVector2D Positions[] = {{.17,.225},{.83,.225},{.17,.695},{.83,.695}};
    return Positions[int32(Weapon)];
}

void AAquilaFlightPawn::UpdateTurretPreviews(bool bFitting, float Seconds)
{
    UpdateFiringSelection(bFitting, Seconds);
    if (!bFitting)
    {
        for (auto& Preview : TurretPreviews) if (Preview && Preview->IsVisible()) Preview->SetVisibility(false);
        for (auto& Preview : AmmoPreviews) if (Preview && Preview->IsVisible()) Preview->SetVisibility(false);
        for (auto& Preview : DefensePreviews) if (Preview && Preview->IsVisible()) Preview->SetVisibility(false);
        if (FittingStars && FittingStars->IsVisible()) FittingStars->SetVisibility(false);
        return;
    }
    auto* PC = Cast<APlayerController>(GetController());
    const auto* Player = PC ? PC->GetLocalPlayer() : nullptr;
    FSceneViewProjectionData View;
    if (!Player || !Player->ViewportClient || !Player->GetProjectionData(Player->ViewportClient->Viewport, View)) return;
    const auto Rect = View.GetConstrainedViewRect();
    int32 Width, Height;
    PC->GetViewportSize(Width, Height);
    if (!Width || !Height || !Rect.Width() || !Rect.Height()) return;
    constexpr double Depth = 520.;
    const double HalfWidth = Depth / View.ProjectionMatrix.M[0][0];
    const double HalfHeight = Depth / View.ProjectionMatrix.M[1][1];
    const double Span = FMath::Min(HalfWidth*.23, HalfHeight*.35);
    if (!FittingStars)
    {
        FittingStars = Instances(Materials[7], TEXT("/Engine/BasicShapes/Sphere.Sphere"));
        FittingStars->AttachToComponent(Camera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        FRandomStream Sky(21542);
        for (int32 I = 0; I < 190; ++I)
        {
            const double Y = Sky.FRandRange(-5900.f,5900.f);
            const double Z = Sky.FRandRange(-3200.f,3200.f);
            const double Size = Sky.FRandRange(.025f,.073f);
            FittingStars->AddInstance(FTransform(FQuat::Identity, FVector(6000,Y,Z), FVector(Size)));
        }
    }
    if (!FittingStars->IsVisible()) FittingStars->SetVisibility(true);
    auto Anchor = [&](FVector2D UV) {
        const FVector2D Pixel = UV * FVector2D(Width, Height);
        return FVector(Depth, HalfWidth*(2.*(Pixel.X-Rect.Min.X)/Rect.Width()-1.),
            HalfHeight*(1.-2.*(Pixel.Y-Rect.Min.Y)/Rect.Height()));
    };
    const FVector Target = Anchor(PreviewTargetUV());
    auto Pose = [](UStaticMeshComponent* Preview, FVector Pivot, const FQuat& Rotation, double Length) {
        if (!Preview->IsVisible()) Preview->SetVisibility(true);
        const FBox Bounds = Preview->GetStaticMesh()->GetBoundingBox();
        const double Scale = Length / Bounds.GetSize().GetMax();
        // Spin about the mesh center so every silhouette stays over its click target.
        Preview->SetRelativeTransform(FTransform(Rotation, Pivot - Rotation.RotateVector(Bounds.GetCenter()*Scale), FVector(Scale)));
    };
    auto Display = [&](UStaticMeshComponent* Preview, FVector2D UV, int32 I, bool bAim) {
        const FVector Pivot = Anchor(UV);
        const FQuat Rotation = bAim ? FRotationMatrix::MakeFromXZ(Target-Pivot,FVector(-1,0,.54)).ToQuat() :
            Pivot.Rotation().Quaternion() * FRotator(32,0,0).Quaternion() *
            FRotator(0, 62. + 23.*I + FMath::Fmod(AliveTime*21.,360.), 0).Quaternion();
        Pose(Preview,Pivot,Rotation,Span);
    };
    for (const auto& Model : AquilaWeapons::Models)
    {
        auto& Preview = TurretPreviews[int32(Model.Weapon)];
        if (SelectedPart < 0) { if (Preview && Preview->IsVisible()) Preview->SetVisibility(false); continue; }
        if (!Preview) Preview = TurretModel(Camera, Model.Weapon, false);
        Display(Preview, TurretPreviewUV(Model.Weapon), int32(Model.Weapon), FiringPreview && FiringPreview->Gun.Weapon == Model.Weapon);
    }
    for (const auto& Model : AquilaDefenses::Models)
    {
        auto& Preview = DefensePreviews[int32(Model.Type)];
        if (!DefenseGalleryVisible(Model.Type)) { if (Preview && Preview->IsVisible()) Preview->SetVisibility(false); continue; }
        if (!Preview) Preview = DefenseModel(Camera, Model.Type, bFitEnemy);
        else if (AquilaDefenses::IsPlating(Model.Type))
        {
            auto* Finish=ArmorFinish(bFitEnemy,static_cast<EAquilaArmor>(Model.Type));
            const int32 Slot=Preview->GetMaterialIndex(TEXT("Plate"));
            if (Preview->GetMaterial(Slot)!=Finish) Preview->SetMaterial(Slot,Finish);
        }
        Display(Preview, DefensePreviewUV(Model.Type), int32(Model.Type), false);
    }
    const bool bShowAmmo = AmmoFittingWeapon().IsSet();
    const double AmmoSpan = FMath::Min(HalfWidth*.16, HalfHeight*.27);
    const FQuat Profile = FRotationMatrix::MakeFromXZ(FVector(-.30,.92,.30),FVector(-1,0,.4)).ToQuat();
    for (const auto& Style : AquilaAmmunition::Models)
    {
        const int32 I = int32(Style.Type);
        auto& Preview = AmmoPreviews[I];
        if (!bShowAmmo) { if (Preview && Preview->IsVisible()) Preview->SetVisibility(false); continue; }
        if (!Preview) Preview = AmmoModel(Style);
        const FQuat Roll(FVector::ForwardVector,FMath::DegreesToRadians(FMath::Fmod(AliveTime*27.+I*33.,360.)));
        Pose(Preview,Anchor(AmmoPreviewUV(Style.Type)),Profile*Roll,AmmoSpan);
    }
    if (FiringPreview) RenderFiringPreview(Target, FMath::Min(HalfWidth*.23, HalfHeight*.38));
}

bool AAquilaFlightPawn::DefenseGalleryVisible(EAquilaDefense Type) const
{
    return SelectedPart == (AquilaDefenses::IsPlating(Type) ? ArmorPart : ShieldPart);
}

FVector2D AAquilaFlightPawn::DefensePreviewUV(EAquilaDefense Type)
{
    const FVector2D Slots[] = {{.18,.28},{.82,.28},{.5,.76}};
    const int32 Index = int32(Type) - (AquilaDefenses::IsPlating(Type) ? 0 : int32(EAquilaDefense::ShieldEmitter));
    return Slots[Index];
}

FVector2D AAquilaFlightPawn::AmmoPreviewUV(EAquilaAmmo Type)
{
    return FVector2D(.5 + (int32(Type)-1)*.135, .851);
}
