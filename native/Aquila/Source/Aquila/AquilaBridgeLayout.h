#pragma once

#include "CoreMinimal.h"

// Boarding-bay metres. The cooked hull socket places the ship's bow on this bearing.
namespace AquilaBridge
{
    inline const FVector Low(27.5,-72.6,0), High(-152,-72.6,48), Center(-186,-72.6,48);
    constexpr double BowYaw=139.92707282;
    inline const FVector Fore=FRotator(0,BowYaw,0).Vector();
    inline const FVector Right=FVector::CrossProduct(FVector::UpVector,Fore);
    inline const FVector Station=Center+Fore*15.1;
    inline const FVector Console=Station-Fore*.85+Right*2.05;
    inline const FVector Drone=Center+Fore*208.+Right*7.+FVector(0,0,20.);
    inline const FVector Turret=Center+Fore*25.+Right*11.+FVector(0,0,.35);
    // Glacier's room uses its forward observation crown as local origin. The deck top is 0.18 m above it.
    inline const FVector GlacierWindow(8.3,0,1.12);
    constexpr double ShieldRadius=12., RangeMetres=10500., RideSeconds=8.;
    constexpr int32 Mount=14;
    inline FVector CarPosition(double Progress)
    {
        const auto Elbow=Low+FVector(0,0,12);
        if (Progress<.19) return FMath::Lerp(Low,Elbow,FMath::SmoothStep(0.,.19,Progress));
        return FMath::Lerp(Elbow,High,FMath::SmoothStep(.19,1.,Progress));
    }
}
