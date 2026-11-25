#pragma once

#include "GameFramework/Character.h"
#include "GameFramework/HUD.h"
#include "AquilaBoarding.generated.h"

class AAquilaBoardingBay;
class AAquilaFlightPawn;

UCLASS()
class AAquilaBoardingWalker : public ACharacter
{
    GENERATED_BODY()
public:
    AAquilaBoardingWalker();
    virtual void BeginPlay() override;
    virtual void Tick(float Seconds) override;
    virtual void CalcCamera(float Seconds, FMinimalViewInfo& View) override;
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
    bool IsOutside() const { return bExterior; }
    bool CompletedBoardingRoute() const { return bArrived; }
    bool IsDeltaTree() const;
    int32 HullLengthMetres() const;
    FString LocationName() const;
    FString BoardingHint() const;
    bool AtTactical() const;
    bool IsWatchingBridge() const;
    bool IsRiding() const { return bRiding; }
    bool IsDroneView() const { return bDroneView && bExterior; }
    bool TracksDrone() const;
    int32 ShieldReading() const;
private:
    UPROPERTY() TObjectPtr<AAquilaBoardingBay> Bay;
    UPROPERTY() TObjectPtr<AAquilaFlightPawn> DuelHost;
    void ReturnToDuel();
    void SwitchHullSize();
    void BoardShip(bool bDelta);
    void SwitchShip();
    void SetOrbitFrame(bool bBerth);
    void ToggleExterior();
    void FocusExterior();
    void ResetRamp();
    void StartAtBridgeWindow();
    void ZoomIn();
    void ZoomOut();
    void MainMenu();
    void UseLift();
    void TargetDrone();
    void FireTurret();
    void UpdateLift(double Seconds);
    bool InLift() const;
    bool bManualBridge = false;
    bool bStartAtBridge = false;
    bool bRiding=false, bGoingUp=false, bDroneView=false;
    double LiftProgress=0.;
    FVector RiderOffset=FVector::ZeroVector;
    bool bExterior = false;
    bool bDockFocus = false;
    bool bArrived = false;
    bool bCrossedLock = false;
    bool bCaptured = false;
    double OrbitYaw = 71, OrbitPitch = -7, OrbitRadius = 47000;
};

UCLASS()
class AAquilaBoardingHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};
