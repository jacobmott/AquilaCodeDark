#pragma once

#include "CoreMinimal.h"

class UMaterialInterface;

struct FAquilaHullSurface
{
    FName Slot;
    const TCHAR* ColorMap;
    const TCHAR* MapFamily;
    FVector2D TileMetres;
    float Emission = 0;
};

TConstArrayView<FAquilaHullSurface> AquilaHullSurfaces(bool bDelta);
bool PrepareAquilaShipMaterials(const FString& Library);
UMaterialInterface* AquilaBakedSurface(bool bDelta, FName Slot);
