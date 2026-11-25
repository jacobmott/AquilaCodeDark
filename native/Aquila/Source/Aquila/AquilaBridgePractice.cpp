#include "AquilaBoardingBay.h"
#include "AquilaBridgeLayout.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"

void AAquilaBoardingBay::TrainingRange()
{
    using namespace AquilaBridge;
    const auto Facing=(Drone-Turret).Rotation();
    Part(Turret-FVector(0,0,3.5),{11.6,11.6,7.1},Graphite,true,FRotator::ZeroRotator,CylinderShape);
    Part(Turret,{8.5,8.5,.45},Bronze,true,FRotator::ZeroRotator,CylinderShape);
    auto* Cannon=NewObject<UStaticMeshComponent>(this,TEXT("BridgeKineticCannon"));
    Cannon->SetupAttachment(RootComponent);
    const auto& Model=AquilaWeapons::Models[int32(EAquilaWeapon::KineticCannon)];
    const auto Path=FString::Printf(TEXT("/Game/Foundation/%s.%s"),Model.Asset,Model.Asset);
    Cannon->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,*Path));
    Cannon->SetRelativeTransform(FTransform(Facing,Turret*100.,FVector(7.4)));
    Cannon->SetMaterial(0,Materials[Armor]);
    Cannon->SetMaterial(1,Materials[Bronze]);
    Cannon->SetMaterial(2,Materials[Amber]);
    Cannon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Cannon->RegisterComponent();
    Muzzle=Cannon->GetComponentTransform().TransformPosition({69,0,21})/100.;

    // One physical target. The bridge and exterior both look at these components in the same world.
    auto* Body=BridgeFrame(RootComponent,Drone);
    Body->SetRelativeRotation(FRotator(0,BowYaw+180,0));
    auto DronePart=[&](FVector P,FVector Size,EFinish Finish,EShape Shape)
    { Part(P,Size,Finish,false,FRotator::ZeroRotator,Shape,Body); };
    DronePart({0,0,0},{12.,5.1,5.3},Graphite,OvalShape);
    DronePart({1.4,0,1.4},{8.4,6.3,2.8},Pale,OvalShape);
    DronePart({5.6,0,0},{1.1,4.2,2.6},Bronze,OvalShape);
    DronePart({6.1,0,.15},{.75,2.15,1.5},Ice,OvalShape);
    for (double Side : {-1.,1.})
    {
        Beam({0,Side*1.7,0},{-1.8,Side*7.1,1.1},1.5,1.1,Bronze,false,Body);
        DronePart({-1.5,Side*6.9,1.},{6.5,2.1,2.1},Ceramic,OvalShape);
        DronePart({1.5,Side*6.9,1.},{.65,1.5,1.4},Amber,OvalShape);
        DronePart({-3.1,Side*2.8,-2.},{3.,2.2,2.5},Bronze,OvalShape);
    }
    Lamp(Drone-Fore*8+FVector(0,0,4),FColor(224,239,255),1700,26);
    DroneShell=NewObject<UStaticMeshComponent>(this,TEXT("TrainingDroneShield"));
    DroneShell->SetupAttachment(RootComponent);
    DroneShell->SetStaticMesh(Sphere);
    DroneShell->SetRelativeTransform(FTransform(FQuat::Identity,Drone*100.,FVector(ShieldRadius*2)));
    DroneShell->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DroneShell->SetCastShadow(false);
    auto* Material=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Foundation/M_ShieldContact.M_ShieldContact"));
    DroneShieldMaterial=UMaterialInstanceDynamic::Create(Material,this);
    DroneShieldMaterial->SetVectorParameterValue(TEXT("Axes"),FLinearColor::White);
    DroneShieldMaterial->SetScalarParameterValue(TEXT("IdleGain"),5.f);
    DroneShieldMaterial->SetScalarParameterValue(TEXT("ShellStrength"),1.f);
    DroneShell->SetMaterial(0,DroneShieldMaterial);
    DroneShell->RegisterComponent();
    auto Marks=[this](const TCHAR* Name,EFinish Finish)
    {
        auto* Batch=NewObject<UInstancedStaticMeshComponent>(this,FName(Name));
        Batch->SetupAttachment(RootComponent);
        Batch->SetStaticMesh(Sphere);
        Batch->SetMaterial(0,Materials[Finish]);
        Batch->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Batch->SetCastShadow(false);
        Batch->RegisterComponent();
        return Batch;
    };
    Tracers=Marks(TEXT("BridgeRoundTrails"),Amber);
    Contacts=Marks(TEXT("BridgeShieldContacts"),Ice);
    ResetTraining();
}

