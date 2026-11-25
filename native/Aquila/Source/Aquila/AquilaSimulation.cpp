#include "AquilaSimulation.h"

namespace
{
constexpr double FixedStep = 1.0 / 120.0;
constexpr double WeaponRange = 65000;
constexpr double ShipSpeed = 2800;
const FVector PlanetCenter(180000, 0, 0);
constexpr double PlanetClearance = 80000;
constexpr bool bEnableLabCourse = false;

struct FWeaponProfile
{
    double Speed;
    double Damage;
    double Interval;
};

FWeaponProfile WeaponProfile(EAquilaWeapon Weapon, EAquilaAmmo Ammo)
{
    switch (Weapon)
    {
    case EAquilaWeapon::KineticCannon: return {7000, 10, 1.2};
    case EAquilaWeapon::PlasmaCaster:  return {4000, 36, 1.8};
    case EAquilaWeapon::PulseRepeater: return {22000, 8, 0.55};
    default: return {8500, Ammo == EAquilaAmmo::Piercing ? 8. : 6., 0.14};
    }
}

double FirstShotDelay(EAquilaWeapon Weapon, int32 Item)
{
    return Weapon == EAquilaWeapon::TwinTurrets ? 0.4 + (Item % 4) * 0.25 : 1.5;
}

double HullMultiplier(EAquilaArmor Armor, bool bPlasma, EAquilaAmmo Ammo)
{
    if (Armor == EAquilaArmor::Prism && bPlasma) return 0;
    if (Armor == EAquilaArmor::Reinforced && !bPlasma) return Ammo == EAquilaAmmo::Piercing ? 0.75 : 0;
    return 1;
}
}

const TCHAR* FAquilaTurret::RoundsName() const
{
    if (FAquilaTurret::IsPlasma(Weapon)) return TEXT("Plasma bolts");
    if (Ammo == EAquilaAmmo::Phase) return TEXT("Phase rounds");
    return Ammo == EAquilaAmmo::Piercing ? TEXT("Piercing rounds") : TEXT("Standard rounds");
}

double FAquilaTurret::BaseDamage() const
{
    return WeaponProfile(Weapon, Ammo).Damage;
}

FTransform FAquilaTurret::MountedPose(bool bEnemy) const
{
    check(Mount>=0 && Mount<MountCount);
    const FTransform& Seat=AquilaHulls::Layout(bEnemy).Guns[Mount];
    // The common mounting ring is centred nine units aft, eight below the weapon pivot.
    const FVector Base(-9.,0.,-8.);
    const FVector Pivot=Seat.TransformPosition(-Base*AquilaHulls::GunScale-FVector(0,0,AquilaHulls::SeatInset));
    return FTransform(Seat.GetRotation(),Pivot,FVector(AquilaHulls::GunScale));
}

FVector FAquilaTurret::BarrelTip(EAquilaWeapon Type,int32 Barrel)
{
    switch (Type)
    {
    case EAquilaWeapon::TwinTurrets: return FVector(57.4,Barrel%2 ? 15 : -15,19);
    case EAquilaWeapon::KineticCannon: return FVector(69,0,21);
    case EAquilaWeapon::PlasmaCaster: return FVector(39,0,23);
    default: return FVector(40,0,19);
    }
}

FVector FAquilaTurret::Muzzle(bool bEnemy,int32 Barrel) const
{
    return MountedPose(bEnemy).TransformPosition(BarrelTip(Weapon,Barrel));
}

int32 FAquilaTurretRack::At(int32 Mount) const
{
    return Stock.IndexOfByPredicate([Mount](const FAquilaTurret& T) { return T.Mount == Mount; });
}

int32 FAquilaTurretRack::Installed() const
{
    int32 N = 0;
    for (const auto& T : Stock) if (T.Mount != INDEX_NONE) ++N;
    return N;
}

bool FAquilaTurretRack::Place(int32 Item, int32 Mount)
{
    if (!Stock.IsValidIndex(Item) || Mount < INDEX_NONE || Mount >= FAquilaTurret::MountCount) return false;
    if (Mount != INDEX_NONE && At(Mount) != INDEX_NONE) return false;
    Stock[Item].Mount = Mount;
    return true;
}

