#pragma once

#include "CoreMinimal.h"

enum class EAquilaArmor : uint8 { Light, Reinforced, Prism, BareHull };
enum class EAquilaDefense : uint8 { LightPlate, ReinforcedPlate, PrismPlate, ShieldEmitter, ReserveCapacitor, EmptySocket };

namespace AquilaDefenses
{
struct FModel
{
    EAquilaDefense Type;
    const TCHAR* Name;
    const TCHAR* Asset;
};
inline constexpr FModel Models[] = {
    {EAquilaDefense::LightPlate, TEXT("Light plating"), TEXT("SM_LightPlate")},
    {EAquilaDefense::ReinforcedPlate, TEXT("Reinforced armor"), TEXT("SM_ReinforcedPlate")},
    {EAquilaDefense::PrismPlate, TEXT("Prism plating"), TEXT("SM_PrismPlate")},
    {EAquilaDefense::ShieldEmitter, TEXT("Standard shield"), TEXT("SM_ShieldEmitter")},
    {EAquilaDefense::ReserveCapacitor, TEXT("Reserve Capacitor"), TEXT("SM_ReserveCapacitor")},
    {EAquilaDefense::EmptySocket, TEXT("No shield"), TEXT("SM_EmptyShieldSocket")}
};
constexpr bool IsPlating(EAquilaDefense Type) { return Type <= EAquilaDefense::PrismPlate; }
inline TOptional<EAquilaDefense> Plate(EAquilaArmor Armor)
{
    if (Armor == EAquilaArmor::BareHull) return {};
    return static_cast<EAquilaDefense>(Armor);
}
}
