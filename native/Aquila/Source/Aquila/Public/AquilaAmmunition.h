#pragma once

#include "CoreMinimal.h"

enum class EAquilaAmmo : uint8 { Standard, Piercing, Phase };

namespace AquilaAmmunition
{
struct FModel
{
    EAquilaAmmo Type;
    const TCHAR* Name;
    const TCHAR* Asset;
    const TCHAR* Description;
    FLinearColor Finish;
    FLinearColor Tracer;
};

inline const FModel Models[] = {
    {EAquilaAmmo::Standard, TEXT("Standard"), TEXT("SM_StandardRound"), TEXT("Hits shields first. Reinforced armor stops these rounds."),
        {.63f,.30f,.075f}, {1.f,.64f,.19f}},
    {EAquilaAmmo::Piercing, TEXT("Piercing"), TEXT("SM_PiercingRound"), TEXT("Hits shields first. Penetrates reinforced armor."),
        {.19f,.36f,.48f}, {.17f,.83f,1.f}},
    {EAquilaAmmo::Phase, TEXT("Phase"), TEXT("SM_PhaseRound"), TEXT("Passes through shields. Reinforced armor stops these rounds."),
        {.37f,.105f,.64f}, {.67f,.28f,1.f}}
};
}
