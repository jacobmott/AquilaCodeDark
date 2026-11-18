#include "AquilaSimulation.h"

namespace
{
constexpr double SurveyRadius = 8000;
constexpr double RecoveryRadius = 6000;
}

bool FAquilaCampaign::ExpeditionActive() const
{
    return Expedition >= EAquilaExpedition::Travel && Expedition <= EAquilaExpedition::Salvage;
}

bool FAquilaCampaign::StartExpedition()
{
    if (ExpeditionActive() || ExpeditionNumber >= MAX_int32 - 1) return false;
    const bool bRetry = Expedition == EAquilaExpedition::Failed;
    if (!bRetry)
    {
        ++ExpeditionNumber;
        // Each destination is outside the planet, in the outward hemisphere from the current ship.
        FRandomStream Random(static_cast<int32>(static_cast<uint32>(WorldSeed) + static_cast<uint32>(ExpeditionNumber) * 7919u));
        FVector Away = (ShipPosition - FVector(180000,0,0)).GetSafeNormal2D();
        if (Away.IsNearlyZero()) Away = FVector(-1,0,0);
        Away = FRotator(0,Random.FRandRange(-20,20),0).RotateVector(Away);
        SignalPosition = ShipPosition + Away * 500000;
        SignalPosition.Z = ShipPosition.Z;
    }
    Expedition = EAquilaExpedition::Travel;
    ExpeditionSeconds = SurveySeconds = RecoverySeconds = 0;
    Shield = Hull = 100;
    bBattle = bArmorBlocked = bHoldFire = false;
    Rounds.Reset();
    Impacts.Reset();
    Order = EAquilaOrder::Signal;
    return true;
}

bool FAquilaCampaign::CanExpeditionAction() const
{
    switch (Expedition)
    {
    case EAquilaExpedition::Travel: return Order != EAquilaOrder::Signal;
    case EAquilaExpedition::Survey: return FVector::Distance(ShipPosition,SignalPosition) > SurveyRadius && Order != EAquilaOrder::Signal;
    case EAquilaExpedition::Battle: return Order != EAquilaOrder::CombatRange || bHoldFire;
    case EAquilaExpedition::Salvage: return Order != EAquilaOrder::Recover && FVector::Distance(ShipPosition,EnemyPosition) > RecoveryRadius;
    default: return ExpeditionNumber < MAX_int32 - 1;
    }
}

bool FAquilaCampaign::ExpeditionAction()
{
    if (!CanExpeditionAction()) return false;
    switch (Expedition)
    {
    case EAquilaExpedition::Available:
    case EAquilaExpedition::Complete:
    case EAquilaExpedition::Failed: return StartExpedition();
    case EAquilaExpedition::Travel:
    case EAquilaExpedition::Survey: Order = EAquilaOrder::Signal; break;
    case EAquilaExpedition::Battle: Order = EAquilaOrder::CombatRange; bHoldFire = false; break;
    case EAquilaExpedition::Salvage: Order = EAquilaOrder::Recover; break;
    }
    return true;
}

bool FAquilaCampaign::CanUpgradePortTurret() const
{
    return PortTurretLevel == 0 && Salvage >= UpgradeCost && !(bBattle && Hull > 0 && EnemyHull > 0);
}

bool FAquilaCampaign::UpgradePortTurret()
{
    if (!CanUpgradePortTurret()) return false;
    Salvage -= UpgradeCost;
    PortTurretLevel = 1;
    return true;
}

