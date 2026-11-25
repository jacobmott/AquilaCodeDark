#include "AquilaBoardingBay.h"
#include "AquilaBridgeLayout.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

USceneComponent* AAquilaBoardingBay::BridgeFrame(USceneComponent* Parent,FVector Position)
{
    auto* Frame=NewObject<USceneComponent>(this);
    Frame->SetupAttachment(Parent);
    Frame->SetRelativeLocation(Position*100.);
    Frame->RegisterComponent();
    return Frame;
}

void AAquilaBoardingBay::DeltaBridge()
{
    using namespace AquilaBridge;
    const FRotator Flat=FRotator::ZeroRotator;
    const FColor Blue(148,228,255), Gold(255,203,128);
    LiftCar=BridgeFrame(RootComponent,Low);
    CarDoor=BridgeFrame(LiftCar,{-3.38,0,0});
    auto Cabin=[&](FVector P,FVector Size,EFinish Finish,bool bSolid=true)
    { Part(P,Size,Finish,bSolid,Flat,BoxShape,LiftCar); };
    Cabin({0,0,-.17},{7,6.25,.34},Deck);
    Cabin({0,0,4.13},{7,6.35,.3},Graphite);
    Cabin({3.43,0,2},{.22,6.3,4.},Graphite);
    Cabin({3.28,0,2.05},{.10,4.5,1.65},Window,false);
    Sign(TEXT("COMMAND   /   03"),{3.19,0,2.22},.48,Blue,180,LiftCar);
    Sign(TEXT("BRANCH TRANSIT"),{3.19,0,1.67},.22,Gold,180,LiftCar);
    for (double Side : {-1.,1.})
    {
        Cabin({0,3.05*Side,2.},{7,.18,4.},Ceramic);
        Cabin({0,2.92*Side,1.},{6.55,.12,.13},Bronze);
        Cabin({0,2.88*Side,3.35},{6.2,.08,.09},Ice,false);
        Cabin({-3.38,2.32*Side,2},{.23,1.62,4},Graphite);
        Cabin({0,1.48*Side,.017},{6.7,.09,.034},Ice,false);
    }
    Cabin({0,0,3.92},{4.4,1.15,.07},Ice,false);
    Lamp({0,0,3.6},Blue,680,8,LiftCar);
    Part({0,0,1.77},{.18,3.10,3.56},Bronze,true,Flat,BoxShape,CarDoor);
    Part({.11,0,2.19},{.055,1.98,.075},Ice,false,Flat,BoxShape,CarDoor);

    auto Passage=[&](double A,double B,double Z)
    {
        const double Mid=(A+B)*.5, Length=FMath::Abs(A-B);
        Part({Mid,Low.Y,Z-.18},{Length,6.25,.36},Deck);
        Part({Mid,Low.Y,Z+4.25},{Length,6.3,.42},Graphite);
        for (double Side : {-1.,1.})
        {
            Part({Mid,Low.Y+3.03*Side,Z+2.},{Length,.23,4.},Ceramic);
            Part({Mid,Low.Y+2.88*Side,Z+2.8},{Length,.07,.14},Bronze,false);
            Part({Mid,Low.Y+1.44*Side,Z+.02},{Length,.11,.04},Ice,false);
        }
        Lamp({Mid,Low.Y,Z+3.8},Blue,770,15);
    };
    Passage(20.65,Low.X-3.49,Low.Z);
    Passage(High.X-3.49,Center.X+16.3,High.Z);
    for (double Side : {-1.,1.})
        Part(Center+FVector(16.30,Side*3.71,2.7),{.43,1.60,5.4},Bronze);
    Part(Center+FVector(16.30,0,4.76),{.43,6.35,1.39},Graphite);
    auto Gate=[&](FVector Stop)
    {
        auto* Frame=BridgeFrame(RootComponent,Stop-FVector(3.6,0,0));
        Part({0,0,1.8},{.19,6.18,3.65},Graphite,true,Flat,BoxShape,Frame);
        Part({-.12,0,1.38},{.05,3.2,.08},Amber,false,Flat,BoxShape,Frame);
        return Frame;
    };
    LowerGate=Gate(Low);
    UpperGate=Gate(High);
    SetLift(0,false);
    CommandRoom();
}

