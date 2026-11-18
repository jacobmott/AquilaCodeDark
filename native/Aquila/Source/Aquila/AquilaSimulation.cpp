#include "AquilaSimulation.h"

namespace
{
constexpr double FixedStep = 1.0 / 120.0;
constexpr double WeaponRange = 65000;
constexpr double HullRadius = 2200;
constexpr double ShieldRadius = 3000;
constexpr double ShipSpeed = 2800;
const FVector PlanetCenter(180000, 0, 0);
constexpr double PlanetClearance = 80000;
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
    bBattle = true;
    EnemyShield = EnemyHull = 100;
    BattleTime = 0;
    TurretClock[0] = 0.4;
    TurretClock[1] = 0.65;
    EnemyClock = 1.5;
    BurstCount[0] = BurstCount[1] = 0;
    bArmorBlocked = bHoldFire = bPiercing = false;
    EnemyPosition = ShipPosition + FVector(35000, 12000, 0);
    if (FVector::Distance(EnemyPosition, PlanetCenter) < PlanetClearance)
        EnemyPosition = ShipPosition + FVector(-35000, 12000, 0);
    Order = EAquilaOrder::Hold;
    Rounds.Reset();
    Impacts.Reset();
}

void FAquilaCampaign::SetPiercing(bool bEnabled)
{
    bPiercing = bEnabled;
    bArmorBlocked = false;
}

bool FAquilaCampaign::SegmentHitsSphere(const FVector& Start, const FVector& End, const FVector& Center, double Radius)
{
    const FVector Delta = End - Start;
    const double Alpha = Delta.IsNearlyZero() ? 0 : FMath::Clamp(FVector::DotProduct(Center - Start, Delta) / Delta.SizeSquared(), 0.0, 1.0);
    return FVector::DistSquared(Start + Alpha * Delta, Center) <= Radius * Radius;
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

void FAquilaCampaign::Step(double Seconds)
{
    const FVector OldPosition = ShipPosition;
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
    Impacts.RemoveAll([](const FAquilaImpact& Impact) { return Impact.Age > 0.7; });
    if (!bBattle) { AdvanceExpedition(Seconds); return; }
    BattleTime += Seconds;
    const bool bBothAlive = Hull > 0 && EnemyHull > 0;
    const double Range = FVector::Distance(ShipPosition, EnemyPosition);
    if (bBothAlive && Range < WeaponRange)
    {
        for (int32 Turret = 0; Turret < 2; ++Turret)
        {
            TurretClock[Turret] -= Seconds;
            if (TurretClock[Turret] <= 0 && !bHoldFire && (!bArmorBlocked || bPiercing) && Rounds.Num() < MaxRounds)
            {
                const FVector Offset(1600, Turret == 0 ? -1900 : 1900, 800);
                const FVector Muzzle = ShipPosition + FRotator(0, FMath::RadiansToDegrees(Heading), 0).RotateVector(Offset);
                FAquilaRound Round;
                Round.Position = Muzzle;
                Round.Velocity = (EnemyPosition - Muzzle).GetSafeNormal() * 8500;
                Round.bPiercing = bPiercing;
                Round.DamageScale = (Expedition == EAquilaExpedition::Battle ? ExpeditionDamageScale : 1)
                    * (Turret == 0 && PortTurretLevel > 0 ? 1.5 : 1);
                Rounds.Add(Round);
                BurstCount[Turret] = (BurstCount[Turret] + 1) % 4;
                TurretClock[Turret] = BurstCount[Turret] == 0 ? 1.65 : 0.14;
            }
        }
        EnemyClock -= Seconds;
        if (EnemyClock <= 0 && Rounds.Num() < MaxRounds)
        {
            const FVector Velocity = (ShipPosition - OldPosition) / Seconds;
            const FVector Lead = ShipPosition + Velocity * (Range / 7000);
            FAquilaRound Round;
            Round.Position = EnemyPosition;
            Round.Velocity = (Lead - EnemyPosition).GetSafeNormal() * 7000;
            Round.bEnemy = true;
            Round.DamageScale = Expedition == EAquilaExpedition::Battle ? ExpeditionDamageScale : 1;
            Rounds.Add(Round);
            EnemyClock = 1.2;
        }
    }
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
        const FVector RelativeStart = Previous - (Round.bEnemy ? OldPosition : EnemyPosition);
        const bool bPlanetHit = SegmentHitsSphere(Previous, Round.Position, FVector(180000,0,-30000), 60000);
        const bool bHit = !bPlanetHit && TargetHull > 0 && SegmentHitsSphere(RelativeStart, Round.Position - Target, FVector::ZeroVector,
            TargetShield > 0 ? ShieldRadius : HullRadius);
        if (bHit)
        {
            const bool bShieldHit = TargetShield > 0;
            const double Damage = (Round.bEnemy ? 10 : (Round.bPiercing ? 8 : 6)) * Round.DamageScale;
            const double Absorbed = FMath::Min(TargetShield, Damage);
            TargetShield -= Absorbed;
            const double Remainder = Damage - Absorbed;
            const bool bBlocked = Remainder > 0 && !Round.bEnemy && !Round.bPiercing;
            if (bBlocked)
            {
                // A previously launched standard round cannot disable newly fitted piercing ammunition.
                if (!bPiercing) bArmorBlocked = true;
            }
            else TargetHull = FMath::Max(0.0, TargetHull - Remainder * (Round.bEnemy ? 1.0 : 0.75));
            if (Impacts.Num() < 32) Impacts.Add({Round.Position, 0, bShieldHit, bBlocked});
            if (!Round.bEnemy && TargetHull == 0) ++Victories;
        }
        if (bHit || bPlanetHit || Round.Age > 20) Rounds.RemoveAtSwap(Index);
    }
    if (Hull <= 0 || (EnemyHull <= 0 && Expedition != EAquilaExpedition::Salvage)) Order = EAquilaOrder::Hold;
    AdvanceExpedition(Seconds);
}

FString FAquilaCampaign::CombatStatus() const
{
    if (!bBattle) return TEXT("EXPLORATION  /  SYSTEMS READY");
    if (Hull <= 0) return TEXT("RESOLUTE DISABLED  /  RETRY AVAILABLE");
    if (EnemyHull <= 0) return TEXT("TARGET DISABLED  /  SECTOR SECURED");
    if (bArmorBlocked && !bPiercing) return TEXT("ARMOR BLOCKS STANDARD ROUNDS  /  FIT PIERCING");
    if (bHoldFire) return TEXT("HOLD FIRE  /  ROUNDS IN FLIGHT REMAIN LIVE");
    if (FVector::Distance(ShipPosition, EnemyPosition) >= WeaponRange) return TEXT("TARGET OUT OF RANGE");
    return TEXT("AUTOMATIC ENGAGEMENT  /  TWO KINETIC TURRETS");
}
