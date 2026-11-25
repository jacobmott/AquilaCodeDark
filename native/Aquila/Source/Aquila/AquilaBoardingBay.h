#pragma once

#include "GameFramework/Actor.h"
#include "AquilaSimulation.h"
#include "AquilaBoardingBay.generated.h"

class UInstancedStaticMeshComponent;
class UMaterialInstanceDynamic;
class UStaticMesh;
class UStaticMeshComponent;
class UTextRenderComponent;

UCLASS()
class AAquilaBoardingBay : public AActor
{
    GENERATED_BODY()
public:
    AAquilaBoardingBay();
    virtual void BeginPlay() override;
    virtual void Tick(float Seconds) override;
    static constexpr int32 ReferenceLengthMetres = 640;
    int32 HullLengthMetres() const { return ReferenceLengthMetres * ScaleFactor; }
    int32 HullScale() const { return ScaleFactor; }
    void SelectShip(bool bDelta, bool bManual) { bDeltaTree=bDelta; bManualBridge=bManual; }
    void SelectDuelDeck(bool bGlacier) { bDeltaTree=!bGlacier; bDuelDeck=true; }
    void UpdateDuelInstruments(const FAquilaCampaign& Duel);
    bool IsDeltaTree() const { return bDeltaTree; }
    FVector HullViewCenter() const;
    FBox2D LandingPadMetres() const;
    void SwitchHullSize();
    void SetLift(double Progress, bool bTravelling);
    void ResetTraining();
    void LockDrone();
    void FireTrainingTurret();
    bool DroneLocked() const { return bLocked; }
    int32 DroneShield() const { return FMath::RoundToInt(Training.EnemyShield); }
    uint64 TrainingShots() const { return Training.DuelStats ? Training.DuelStats->NextRoundId : 0; }
    uint64 TrainingHits() const { return Training.LastImpactSerial; }
private:
    friend class FAquilaDuelShipJump;
    enum EFinish { Armor, Trim, Spine, Deck, Inset, Pale, Ceramic, Bronze, Graphite, Amber, Ice, Window, Star, Exhaust, ImpactRail, FinishCount };
    enum EShape { BoxShape, OvalShape, CylinderShape };
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> Materials;
    UPROPERTY() TObjectPtr<UStaticMesh> Cube;
    UPROPERTY() TObjectPtr<UStaticMesh> Sphere;
    UPROPERTY() TObjectPtr<UStaticMesh> Cylinder;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> ExteriorMesh;
    UPROPERTY() TObjectPtr<USceneComponent> HullFrame;
    UPROPERTY() TObjectPtr<USceneComponent> ShuttleFrame;
    UPROPERTY() TObjectPtr<USceneComponent> BerthStructure;
    TMap<TPair<USceneComponent*, int32>, UInstancedStaticMeshComponent*> Batches;
    int32 ScaleFactor = 2;
    bool bDeltaTree = false;
    bool bDuelDeck = false;
    bool bManualBridge = false;
    void Part(FVector Center, FVector Size, EFinish Finish, bool bSolid = true, FRotator Rotation = FRotator::ZeroRotator,
        EShape Shape = BoxShape, USceneComponent* Parent = nullptr);
    void Beam(FVector A, FVector B, double Width, double Height, EFinish Finish, bool bSolid = true, USceneComponent* Parent = nullptr);
    void FitHullToBerth();
    void BuildLandingPad();
    void Lamp(FVector Position, FColor Color, float Lumens, float Reach, USceneComponent* Parent = nullptr);
    UTextRenderComponent* Sign(const TCHAR* Text, FVector Position, double Height, FColor Color, double Yaw = 90,
        USceneComponent* Parent = nullptr);
    void Exterior();
    void GlacierExteriorLights();
    void DeltaTerminal();
    void DeltaBridge();
    void CommandRoom();
    void GlacierCommandRoom();
    void TrainingRange();
    USceneComponent* BridgeFrame(USceneComponent* Parent,FVector Position);
    UPROPERTY() TObjectPtr<USceneComponent> LiftCar;
    UPROPERTY() TObjectPtr<USceneComponent> CarDoor;
    UPROPERTY() TObjectPtr<USceneComponent> LowerGate;
    UPROPERTY() TObjectPtr<USceneComponent> UpperGate;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> DroneShell;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> DroneShieldMaterial;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> Tracers;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> Contacts;
    UPROPERTY() TObjectPtr<UTextRenderComponent> StationReadout;
    FAquilaCampaign Training;
    double RechargeAt = 0.;
    FVector Muzzle;
    bool bLocked=false;
    void Shuttle();
    void Arrivals();
};