void FAquilaCampaign::AdvanceExpedition(double Seconds)
{
    if (!ExpeditionActive()) return;
    ExpeditionSeconds += Seconds;
    if (Hull <= 0)
    {
        Expedition = EAquilaExpedition::Failed;
        Order = EAquilaOrder::Hold;
        return;
    }
    if (Expedition == EAquilaExpedition::Travel && FVector::Distance(ShipPosition,SignalPosition) <= SurveyRadius)
    {
        Expedition = EAquilaExpedition::Survey;
        Order = EAquilaOrder::Hold;
    }
    else if (Expedition == EAquilaExpedition::Survey && FVector::Distance(ShipPosition,SignalPosition) <= SurveyRadius)
    {
        SurveySeconds = FMath::Min(SurveyDuration,SurveySeconds + Seconds);
        if (SurveySeconds >= SurveyDuration)
        {
            Expedition = EAquilaExpedition::Battle;
            // Keep the selected ammunition. Navigation and acquisition are automatic; tactical modes remain player decisions.
            const bool bSelectedPiercing = bPiercing;
            BeginEncounter();
            SetPiercing(bSelectedPiercing);
            Order = EAquilaOrder::CombatRange;
        }
    }
    else if (Expedition == EAquilaExpedition::Battle && EnemyHull <= 0)
    {
        Expedition = EAquilaExpedition::Salvage;
        Order = EAquilaOrder::Hold;
    }
    else if (Expedition == EAquilaExpedition::Salvage && FVector::Distance(ShipPosition,EnemyPosition) <= RecoveryRadius)
    {
        RecoverySeconds = FMath::Min(RecoveryDuration,RecoverySeconds + Seconds);
        if (RecoverySeconds >= RecoveryDuration)
        {
            // Reward and claimed state share one immutable save revision. Reload cannot claim this expedition twice.
            Salvage = static_cast<int32>(FMath::Min(static_cast<int64>(Salvage) + SalvageReward,static_cast<int64>(MAX_int32 - 1)));
            CompletedExpeditions = ExpeditionNumber;
            Expedition = EAquilaExpedition::Complete;
            Order = EAquilaOrder::Hold;
            bBattle = false;
            Rounds.Reset();
        }
    }
}

FString FAquilaCampaign::ExpeditionActionLabel() const
{
    switch (Expedition)
    {
    case EAquilaExpedition::Travel: return Order == EAquilaOrder::Signal ? TEXT("En route") : TEXT("Resume course");
    case EAquilaExpedition::Survey: return FVector::Distance(ShipPosition,SignalPosition) <= SurveyRadius ? TEXT("Surveying") : TEXT("Return to signal");
    case EAquilaExpedition::Battle: return TEXT("Engage target");
    case EAquilaExpedition::Salvage: return FVector::Distance(ShipPosition,EnemyPosition) <= RecoveryRadius ? TEXT("Recovering") : TEXT("Recover salvage");
    case EAquilaExpedition::Complete: return TEXT("Next signal");
    case EAquilaExpedition::Failed: return TEXT("Retry expedition");
    default: return TEXT("Investigate signal");
    }
}

FString FAquilaCampaign::ExpeditionStatus() const
{
    switch (Expedition)
    {
    case EAquilaExpedition::Travel:
        return FString::Printf(TEXT("SIGNAL %02d  |  %.0f km to survey  |  %s"),ExpeditionNumber,
            FVector::Distance(ShipPosition,SignalPosition)/1000,*OrderStatus());
    case EAquilaExpedition::Survey:
        return FString::Printf(TEXT("SURVEY %.0f%%  |  %s"),SurveySeconds/SurveyDuration*100,
            FVector::Distance(ShipPosition,SignalPosition) > SurveyRadius ? TEXT("Return within 8 km to continue") :
            SurveySeconds < 30 ? TEXT("Mapping a lost research beacon") : SurveySeconds < 60 ? TEXT("Reading a salvage signature") : TEXT("Armed contact emerging"));
    case EAquilaExpedition::Battle:
        return FString::Printf(TEXT("SECURE THE SIGNAL  |  %s"),*CombatStatus());
    case EAquilaExpedition::Salvage:
        return FString::Printf(TEXT("RECOVER SALVAGE  |  %.1f km  |  Drones %.0f%%"),
            FVector::Distance(ShipPosition,EnemyPosition)/1000,RecoverySeconds/RecoveryDuration*100);
    case EAquilaExpedition::Complete:
        return FString::Printf(TEXT("EXPEDITION %02d COMPLETE  |  +%d salvage recovered"),ExpeditionNumber,SalvageReward);
    case EAquilaExpedition::Failed: return TEXT("EXPEDITION FAILED  |  Retry restores the ship; earned upgrades stay");
    default: return TEXT("VESPER ECHO  |  A research beacon is calling. Investigate its signal.");
    }
}

FString FAquilaCampaign::UpgradeStatus() const
{
    return FString::Printf(TEXT("Salvage %d  |  Port turret %s  |  Starboard Mk I"),Salvage,
        PortTurretLevel > 0 ? TEXT("Mk II (+50% impact)") : TEXT("Mk I; refit costs 100"));
}

FString FAquilaCampaign::OrderStatus() const
{
    const TCHAR* Names[] = {TEXT("Holding position"),TEXT("Approaching Vesper"),TEXT("Keeping combat range"),
        TEXT("Withdrawing"),TEXT("Following signal"),TEXT("Approaching salvage")};
    return Names[static_cast<int32>(Order)];
}