bool FAquilaTurretRack::Fit(int32 Mount, EAquilaWeapon Type)
{
    if (Mount < 0 || Mount >= FAquilaTurret::MountCount) return false;
    int32 Item = At(Mount);
    if (Item == INDEX_NONE)
    {
        Item = At(INDEX_NONE);
        if (!Place(Item, Mount)) return false;
    }
    Stock[Item].Weapon = Type;
    return true;
}

FAquilaTurret FAquilaShipLoadout::Starter(int32 Item) const
{
    const int32 Mount = Weapon == EAquilaWeapon::TwinTurrets ? (Item == 0 ? 6 : 26) : 14;
    return {Mount, Weapon, bPiercing ? EAquilaAmmo::Piercing : EAquilaAmmo::Standard, false};
}

void FAquilaShipLoadout::PrepareRack()
{
    if (Rack)
    {
        auto& Stock = Rack->Stock;
        if (Stock.Num() > FAquilaTurret::MountCount) Stock.SetNum(FAquilaTurret::MountCount);
        bool Used[FAquilaTurret::MountCount] = {};
        for (auto& T : Stock)
        {
            if (T.Mount < 0 || T.Mount >= FAquilaTurret::MountCount || Used[T.Mount]) T.Mount = INDEX_NONE;
            else Used[T.Mount] = true;
        }
        return;
    }
    auto& Inventory = Rack.Emplace();
    for (int32 Item = 0; Item < StarterCount(); ++Item) Inventory.Stock.Add(Starter(Item));
}

void FAquilaShipLoadout::ClearFitting()
{
    PrepareRack();
    for (auto& Turret : Rack->Stock) Turret = FAquilaTurret{};
    Armor = EAquilaArmor::BareHull;
    bPiercing = bShield = bReserveCapacitor = bWeave = false;
}

void FAquilaCampaign::BeginDuel(const FAquilaDuelLoadout& Fitting)
{
    Loadout = Fitting;
    if (!Loadout.Resolute.bShield) Loadout.Resolute.bReserveCapacitor = false;
    if (!Loadout.Sentinel.bShield) Loadout.Sentinel.bReserveCapacitor = false;
    Loadout.Resolute.bWeave = Loadout.Sentinel.bWeave = false;
    Loadout.Resolute.PrepareRack();
    Loadout.Sentinel.PrepareRack();
    auto& Lab = DuelStats.Emplace();
    Shield = Loadout.Resolute.bShield ? 100 : 0;
    BeginExpeditionBattle();
    Lab.Axis = (EnemyPosition - ShipPosition).GetSafeNormal();
    Lab.Sideways = FVector::CrossProduct(FVector::UpVector, Lab.Axis);
}

void FAquilaCampaign::EditDuelShip(bool bEnemy, TFunctionRef<void(FAquilaShipLoadout&)> Edit)
{
    if (!DuelStats) return;
    auto& Ship = Loadout.ForShip(bEnemy);
    const auto Before = Ship;
    Edit(Ship);
    Ship.PrepareRack();

    const int32 Side = bEnemy ? 1 : 0;
    auto& Cycle = Guns[Side];
    bool& Held = bEnemy ? bEnemyArmorBlocked : bArmorBlocked;
    const auto& Stock = Ship.Rack->Stock;
    for (int32 I = 0; I < Stock.Num(); ++I)
    {
        const auto& Gun = Stock[I];
        const auto Old = Before.Rack->Stock.IsValidIndex(I) ? Before.Rack->Stock[I] : FAquilaTurret{};
        const bool bGunChanged = Gun.Mount != Old.Mount || Gun.Weapon != Old.Weapon;
        if (bGunChanged)
        {
            Cycle.Clock[I] = FirstShotDelay(Gun.Weapon, I);
            Cycle.Burst[I] = 0;
        }
        if (bGunChanged || Gun.Ammo != Old.Ammo || Gun.bIntercept != Old.bIntercept)
        {
            Cycle.Blocked[I] = false;
            Held = false;
        }
    }
    if (!Ship.Rack->Installed()) Held = true;

    double& Charge = bEnemy ? EnemyShield : Shield;
    if (!Ship.bShield) Charge = 0;
    else if (!Before.bShield && (bEnemy ? EnemyHull : Hull) > 0) Charge = 100;
    if (Before.Armor != Ship.Armor || Before.bShield != Ship.bShield)
    {
        // An opponent held by the old defenses gets to try its fitted guns again.
        FMemory::Memzero(Guns[1-Side].Blocked);
        bool& OpponentHeld = bEnemy ? bArmorBlocked : bEnemyArmorBlocked;
        OpponentHeld = Loadout.ForShip(!bEnemy).Rack->Installed() == 0;
    }
}