void AAquilaBoardingBay::CommandRoom()
{
    using namespace AquilaBridge;
    const FRotator Flat=FRotator::ZeroRotator;
    const FColor Blue(148,228,255), Gold(255,203,128);
    if (bDuelDeck)
    {
        // This visit ends at the aft pressure door. The boarding walk opens into its lift passage here.
        Part(Center+FVector(16.18,0,2.6),{.35,8.8,5.2},Ceramic);
        Part(Center+FVector(15.97,0,2.05),{.10,3.4,3.95},Graphite);
        Sign(TEXT("COMMAND DECK"),Center+FVector(15.88,0,2.3),.30,Blue,180);
    }
    // The salon's perimeter follows the hull. Its actual sightline continues across the forward branches.
    Part(Center-FVector(0,0,8.35),{40,40,16},Graphite,true,Flat,OvalShape);
    Part(Center-FVector(0,0,.56),{35.3,35.3,1.12},Bronze,true,Flat,CylinderShape);
    Part(Center-FVector(0,0,.10),{34.7,34.7,.20},Deck,true,Flat,CylinderShape);
    Part(Center+FVector(0,0,5.85),{35.3,35.3,.78},Ceramic,true,Flat,CylinderShape);
    Part(Center+FVector(0,0,7.78),{34.5,34.5,3.4},Bronze,false,Flat,OvalShape);
    auto Rim=[&](double A,double R,double Z) { return Center+FVector(R*FMath::Cos(A),R*FMath::Sin(A),Z); };
    constexpr int32 Bays=24;
    for (int32 I=0; I<Bays; ++I)
    {
        const double A=UE_TWO_PI*I/Bays, B=UE_TWO_PI*(I+1)/Bays;
        if (I!=0 && I!=Bays-1)
        {
            Beam(Rim(A,16.7,.21),Rim(B,16.7,.21),.51,.42,Ceramic);
            Beam(Rim(A,16.7,.46),Rim(B,16.7,.46),.62,.08,Bronze);
            if (bDuelDeck)
                Beam(Rim(A+.012,16.46,.505),Rim(B-.012,16.46,.505),.105,.034,ImpactRail,false);
            auto* Glass=NewObject<UBoxComponent>(this);
            Glass->SetupAttachment(RootComponent);
            const FVector L=Rim(A,16.77,3), R=Rim(B,16.77,3);
            Glass->InitBoxExtent({12.,(R-L).Size()*50.,258.});
            Glass->SetRelativeLocation((L+R)*50.);
            Glass->SetRelativeRotation(FRotationMatrix::MakeFromYZ(R-L,FVector::UpVector).Rotator());
            Glass->SetCollisionProfileName(TEXT("BlockAll"));
            Glass->SetGenerateOverlapEvents(false);
            Glass->RegisterComponent();
        }
        Beam(Rim(A,16.7,5.16),Rim(B,16.7,5.16),.72,.64,Graphite);
        Beam(Rim(A,16.20,4.91),Rim(B,16.20,4.91),.10,.06,Ice,false);
        if (I!=0 && I%2!=0)
            Beam(Rim(A,16.8,.45),Rim(A,16.55,5.),.21,.24,Bronze);
        Beam(Rim(A,12.8,.014),Rim(B,12.8,.014),.10,.028,Bronze,false);
        if (I%4==2)
        {
            Beam(Rim(A,16.2,5.28),Rim(A,4.6,4.87),.48,.39,Graphite,false);
            Beam(Rim(A,14.9,5.01),Rim(A,5.2,4.64),.12,.07,Amber,false);
        }
    }
    Part(Center+FVector(0,0,5.2),{9.8,9.8,.57},Graphite,false,Flat,CylinderShape);
    Part(Center+FVector(0,0,4.88),{7.15,7.15,.12},Ice,false,Flat,CylinderShape);
    Lamp(Center+FVector(0,0,4.6),Blue,2300,25);
    Sign(TEXT("COMMAND DECK   /   03"),Center+FVector(16.2,0,3.85),.58,Gold,0);
    for (double Side : {-1.,1.})
    {
        const FVector SideDesk=Center+Right*Side*7.-Fore*2.2;
        Part(SideDesk+FVector(0,0,.47),{2.8,3.1,.94},Graphite,true,FRotator(0,BowYaw,0));
        Part(SideDesk+FVector(0,0,.99),{2.15,3.24,.14},Bronze,true,FRotator(0,BowYaw,0));
        Part(SideDesk+FVector(0,0,1.08),{.93,2.27,.06},Window,false,FRotator(0,BowYaw,0));
        Part(SideDesk+FVector(0,0,1.12),{.11,1.98,.04},Ice,false,FRotator(0,BowYaw,0));
        Lamp(SideDesk+FVector(0,0,3.9),Gold,780,12);
    }
    // Fit the unit beside the standing position. A crew member can look down over the bow between the rails.
    const FRotator Facing(0,BowYaw+90.,0);
    Part(Console+FVector(0,0,.38),{1.15,3.05,.76},Graphite,true,Facing);
    Part(Console+FVector(0,0,.80),{1.57,4.08,.12},Bronze,true,Facing);
    Part(Console+FVector(0,0,.89),{1.25,3.82,.07},Window,true,Facing);
    for (double V : {-1.57,-1.28,1.28,1.57})
        Part(Console+Fore*V+FVector(0,0,.934),{.57,.12,.025},Ice,false,Facing);
    const FVector Face=Console-Right*.79;
    Part(Face+FVector(0,0,.63),{.08,3.66,.46},Inset,false,Facing);
    StationReadout=Sign(bDuelDeck ? TEXT("DELTA TREE   /   FIRE CONTROL") : TEXT("TACTICAL   /   DRONE 01"),
        Face-Right*.052+FVector(0,0,.65),.20,Blue,BowYaw+270);
    if (bDuelDeck)
        Sign(TEXT("N   OTHER BRIDGE"),Face-Right*.054+FVector(0,0,.45),.12,Gold,BowYaw+270);
    Sign(TEXT("TACTICAL  01"),Console-Fore*2.05+FVector(0,0,.59),.19,Gold,BowYaw+180);
    for (double Side : {-1.,1.})
        Beam(Center+Right*Side*.86+FVector(0,0,.015),Station+Right*Side*.86+FVector(0,0,.015),.065,.03,Ice,false);
}

