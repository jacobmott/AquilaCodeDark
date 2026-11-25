#include "AquilaBoardingBay.h"
#include "Components/BoxComponent.h"

void AAquilaBoardingBay::GlacierCommandRoom()
{
    const FColor Gold(255,203,130), Blue(170,226,248);
    const FRotator Flat=FRotator::ZeroRotator;
    // Low, wide observation gallery. Its lower skirt intersects the curved roof under the flat walking deck.
    Part({0,0,-1.32},{24.8,30.6,3.0},Spine,true,Flat,CylinderShape);
    Part({0,0,.09},{24.4,30.2,.18},Deck,true,Flat,CylinderShape);
    Part({0,0,5.32},{24.8,30.6,.58},Trim,true,Flat,CylinderShape);
    Part({0,0,5.85},{23.8,29.8,1.25},Armor,false,Flat,OvalShape);
    auto Rim=[](double Angle,double Z,double K=1.) { return FVector(11.8*K*FMath::Cos(Angle),14.7*K*FMath::Sin(Angle),Z); };
    constexpr int32 Bays=16;
    for (int32 I=0; I<Bays; ++I)
    {
        // Center a full pane on the bow, leaving the opponent in view from the default eye.
        const double A=UE_TWO_PI*(I-.5)/Bays, B=UE_TWO_PI*(I+.5)/Bays;
        const bool bWindow=FMath::Cos((A+B)*.5)>-.25;
        const FVector L=Rim(A,2.6), R=Rim(B,2.6);
        if (bWindow)
        {
            Beam(Rim(A,.45),Rim(B,.45),.48,.54,Armor);
            Beam(Rim(A,.76,.986),Rim(B,.76,.986),.56,.10,Trim);
            Beam(Rim(A+.01,.82,.97),Rim(B-.01,.82,.97),.095,.035,ImpactRail,false);
            auto* Glass=NewObject<UBoxComponent>(this);
            Glass->SetupAttachment(RootComponent);
            Glass->InitBoxExtent({13.,(R-L).Size()*50.,235.});
            Glass->SetRelativeLocation((L+R)*50.);
            Glass->SetRelativeRotation(FRotationMatrix::MakeFromYZ(R-L,FVector::UpVector).Rotator());
            Glass->SetCollisionProfileName(TEXT("BlockAll"));
            Glass->SetGenerateOverlapEvents(false);
            Glass->RegisterComponent();
        }
        else Beam(L,R,.45,5.0,Spine);
        Beam(Rim(A,4.94),Rim(B,4.94),.60,.36,Trim);
        Beam(Rim(A,.68),Rim(A,4.85,.98),.25,.28,Trim);
        Beam(Rim(A+.025,4.71,.97),Rim(B-.025,4.71,.97),.095,.055,Amber,false);
        Beam(Rim(A,.20,.73),Rim(B,.20,.73),.085,.025,Trim,false);
    }
    // Paired consoles frame the broad central window. Amber instruments distinguish this bridge at a glance.
    for (double Side : {-1.,1.})
    {
        const FVector Desk(9.10,3.35*Side,0);
        const FRotator Facing(0,Side*13.,0);
        Part(Desk+FVector(0,0,.57),{1.04,2.55,.78},Spine,true,Facing);
        Part(Desk+FVector(0,0,1.01),{1.35,3.42,.19},Trim,true,Facing);
        Part(Desk+FVector(0,0,1.13),{1.03,2.98,.065},Window,false,Facing);
        for (int32 J=-1; J<=1; ++J)
            Part(Desk+Facing.RotateVector(FVector(-.09,.71*J,1.176)),{.32,.53,.02},Amber,false,Facing);
        Lamp(Desk+FVector(0,0,3.95),Gold,540,8.);
    }
    // Put the ship's name inside the sill where it can be read while watching the other ship.
    Part({11.25,-.25,.66},{.17,2.76,.58},Trim);
    Part({11.145,-.25,.66},{.06,2.65,.51},Window,false);
    Sign(TEXT("GLACIER TONGUE"),{11.10,-.25,.76},.20,Gold,180);
    Sign(TEXT("N   SWITCH BRIDGE"),{11.10,-.25,.54},.13,Blue,180);
    Part({-3,0,.48},{3.9,3.9,.60},Trim,true,Flat,CylinderShape);
    Part({-3,0,.84},{3.2,3.2,.14},Window,true,Flat,CylinderShape);
    Sign(TEXT("GLACIER TONGUE   /   COMMAND"),{-11.30,0,2.91},.48,Blue,0);
    Lamp({-.8,0,4.65},Blue,1650,18.);
}