bool FAquilaCampaign::StartPractice()
{
    if (ExpeditionActive()) return false;
    Shield = Hull = 100;
    StepRemainder = 0;
    BeginEncounter();
    return true;
}

void FAquilaCampaign::BeginEncounter()
{
    ++EncounterGeneration;
    bBattle = true;
    EnemyHull = 100;
    EnemyShield = Loadout.Sentinel.bShield ? 100 : 0;
    BattleTime = 0;
    for (int32 Side = 0; Side < 2; ++Side)
    {
        Guns[Side] = FAquilaFiringCycle();
        const auto& Fit = Loadout.ForShip(Side != 0);
        const int32 Count = Fit.Rack ? Fit.Rack->Stock.Num() : Fit.StarterCount();
        for (int32 I = 0; I < Count; ++I)
        {
            const auto Gun = Fit.Rack ? Fit.Rack->Stock[I] : Fit.Starter(I);
            Guns[Side].Clock[I] = FirstShotDelay(Gun.Weapon, I);
        }
    }
    bArmorBlocked = bEnemyArmorBlocked = bHoldFire = Loadout.Resolute.bPiercing = false;
    EnemyPosition = ShipPosition + FVector(35000, 12000, 0);
    if (FVector::Distance(EnemyPosition, PlanetCenter) < PlanetClearance)
        EnemyPosition = ShipPosition + FVector(-35000, 12000, 0);
    Order = EAquilaOrder::Hold;
    Rounds.Reset();
    Impacts.Reset();
}

void FAquilaCampaign::SetPiercing(bool bEnabled)
{
    Loadout.Resolute.bPiercing = bEnabled;
    bArmorBlocked = false;
}

bool FAquilaCampaign::SegmentHitsSphere(const FVector& Start, const FVector& End, const FVector& Center, double Radius, FVector* Entry)
{
    const FVector Delta = End - Start;
    const double LengthSquared = Delta.SizeSquared();
    const double Projection = Delta.IsNearlyZero() ? 0 : FVector::DotProduct(Center - Start, Delta) / LengthSquared;
    if (FVector::DistSquared(Start + FMath::Clamp(Projection, 0., 1.) * Delta, Center) > Radius * Radius) return false;
    if (Entry)
    {
        double Fraction = 0;
        if (FVector::DistSquared(Start, Center) > Radius * Radius && LengthSquared > 0)
        {
            const double MissSquared = FVector::DistSquared(Start + Projection * Delta, Center);
            Fraction = FMath::Clamp(Projection - FMath::Sqrt(FMath::Max(0., Radius * Radius - MissSquared) / LengthSquared), 0., 1.);
        }
        *Entry = Start + Fraction * Delta;
    }
    return true;
}

void FAquilaCampaign::Advance(double Seconds)
{
    if (!FMath::IsFinite(Seconds) || Seconds <= 0) return;
    // Bounded catch-up avoids a mobile resume or debugger hitch producing a giant simulation jump.
    StepRemainder += FMath::Min(Seconds, 0.25);
    while (StepRemainder + 1e-10 >= FixedStep)
    {
        Step(FixedStep);
        StepRemainder = FMath::Max(0.0, StepRemainder - FixedStep);
    }
}

