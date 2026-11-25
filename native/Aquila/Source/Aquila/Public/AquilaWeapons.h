#pragma once

#include "CoreMinimal.h"

enum class EAquilaWeapon : uint8 { TwinTurrets, KineticCannon, PlasmaCaster, PulseRepeater };

namespace AquilaWeapons
{
struct FModel
{
    EAquilaWeapon Weapon;
    const TCHAR* Name;
    const TCHAR* Asset;
};
inline constexpr FModel Models[] = {
    {EAquilaWeapon::TwinTurrets, TEXT("Burst turret"), TEXT("SM_BurstTurret")},
    {EAquilaWeapon::KineticCannon, TEXT("Kinetic cannon"), TEXT("SM_KineticCannon")},
    {EAquilaWeapon::PlasmaCaster, TEXT("Plasma caster"), TEXT("SM_PlasmaCaster")},
    {EAquilaWeapon::PulseRepeater, TEXT("Pulse repeater"), TEXT("SM_PulseRepeater")}
};
}
