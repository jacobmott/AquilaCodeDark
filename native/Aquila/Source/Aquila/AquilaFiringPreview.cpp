#include "AquilaFiringPreview.h"

FAquilaFiringPreview::FAquilaFiringPreview(FAquilaTurret Fitting) : Gun(Fitting)
{
    Gun.Mount = SampleMount;
    if (FAquilaTurret::IsPlasma(Gun.Weapon)) Gun.Ammo = EAquilaAmmo::Standard;
    switch (Gun.Weapon)
    {
    case EAquilaWeapon::TwinTurrets: Description = TEXT("Fires four quick rounds, then reloads. "); break;
    case EAquilaWeapon::KineticCannon: Description = TEXT("Fires one heavy round and leads moving targets. "); break;
    case EAquilaWeapon::PlasmaCaster: Description = TEXT("Heavy plasma bolts break shields and burn reinforced armor. Slow reload. Prism plating stops plasma."); break;
    case EAquilaWeapon::PulseRepeater: Description = TEXT("Fires fast, light plasma pulses. They damage shields and reinforced armor. Prism plating stops plasma."); break;
    }
    if (!FAquilaTurret::IsPlasma(Gun.Weapon))
    {
        if (Gun.Ammo == EAquilaAmmo::Phase) Description += TEXT("Phase rounds pass through shields. Reinforced armor stops them.");
        else if (Gun.Ammo == EAquilaAmmo::Piercing) Description += TEXT("Piercing rounds strip shields, then punch through reinforced armor.");
        else Description += TEXT("Standard rounds strip shields. Reinforced armor stops them.");
    }
    if (Gun.bIntercept) Description += TEXT(" The fitted chip also intercepts incoming fire.");
    Restart();
}

void FAquilaFiringPreview::Restart()
{
    FAquilaDuelLoadout Fit;
    Fit.Resolute.Rack.Emplace().Stock.Add(Gun);
    Fit.Sentinel.Rack.Emplace(); // The target has no regular battery.
    Range = FAquilaCampaign();
    Range.BeginDuel(Fit);
    Range.Order = EAquilaOrder::Hold;
    Range.EnemyPosition = Origin() + FVector(Distance,0,0);
    Range.DuelStats->Axis = (Range.EnemyPosition - Range.ShipPosition).GetSafeNormal();
    Range.DuelStats->Sideways = FVector::CrossProduct(FVector::UpVector, Range.DuelStats->Axis);
    Range.Guns[0].Clock[0] = .08;
    // A two-hit training shield makes each layer visible at the real weapon cadence.
    const double Damage = Gun.BaseDamage() * FAquilaCampaign::ExpeditionDamageScale;
    Range.EnemyShield = ShieldCapacity = 2. * Damage;
    Range.EnemyHull = HullCapacity = 4. * Damage;
    TimeSinceResult = -1;
    ShieldBreakHold = 0;
    bSentIncoming = false;
    ++Pass;
}

void FAquilaFiringPreview::Advance(double Seconds)
{
    if (Seconds <= 0) return;
    DescriptionAge += Seconds;
    if (ShieldBreakHold > 0)
    {
        const double Held = FMath::Min(ShieldBreakHold, Seconds);
        ShieldBreakHold -= Held;
        Seconds -= Held;
        if (Seconds <= 0) return;
    }
    // Give a phase round a readable passage between the two physical contact surfaces.
    const bool bCrossing = Gun.Ammo == EAquilaAmmo::Phase && Range.Rounds.ContainsByPredicate([this](const FAquilaRound& R) {
        const double Remaining = Range.EnemyPosition.X - R.Position.X;
        return !R.bEnemy && Remaining <= Range.ShieldRadius + 200 && Remaining > Range.HullRadius;
    });
    const double Before = Range.EnemyShield;
    Range.Advance(Seconds * (bCrossing ? .40 : 1.));
    if (Before > 0 && Range.EnemyShield == 0) ShieldBreakHold = .42;
    if (TimeSinceResult < 0 && (Range.DuelStats->Shots[0].Blocked || Range.EnemyHull < HullCapacity))
        TimeSinceResult = 0;
    if (TimeSinceResult < 0) return;
    TimeSinceResult += Seconds;
    if (Gun.bIntercept && !bSentIncoming && TimeSinceResult >= .35)
    {
        auto& Incoming = Range.Rounds.AddDefaulted_GetRef();
        Incoming.bEnemy = true;
        Incoming.Position = Range.EnemyPosition;
        Incoming.Velocity = (Range.ShipPosition - Incoming.Position).GetSafeNormal()*1700.;
        Incoming.Id = ++Range.DuelStats->NextRoundId;
        bSentIncoming = true;
    }
    if (TimeSinceResult >= (Gun.bIntercept ? 3.1 : 1.35)) Restart();
}

FString FAquilaFiringPreview::Outcome() const
{
    if (Range.DuelStats->Shots[0].Intercepted > 0) return TEXT("Chip intercepted incoming fire");
    if (Range.DuelStats->Shots[0].Blocked > 0)
        return Gun.Ammo == EAquilaAmmo::Phase ? TEXT("Shield bypassed  ·  Armor held") : TEXT("Reinforced armor held");
    if (Range.EnemyHull < HullCapacity)
        return FAquilaTurret::IsPlasma(Gun.Weapon) ? TEXT("Plasma damaged the hull") : TEXT("Reinforced armor pierced");
    if (Range.EnemyShield == 0) return TEXT("Shield down");
    if (Range.EnemyShield < ShieldCapacity) return TEXT("Shield absorbed the hit");
    if (Gun.Ammo == EAquilaAmmo::Phase && Range.Rounds.ContainsByPredicate([this](const FAquilaRound& R) {
        return !R.bEnemy && Range.EnemyPosition.X-R.Position.X < Range.ShieldRadius;
    })) return TEXT("Crossing the shield");
    return TEXT("Reinforced target");
}
