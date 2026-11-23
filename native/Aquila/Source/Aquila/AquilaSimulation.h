#pragma once

#include "CoreMinimal.h"

// Simulation positions and velocities are meters and meters/second, independent of rendering and device quality.
enum class EAquilaOrder : uint8 { Hold, Approach, CombatRange, Withdraw, Signal, Recover };
enum class EAquilaExpedition : uint8 { Available, Travel, Survey, Battle, Salvage, Complete, Failed };

struct FAquilaRound
{
    FVector Position = FVector::ZeroVector;
    FVector Velocity = FVector::ZeroVector;
    double Age = 0;
    bool bEnemy = false;
    bool bPiercing = false;
    double DamageScale = 1; // Captured at launch; upgrades never change a round already flying.
};

struct FAquilaImpact
{
    FVector Position = FVector::ZeroVector;
    double Age = 0;
    bool bShield = false;
    bool bBlocked = false;
};

struct FAquilaCampaign
{
    static constexpr int32 SchemaVersion = 3;
    static constexpr int32 GenerationVersion = 1;
    static constexpr int32 MaxRounds = 160;
    static constexpr int32 SalvageReward = 120;
    static constexpr int32 UpgradeCost = 100;
    static constexpr double SurveyDuration = 90;
    static constexpr double RecoveryDuration = 45;
    static constexpr double ExpeditionDamageScale = 0.035;
    FString CampaignId = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
    FString RevisionId;
    FString ParentRevisionId;
    int32 Sequence = 0;
    int32 WorldSeed = 7142;
    int32 Victories = 0;
    int32 ExpeditionNumber = 0;
    int32 CompletedExpeditions = 0;
    int32 Salvage = 0;
    int32 PortTurretLevel = 0;
    EAquilaExpedition Expedition = EAquilaExpedition::Available;
    FVector SignalPosition = FVector::ZeroVector;
    double ExpeditionSeconds = 0;
    double SurveySeconds = 0;
    double RecoverySeconds = 0;
    FVector ShipPosition = FVector::ZeroVector;
    FVector EnemyPosition = FVector(35000, 12000, 0);
    double Heading = 0;
    double Shield = 100;
    double Hull = 100;
    double EnemyShield = 100;
    double EnemyHull = 100;
    double BattleTime = 0;
    double StepRemainder = 0;
    double TurretClock[2] = {0.4, 0.65};
    int32 BurstCount[2] = {0, 0};
    double EnemyClock = 1.5;
    EAquilaOrder Order = EAquilaOrder::Hold;
    bool bBattle = false;
    bool bHoldFire = false;
    bool bPiercing = false;
    bool bArmorBlocked = false;
    TArray<FAquilaRound> Rounds;
    TArray<FAquilaImpact> Impacts; // Cosmetic; never persisted or used for damage.

    bool StartPractice();
    void BeginEncounter();
    void BeginExpeditionBattle();
    bool StartExpedition();
    bool ExpeditionAction();
    bool UpgradePortTurret();
    bool ExpeditionActive() const;
    bool CanExpeditionAction() const;
    bool CanUpgradePortTurret() const;
    void AdvanceExpedition(double Seconds);
    FString ExpeditionActionLabel() const;
    FString ExpeditionStatus() const;
    FString UpgradeStatus() const;
    FString OrderStatus() const;
    void Advance(double Seconds);
    void Step(double Seconds);
    void SetPiercing(bool bEnabled);
    FString CombatStatus() const;
    static bool SegmentHitsSphere(const FVector& Start, const FVector& End, const FVector& Center, double Radius);
};