void FAquilaCampaign::FireBattery(bool bEnemy, const FVector& TargetVelocity, double Range, double Seconds)
{
    if ((bEnemy ? EnemyHull : Hull) <= 0) return;
    const bool bCanAttackShip = (bEnemy ? Hull : EnemyHull) > 0 && Range < WeaponRange;
    if (!bCanAttackShip && (!DuelStats || Rounds.IsEmpty())) return;
    const auto& Fit = Loadout.ForShip(bEnemy);
    const auto& Defender = Loadout.ForShip(!bEnemy);
    const int32 Side = bEnemy ? 1 : 0;
    auto& Cycle = Guns[Side];
    const FVector& Shooter = bEnemy ? EnemyPosition : ShipPosition;
    const FVector& Target = bEnemy ? ShipPosition : EnemyPosition;
    const FRotator Facing(0, bEnemy ? 180. : FMath::RadiansToDegrees(Heading), 0);
    bool& Held = bEnemy ? bEnemyArmorBlocked : bArmorBlocked;
    const int32 Count = Fit.Rack ? Fit.Rack->Stock.Num() : Fit.StarterCount();
    bool bAllBlocked = true;
    for (int32 I = 0; I < Count; ++I)
    {
        const auto Gun = Fit.Rack ? Fit.Rack->Stock[I] : Fit.Starter(I);
        if (Gun.Mount == INDEX_NONE) continue;
        const bool bPlasma = FAquilaTurret::IsPlasma(Gun.Weapon);
        const bool bBlocked = (Fit.Rack ? Cycle.Blocked[I] : Held) && HullMultiplier(Defender.Armor, bPlasma, Gun.Ammo) == 0;
        bAllBlocked &= bBlocked;
        Cycle.Clock[I] -= Seconds;
        if (Cycle.Clock[I] > 0 || (!bEnemy && bHoldFire)) continue;
        const FVector Muzzle = Shooter + Facing.RotateVector(Gun.Muzzle(bEnemy,Cycle.Burst[I]) * 10.);
        const auto Profile = WeaponProfile(Gun.Weapon, Gun.Ammo);
        if (Gun.bIntercept && DuelStats)
        {
            auto* Incoming = Rounds.FindByPredicate([&](const FAquilaRound& R) {
                return R.bEnemy != bEnemy && !R.bClaimed && FVector::DistSquared(R.Position, Shooter) < FMath::Square(15000.);
            });
            if (Incoming)
            {
                Incoming->bClaimed = true;
                const double Flight = Gun.Weapon == EAquilaWeapon::PlasmaCaster ? 0.7 : 0.35;
                DuelStats->ScreenShots.Add({Incoming->Id, bEnemy, Muzzle, Muzzle, 0., Flight});
                Cycle.Clock[I] = Profile.Interval;
                continue;
            }
        }
        // A chip may still claim a live round when its original firing ship is gone.
        if (!bCanAttackShip || bBlocked || Rounds.Num() + (DuelStats ? DuelStats->ScreenShots.Num() : 0) >= MaxRounds) continue;
        auto& Round = Rounds.AddDefaulted_GetRef();
        Round.Position = Muzzle;
        FVector Aim = Target;
        Cycle.Clock[I] = Profile.Interval;
        if (Gun.Weapon == EAquilaWeapon::KineticCannon) Aim += TargetVelocity * (Range / Profile.Speed);
        if (Gun.Weapon == EAquilaWeapon::TwinTurrets)
        {
            Cycle.Burst[I] = (Cycle.Burst[I] + 1) % 4;
            if (Cycle.Burst[I] == 0) Cycle.Clock[I] = 1.65;
        }
        Round.Velocity = (Aim - Muzzle).GetSafeNormal() * Profile.Speed;
        Round.bEnemy = bEnemy;
        Round.Ammo = bPlasma ? EAquilaAmmo::Standard : Gun.Ammo;
        Round.Turret = I;
        Round.Weapon = Gun.Weapon;
        if (DuelStats) Round.Id = ++DuelStats->NextRoundId;
        Round.DamageScale = Expedition == EAquilaExpedition::Battle ? ExpeditionDamageScale : 1;
        if (!bEnemy && Gun.Weapon == EAquilaWeapon::TwinTurrets && Gun.Mount == 6 && PortTurretLevel > 0) Round.DamageScale *= 1.5;
    }
    if (Fit.Rack) Held = bAllBlocked;
}