void AAquilaBoardingBay::ResetTraining()
{
    using namespace AquilaBridge;
    if (!DroneShell) return;
    Training=FAquilaCampaign();
    Training.Loadout.Resolute.Rack.Emplace().Stock.Add({Mount,EAquilaWeapon::KineticCannon});
    Training.Loadout.Sentinel.Rack.Emplace();
    Training.StartPractice();
    Training.EnemyPosition=Training.ShipPosition+Training.Loadout.Resolute.Rack->Stock[0].Muzzle(false)*10.+FVector(RangeMetres,0,0);
    auto& Stats=Training.DuelStats.Emplace();
    Stats.Axis=FVector::ForwardVector;
    Stats.Sideways=FVector::RightVector;
    Training.bHoldFire=bManualBridge;
    Training.Guns[0].Clock[0]=0.;
    bLocked=false;
    RechargeAt=0.;
    StationReadout->SetText(FText::FromString(bManualBridge ? TEXT("TACTICAL   /   ACQUIRE  01") : TEXT("01   /   SHIELD  100%")));
}

void AAquilaBoardingBay::LockDrone()
{
    if (!bManualBridge) return;
    if (DroneShield()==0) ResetTraining();
    bLocked=true;
    StationReadout->SetText(FText::FromString(FString::Printf(TEXT("01   /   SHIELD  %d%%"),DroneShield())));
}

void AAquilaBoardingBay::FireTrainingTurret()
{
    if (!bManualBridge || !bLocked || Training.EnemyShield<=0) return;
    Training.bHoldFire=false;
    Training.FireBattery(false,FVector::ZeroVector,(Training.EnemyPosition-Training.ShipPosition).Size(),0.);
    Training.bHoldFire=true;
}

void AAquilaBoardingBay::Tick(float Seconds)
{
    using namespace AquilaBridge;
    Super::Tick(Seconds);
    if (!DroneShell) return;
    const int32 Previous=DroneShield();
    if (!bManualBridge || bLocked) Training.Advance(Seconds);
    if (!bManualBridge)
    {
        if (Previous>0 && DroneShield()==0)
        {
            Training.bHoldFire=true;
            RechargeAt=Training.BattleTime+2.;
        }
        else if (RechargeAt>0 && Training.BattleTime>=RechargeAt)
        {
            // Recharge this practice target in place. Airborne shots and both cameras share its history.
            Training.EnemyShield=100.;
            Training.Guns[0].Blocked[0]=false;
            Training.bArmorBlocked=false;
            Training.bHoldFire=false;
            RechargeAt=0.;
        }
    }
    if (DroneShield()!=Previous)
        StationReadout->SetText(FText::FromString(FString::Printf(TEXT("01   /   SHIELD  %d%%"),DroneShield())));
    FVector Eye=Center*100.;
    FRotator ViewRotation;
    if (auto* PC=GetWorld()->GetFirstPlayerController()) PC->GetPlayerViewPoint(Eye,ViewRotation);
    const FVector Camera=(Eye/100.-Drone)/ShieldRadius;
    DroneShieldMaterial->SetVectorParameterValue(TEXT("Eye"),FLinearColor(Camera.X,Camera.Y,Camera.Z));
    const FVector Inbound=(Drone-Muzzle).GetSafeNormal();
    const FVector Contact=Drone-Inbound*ShieldRadius;
    auto Stroke=[](UInstancedStaticMeshComponent* Marks,FVector A,FVector B,double Width)
    {
        const FVector Axis=B-A;
        Marks->AddInstance(FTransform(Axis.Rotation(),(A+B)*50.,FVector(Axis.Size(),Width,Width)));
    };
    if (Tracers->GetInstanceCount()) Tracers->ClearInstances();
    if (Contacts->GetInstanceCount()) Contacts->ClearInstances();
    for (const auto& Round : Training.Rounds)
    {
        const double Remaining=Training.EnemyPosition.X-Round.Position.X;
        const double T=FMath::Clamp((RangeMetres-Remaining)/(RangeMetres-Training.ShieldRadius),0.,1.);
        const FVector Tip=FMath::Lerp(Muzzle,Contact,T);
        Stroke(Tracers,Tip-Inbound*FMath::Min(5.,(Tip-Muzzle).Size()),Tip,.56);
        if (Round.Age<.15) Stroke(Tracers,Muzzle,Muzzle+Inbound*2.9,1.3*(1.-Round.Age/.15));
    }
    FVector Wave=FVector::ZeroVector;
    constexpr double FlashSeconds=.95;
    for (const auto& Impact : Training.Impacts)
    {
        if (!Impact.bShield || Impact.Age>=FlashSeconds) continue;
        const double T=Impact.Age/FlashSeconds;
        Wave=-Inbound*(2.-T);
        const FVector Tangent=FVector::CrossProduct(Inbound,FVector::UpVector).GetSafeNormal();
        const FVector Up=FVector::CrossProduct(Tangent,Inbound);
        for (int32 I=0; I<12; ++I)
        {
            const double A=UE_TWO_PI*I/12.+Impact.Serial*.61;
            const FVector Ray=Tangent*FMath::Cos(A)+Up*FMath::Sin(A);
            const FVector P=Contact+Ray*(.7+7.9*T)+Inbound*(2.*T*T);
            Stroke(Contacts,P,P+Ray*(1.4-.95*T),.50-.38*T);
        }
    }
    DroneShell->SetVisibility(Training.EnemyShield>0 || !Wave.IsZero());
    DroneShieldMaterial->SetVectorParameterValue(TEXT("HitA"),FLinearColor(Wave.X,Wave.Y,Wave.Z));
}
