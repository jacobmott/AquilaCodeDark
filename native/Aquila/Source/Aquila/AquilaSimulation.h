#pragma once

#include "CoreMinimal.h"
#include "AquilaWeapons.h"
#include "AquilaDefenses.h"
#include "AquilaAmmunition.h"
#include "AquilaHullLayout.h"

// Simulation positions and velocities are meters and meters/second, independent of rendering and device quality.
enum class EAquilaOrder : uint8 { Hold, Approach, CombatRange, Withdraw, Signal, Recover };
enum class EAquilaExpedition : uint8 { Available, Travel, Survey, Battle, Salvage, Complete, Failed };

struct FAquilaTurret
{
    static constexpr int32 MountCount = AquilaHulls::GunCount;
    int32 Mount = INDEX_NONE;
    EAquilaWeapon Weapon = EAquilaWeapon::TwinTurrets;
    EAquilaAmmo Ammo = EAquilaAmmo::Standard;
    bool bIntercept = false;
    FTransform MountedPose(bool bEnemy) const;
    FVector Muzzle(bool bEnemy,int32 Barrel=0) const;
    static FVector BarrelTip(EAquilaWeapon Type,int32 Barrel);
    static bool IsPlasma(EAquilaWeapon Type) { return Type == EAquilaWeapon::PlasmaCaster || Type == EAquilaWeapon::PulseRepeater; }
    double BaseDamage() const;
    const TCHAR* RoundsName() const;
};

struct FAquilaTurretRack
{
    TArray<FAquilaTurret> Stock; // Stable item indices; an unassigned item stays in this ship's inventory.
    int32 At(int32 Mount) const;
    int32 Installed() const;
    bool Place(int32 Item, int32 Mount);
    bool Fit(int32 Mount, EAquilaWeapon Type);
};

struct FAquilaShipLoadout
{
    EAquilaWeapon Weapon = EAquilaWeapon::TwinTurrets;
    bool bPiercing = false;
    EAquilaArmor Armor = EAquilaArmor::Light;
    bool bShield = true;
    bool bWeave = false;
    TOptional<FAquilaTurretRack> Rack;
    bool bReserveCapacitor = false;
    bool UsesPlasma() const { return FAquilaTurret::IsPlasma(Weapon); }
    int32 StarterCount() const { return Weapon == EAquilaWeapon::TwinTurrets ? 2 : 1; }
    FAquilaTurret Starter(int32 Item) const;
    void PrepareRack();
    void ClearFitting();
};

struct FAquilaDuelLoadout
{
    FAquilaShipLoadout Resolute;
    FAquilaShipLoadout Sentinel{EAquilaWeapon::KineticCannon, false, EAquilaArmor::Reinforced, true};
    FAquilaShipLoadout& ForShip(bool bEnemy) { return bEnemy ? Sentinel : Resolute; }
    const FAquilaShipLoadout& ForShip(bool bEnemy) const { return bEnemy ? Sentinel : Resolute; }
};

struct FAquilaShotTally
{
    int32 Hits = 0;
    int32 Blocked = 0;
    int32 Misses = 0;
    int32 Intercepted = 0;
};

struct FAquilaScreenShot
{
    uint64 TargetId;
    bool bEnemy;
    FVector Origin;
    FVector Position;
    double Time = 0;
    double Duration;
};

struct FAquilaInterceptFlash
{
    FVector Position;
    double Age = 0;
};

struct FAquilaDuelStats
{
    FVector Axis;
    FVector Sideways;
    FAquilaShotTally Shots[2];
    bool bReserveSpent[2] = {};
    uint64 NextRoundId = 0;
    TArray<FAquilaScreenShot> ScreenShots;
    TArray<FAquilaInterceptFlash> Flashes;
};

struct FAquilaFiringCycle
{
    double Clock[FAquilaTurret::MountCount] = {0.4, 0.65};
    int32 Burst[FAquilaTurret::MountCount] = {};
    bool Blocked[FAquilaTurret::MountCount] = {};
};

struct FAquilaRound
{
    FVector Position = FVector::ZeroVector;
    FVector Velocity = FVector::ZeroVector;
    double Age = 0;
    bool bEnemy = false;
    EAquilaAmmo Ammo = EAquilaAmmo::Standard;
    bool HitsShield(double Charge) const { return Charge > 0 && Ammo != EAquilaAmmo::Phase; }
    double DamageScale = 1; // Captured at launch; upgrades never change a round already flying.
    int32 Turret = INDEX_NONE;
    EAquilaWeapon Weapon = EAquilaWeapon::TwinTurrets;
    uint64 Id = 0;
    bool bClaimed = false;
};

struct FAquilaImpact
{
    // Contact and incoming direction in the struck ship's frame, in simulation meters.
    FVector LocalContact = FVector::ZeroVector;
    FVector LocalDirection = FVector::ForwardVector;
    uint64 Serial = 0;
    double Age = 0;
    bool bTargetEnemy = false;
    bool bShield = false;
    bool bBlocked = false;
    bool bHull = false;
    bool bDestroyed = false;
    uint64 RoundId = 0;
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
    static constexpr double HullRadius = 2200;
    static constexpr double ShieldRadius = 3000;
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
    FAquilaDuelLoadout Loadout;
    TOptional<FAquilaDuelStats> DuelStats; // Present only in the unsaved simulator.
    FAquilaFiringCycle Guns[2] = {{}, {{1.5, 0.65}, {}}};
    EAquilaOrder Order = EAquilaOrder::Hold;
    bool bBattle = false;
    bool bHoldFire = false;
    bool bArmorBlocked = false;
    bool bEnemyArmorBlocked = false;
    TArray<FAquilaRound> Rounds;
    TArray<FAquilaImpact> Impacts; // Cosmetic; never persisted or used for damage.
    uint64 LastImpactSerial = 0;
    uint32 EncounterGeneration = 0;

    bool StartPractice();
    void BeginEncounter();
    void BeginDuel(const FAquilaDuelLoadout& Fitting);
    void EditDuelShip(bool bEnemy, TFunctionRef<void(FAquilaShipLoadout&)> Edit);
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
    void FireBattery(bool bEnemy, const FVector& TargetVelocity, double Range, double Seconds);
    void AdvanceScreenShots(double Seconds);
    void SetPiercing(bool bEnabled);
    FString CombatStatus() const;
    static bool SegmentHitsSphere(const FVector& Start, const FVector& End, const FVector& Center, double Radius, FVector* Entry = nullptr);
};
