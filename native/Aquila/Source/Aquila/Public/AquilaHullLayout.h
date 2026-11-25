#pragma once

#include "CoreMinimal.h"

class UStaticMesh;
class UMaterialInterface;

namespace AquilaHulls
{
    constexpr double KeelLength=1360., GunScale=.48, SeatInset=3.5;
    constexpr int32 GunCount=30;
    inline FName GunSocket(int32 Index) { return FName(*FString::Printf(TEXT("Gun%02d"),Index+1)); }

    struct FLayout
    {
        FTransform MeshToShip;
        FTransform Guns[GunCount];
        FTransform Armor, Shield, Reserve, BridgeAnchor;
    };
    UStaticMesh* LoadMesh(bool bGlacier);
    UMaterialInterface* Surface(bool bGlacier, FName Slot);
    const FLayout& Layout(bool bGlacier);
}
