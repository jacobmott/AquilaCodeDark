#include "AquilaHullLayout.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshSocket.h"

UStaticMesh* AquilaHulls::LoadMesh(bool bGlacier)
{
    return LoadObject<UStaticMesh>(nullptr,bGlacier
        ? TEXT("/Game/Foundation/SM_GlacierBoardingHull.SM_GlacierBoardingHull")
        : TEXT("/Game/Foundation/SM_DeltaBoardingHull.SM_DeltaBoardingHull"));
}

UMaterialInterface* AquilaHulls::Surface(bool bGlacier, FName Slot)
{
    auto* Mesh=LoadMesh(bGlacier);
    check(Mesh);
    const int32 Index=Mesh->GetMaterialIndex(Slot);
    checkf(Index!=INDEX_NONE,TEXT("Missing %s finish on %s"),*Slot.ToString(),*Mesh->GetName());
    return Mesh->GetMaterial(Index);
}

static AquilaHulls::FLayout ReadLayout(bool bGlacier)
{
    using namespace AquilaHulls;
    auto* Mesh=LoadMesh(bGlacier);
    checkf(Mesh,TEXT("Prepare both boarding hulls before running Aquila."));
    const FBox Bounds=Mesh->GetBoundingBox();
    const double Scale=KeelLength/Bounds.GetSize().X;
    FLayout Result;
    FVector Pivot=Bounds.GetCenter();
    // Aim at the inhabited body. The survey mast extends far above the main deck.
    Pivot.Z=Bounds.Min.Z+Bounds.GetSize().Z*(bGlacier ? .42 : .29);
    Result.MeshToShip=FTransform(FQuat::Identity,-Pivot*Scale,FVector(Scale));
    auto Socket=[&](FName Name)
    {
        const auto* S=Mesh->FindSocket(Name);
        checkf(S,TEXT("Missing %s on %s. Run native Prepare before packaging."),*Name.ToString(),*Mesh->GetName());
        return FTransform(S->RelativeRotation,Result.MeshToShip.TransformPosition(S->RelativeLocation));
    };
    for (int32 I=0; I<GunCount; ++I) Result.Guns[I]=Socket(GunSocket(I));
    Result.Armor=Socket(TEXT("Armor"));
    Result.Shield=Socket(TEXT("Shield"));
    Result.Reserve=Socket(TEXT("Reserve"));
    Result.BridgeAnchor=Socket(bGlacier ? TEXT("CommandBridge") : TEXT("BoardingDock"));
    return Result;
}

const AquilaHulls::FLayout& AquilaHulls::Layout(bool bGlacier)
{
    // Only numeric transforms survive here. Saved mount indices require no asset or world references.
    static const FLayout Ships[]={ReadLayout(false),ReadLayout(true)};
    return Ships[int32(bGlacier)];
}