void AAquilaBoardingBay::UpdateDuelInstruments(const FAquilaCampaign& Duel)
{
    // Reflected light on the window sill identifies a hit on this ship, even when the bow hides the contact.
    const FLinearColor Idle(.008f,.03f,.045f);
    const int32 Latest=Duel.Impacts.FindLastByPredicate([this](const FAquilaImpact& Hit) {
        return Hit.bTargetEnemy==!bDeltaTree && (Hit.bShield || Hit.bHull || Hit.bBlocked) && Hit.Age<.58;
    });
    FLinearColor Light=Idle;
    if (Latest!=INDEX_NONE)
    {
        const auto& Hit=Duel.Impacts[Latest];
        const float Pulse=FMath::Square(1.f-float(Hit.Age/.58));
        const FLinearColor Strike=Hit.bShield ? FLinearColor(.05f,2.8f,4.2f) : FLinearColor(4.f,.8f,.07f);
        Light=FMath::Lerp(Idle,Strike,Pulse);
    }
    Materials[ImpactRail]->SetVectorParameterValue(TEXT("Color"),Light);
}

void AAquilaBoardingBay::SetLift(double Progress,bool bTravelling)
{
    using namespace AquilaBridge;
    if (!LiftCar) return;
    LiftCar->SetRelativeLocation(CarPosition(Progress)*100.);
    CarDoor->SetRelativeLocation(FVector(-3.38,0,bTravelling?0:4.15)*100.);
    LowerGate->SetRelativeLocation((Low+FVector(-3.6,0,!bTravelling && Progress==0 ? 4.15 : 0))*100.);
    UpperGate->SetRelativeLocation((High+FVector(-3.6,0,!bTravelling && Progress==1 ? 4.15 : 0))*100.);
}