void FAquilaCampaign::AdvanceScreenShots(double Seconds)
{
    auto& Lab = DuelStats.GetValue();
    for (auto& Flash : Lab.Flashes) Flash.Age += Seconds;
    Lab.Flashes.RemoveAll([](const FAquilaInterceptFlash& Flash) { return Flash.Age >= .8; });
    for (int32 I = Lab.ScreenShots.Num() - 1; I >= 0; --I)
    {
        auto& Bolt = Lab.ScreenShots[I];
        const int32 Target = Rounds.IndexOfByPredicate([&](const FAquilaRound& R) { return R.Id == Bolt.TargetId; });
        if (Target != INDEX_NONE)
        {
            Bolt.Time += Seconds;
            Bolt.Position = FMath::Lerp(Bolt.Origin, Rounds[Target].Position, FMath::Min(1., Bolt.Time / Bolt.Duration));
            if (Bolt.Time < Bolt.Duration) continue;
            if (Lab.Flashes.Num() == 32) Lab.Flashes.RemoveAt(0, 1, EAllowShrinking::No);
            Lab.Flashes.Add({Bolt.Position});
            Rounds.RemoveAtSwap(Target);
            ++Lab.Shots[Bolt.bEnemy ? 1 : 0].Intercepted;
        }
        Lab.ScreenShots.RemoveAtSwap(I);
    }
}

