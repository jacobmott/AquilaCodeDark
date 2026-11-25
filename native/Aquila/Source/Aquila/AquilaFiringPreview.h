#pragma once

#include "AquilaSimulation.h"

// A miniature duel with its own clock, one fitted gun and an expendable reinforced target.
struct FAquilaFiringPreview
{
    explicit FAquilaFiringPreview(FAquilaTurret Fitting);
    static constexpr int32 SampleMount = 14;
    static constexpr double Distance = 10800;
    FVector Origin() const { return Gun.Muzzle(false) * 10.; }
    FAquilaTurret Gun;
    FAquilaCampaign Range;
    FString Description;
    double ShieldCapacity = 0, HullCapacity = 0;
    double DescriptionAge = 0, TimeSinceResult = -1, ShieldBreakHold = 0;
    int32 Pass = 0;
    bool bSentIncoming = false;
    void Advance(double Seconds);
    FString Outcome() const;
private:
    void Restart();
};