void FAquilaCampaign::Step(double Seconds)
{
    const FVector OldPosition = ShipPosition;
    const FVector OldEnemyPosition = EnemyPosition;
    if (Order == EAquilaOrder::Withdraw && FVector::Distance(ShipPosition, EnemyPosition) >= 85000)
        Order = EAquilaOrder::Hold;
    if (Order != EAquilaOrder::Hold && Hull > 0)
    {
        FVector Destination = PlanetCenter - FVector(PlanetClearance + 10000, 0, 0);
        if (Order == EAquilaOrder::Signal) Destination = SignalPosition;
        if (Order == EAquilaOrder::Recover) Destination = EnemyPosition;
        if (Order == EAquilaOrder::CombatRange || Order == EAquilaOrder::Withdraw)
        {
            FVector Away = (ShipPosition - EnemyPosition).GetSafeNormal();
            if (Away.IsNearlyZero()) Away = FVector(-1, 0, 0);
            Destination = EnemyPosition + Away * (Order == EAquilaOrder::CombatRange ? 35000 : 85000);
        }
        const FVector ToGoal = Destination - ShipPosition;
        const double ArrivalRadius = Order == EAquilaOrder::Recover ? 5000 : 500;
        if (ToGoal.Size() < ArrivalRadius) Order = EAquilaOrder::Hold;
        else
        {
            const double DesiredHeading = FMath::Atan2(ToGoal.Y, ToGoal.X);
            Heading += FMath::Clamp(FMath::FindDeltaAngleRadians(Heading, DesiredHeading), -0.45 * Seconds, 0.45 * Seconds);
            const double Alignment = FMath::Max(0.0, FMath::Cos(DesiredHeading - Heading));
            ShipPosition += FVector(FMath::Cos(Heading), FMath::Sin(Heading), 0) * FMath::Min(ShipSpeed * Seconds * Alignment, ToGoal.Size());
            if (FVector::Distance(ShipPosition, PlanetCenter) < PlanetClearance)
            {
                ShipPosition = PlanetCenter + (ShipPosition - PlanetCenter).GetSafeNormal() * PlanetClearance;
                Order = EAquilaOrder::Hold;
            }
        }
    }
    for (auto& Impact : Impacts) Impact.Age += Seconds;
    // One render frame can contain eight capped quarter-second advances in the fast duel.
    Impacts.RemoveAll([](const FAquilaImpact& Impact) { return Impact.Age > 2.1; });
    if (!bBattle) { AdvanceExpedition(Seconds); return; }
    const double PreviousTime = BattleTime;
    BattleTime += Seconds;
    if (bEnableLabCourse && DuelStats && (Loadout.Resolute.bWeave || Loadout.Sentinel.bWeave))
    {
        // A bounded lateral course. Shots keep their launch direction when either ship changes course.
        const FVector Drift = DuelStats->Sideways * (7000. * (FMath::Sin(BattleTime * 0.40) - FMath::Sin(PreviousTime * 0.40)));
        if (Loadout.Sentinel.bWeave && EnemyHull > 0) EnemyPosition += Drift;
        if (Loadout.Resolute.bWeave && Hull > 0) ShipPosition -= Drift;
    }
    const double Range = FVector::Distance(ShipPosition, EnemyPosition);
    FireBattery(false, (EnemyPosition - OldEnemyPosition) / Seconds, Range, Seconds);
    FireBattery(true, (ShipPosition - OldPosition) / Seconds, Range, Seconds);
    for (int32 Index = Rounds.Num() - 1; Index >= 0; --Index)
    {
        auto& Round = Rounds[Index];
        const FVector Previous = Round.Position;
        Round.Position += Round.Velocity * Seconds;
        Round.Age += Seconds;
        const FVector Target = Round.bEnemy ? ShipPosition : EnemyPosition;
        double& TargetShield = Round.bEnemy ? Shield : EnemyShield;
        double& TargetHull = Round.bEnemy ? Hull : EnemyHull;
        // Relative swept contact also catches a moving target crossing a projectile between frames.
        const FVector RelativeStart = Previous - (Round.bEnemy ? OldPosition : OldEnemyPosition);
        const bool bPlanetHit = SegmentHitsSphere(Previous, Round.Position, FVector(180000,0,-30000), 60000);
        const FVector RelativeEnd = Round.Position - Target;
        const bool bShieldContact = Round.HitsShield(TargetShield);
        FVector Entry;
        const bool bHit = !bPlanetHit && TargetHull > 0 && SegmentHitsSphere(RelativeStart, RelativeEnd, FVector::ZeroVector,
            bShieldContact ? ShieldRadius : HullRadius, &Entry);
        if (bHit)
        {
            const double HullBefore = TargetHull;
            const auto& Attacker = Loadout.ForShip(Round.bEnemy);
            const auto& Defender = Loadout.ForShip(!Round.bEnemy);
            // Old match journals have no per-item identity. Their battery has one fixed weapon type.
            const auto Weapon = Round.Turret == INDEX_NONE ? Attacker.Weapon : Round.Weapon;
            const bool bPlasma = FAquilaTurret::IsPlasma(Weapon);
            const double Damage = WeaponProfile(Weapon, Round.Ammo).Damage * Round.DamageScale;
            const double Absorbed = bShieldContact ? FMath::Min(TargetShield, Damage) : 0.;
            TargetShield -= Absorbed;
            double Remainder = Damage - Absorbed;
            const int32 DefenderIndex = Round.bEnemy ? 0 : 1;
            if (bShieldContact && TargetShield == 0 && DuelStats && Defender.bReserveCapacitor && !DuelStats->bReserveSpent[DefenderIndex])
            {
                DuelStats->bReserveSpent[DefenderIndex] = true;
                const double ReserveAbsorbed = FMath::Min(100., Remainder);
                TargetShield = 100. - ReserveAbsorbed;
                Remainder -= ReserveAbsorbed;
            }
            const double Multiplier = HullMultiplier(Defender.Armor, bPlasma, Round.Ammo);
            const bool bBlocked = Remainder > 0 && Multiplier == 0;
            if (bBlocked)
            {
                // A previously launched standard round cannot disable newly fitted piercing ammunition.
                if (Attacker.Rack && Attacker.Rack->Stock.IsValidIndex(Round.Turret))
                {
                    bool bAllStopped = true;
                    for (int32 I = 0; I < Attacker.Rack->Stock.Num(); ++I)
                    {
                        const auto& Gun = Attacker.Rack->Stock[I];
                        if (Gun.Mount == INDEX_NONE) continue;
                        auto& Stopped = Guns[Round.bEnemy ? 1 : 0].Blocked[I];
                        if (FAquilaTurret::IsPlasma(Gun.Weapon) == bPlasma && (bPlasma || Gun.Ammo == Round.Ammo)) Stopped = true;
                        bAllStopped &= Stopped;
                    }
                    (Round.bEnemy ? bEnemyArmorBlocked : bArmorBlocked) = bAllStopped;
                }
                else if (HullMultiplier(Defender.Armor, bPlasma, Attacker.Starter(0).Ammo) == 0)
                    (Round.bEnemy ? bEnemyArmorBlocked : bArmorBlocked) = true;
            }
            else TargetHull = FMath::Max(0.0, TargetHull - Remainder * Multiplier);
            if (DuelStats)
            {
                auto& Tally = DuelStats->Shots[Round.bEnemy ? 1 : 0];
                if (bBlocked) ++Tally.Blocked;
                else ++Tally.Hits;
            }
            if (Impacts.Num() == 64)
            {
                const int32 Oldest = Impacts.IndexOfByPredicate([](const FAquilaImpact& I) { return !I.bDestroyed; });
                Impacts.RemoveAt(Oldest, 1, EAllowShrinking::No);
            }
            auto& Hit = Impacts.AddDefaulted_GetRef();
            const FRotator Facing(0, Round.bEnemy ? FMath::RadiansToDegrees(Heading) : 180., 0);
            Hit.LocalContact = Facing.UnrotateVector(Entry);
            Hit.LocalDirection = Facing.UnrotateVector((RelativeEnd - RelativeStart).GetSafeNormal(UE_SMALL_NUMBER, Round.Velocity.GetSafeNormal()));
            Hit.Serial = ++LastImpactSerial;
            Hit.RoundId = Round.Id;
            Hit.bTargetEnemy = !Round.bEnemy;
            Hit.bShield = Absorbed > 0;
            Hit.bBlocked = bBlocked;
            Hit.bHull = TargetHull < HullBefore;
            Hit.bDestroyed = TargetHull == 0;
            if (!Round.bEnemy && TargetHull == 0) ++Victories;
        }
        // The duel's weaving lanes stay transverse to this plane. A shot beyond it has cleared the target.
        const bool bPassed = DuelStats && TargetHull > 0 && (Round.bEnemy ? -1. : 1.) *
            FVector::DotProduct(RelativeEnd, DuelStats->Axis) > ShieldRadius && FVector::DotProduct(Round.Velocity, RelativeEnd) > 0;
        if (bHit || bPlanetHit || bPassed || Round.Age > 20)
        {
            if (DuelStats && !bHit && TargetHull > 0) ++DuelStats->Shots[Round.bEnemy ? 1 : 0].Misses;
            Rounds.RemoveAtSwap(Index);
        }
    }
    if (DuelStats) AdvanceScreenShots(Seconds);
    if (Hull <= 0 || (EnemyHull <= 0 && Expedition != EAquilaExpedition::Salvage)) Order = EAquilaOrder::Hold;
    AdvanceExpedition(Seconds);
}

FString FAquilaCampaign::CombatStatus() const
{
    if (!bBattle) return TEXT("EXPLORATION  /  SYSTEMS READY");
    if (Hull <= 0) return TEXT("DELTA TREE DISABLED  /  RETRY AVAILABLE");
    if (EnemyHull <= 0) return TEXT("TARGET DISABLED  /  SECTOR SECURED");
    if (bArmorBlocked && !Loadout.Resolute.bPiercing) return TEXT("ARMOR BLOCKS STANDARD ROUNDS  /  FIT PIERCING");
    if (bHoldFire) return TEXT("HOLD FIRE  /  ROUNDS IN FLIGHT REMAIN LIVE");
    if (FVector::Distance(ShipPosition, EnemyPosition) >= WeaponRange) return TEXT("TARGET OUT OF RANGE");
    return TEXT("AUTOMATIC ENGAGEMENT  /  TWO KINETIC TURRETS");
}
