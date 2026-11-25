#include "AquilaBoardingBay.h"

void AAquilaBoardingBay::DeltaTerminal()
{
    const FRotator Flat=FRotator::ZeroRotator;
    const FColor Gold(255,203,134), Blue(153,223,247);
    auto DeckLight = [this](FVector A, FVector B, EFinish Finish, double Width=.12)
    {
        Beam(A,B,Width,.035,Finish,false);
    };

    // Peregrine parks outside the pressure shell, as shown in the passenger terminal section.
    Part({0,51,-2.7},{91,119,3.0},Graphite,true,Flat,CylinderShape);
    Part({0,51,-.72},{92,120,1.44},Ceramic,true,Flat,CylinderShape);
    for (double Side : {-1.,1.})
    {
        Beam({Side*10,8,-3.},{Side*33,77,-2.9},2.,3.4,Bronze);
        for (int I=0; I<18; ++I)
        {
            const double A=PI*I/18., B=PI*(I+.64)/18.;
            DeckLight({Side*44.6*FMath::Sin(A),51+58.5*FMath::Cos(A),.035},
                      {Side*44.6*FMath::Sin(B),51+58.5*FMath::Cos(B),.035},Amber,.22);
        }
        for (double Y=18; Y<=91; Y+=8)
            Part({Side*34,Y,.015},{1.25,.14,.03},Graphite,false);
        DeckLight({Side*2.3,15,.026},{Side*2.3,8,.026},Amber);
    }

    // Replace the study's outermost solid island with an occupied rotunda on that same foundation.
    constexpr double CenterY=-17., Radius=26.;
    Part({0,CenterY,-4.35},{56.4,56.4,7.0},Graphite,true,Flat,CylinderShape);
    Part({0,CenterY,-.65},{55.6,55.6,1.3},Bronze,true,Flat,CylinderShape);
    Part({0,CenterY,-.06},{54.8,54.8,.12},Ceramic,true,Flat,CylinderShape);
    Part({0,CenterY,7.15},{55.3,55.3,1.15},Graphite,true,Flat,CylinderShape);
    Part({0,CenterY,7.77},{56.,56.,.47},Bronze,false,Flat,CylinderShape);
    Part({0,CenterY,8.25},{54.2,54.2,7.1},Ceramic,true,Flat,OvalShape);
    auto Circle = [](double Angle, double R, double Z)
    {
        return FVector(R*FMath::Sin(Angle),-17.+R*FMath::Cos(Angle),Z);
    };
    for (int I=0; I<32; ++I)
    {
        const double A=2.*PI*I/32., B=2.*PI*(I+1)/32.;
        const bool bPortal = I==0 || I==15 || I==16 || I==31;
        if (!bPortal)
        {
            Beam(Circle(A,Radius,1.),Circle(B,Radius,1.),.85,2.,Ceramic);
            Beam(Circle(A,Radius,3.35),Circle(B,Radius,3.35),.55,2.75,Window);
            Beam(Circle(A,Radius,5.75),Circle(B,Radius,5.75),.85,2.1,Ceramic);
            Part(Circle(A,Radius,3.25),{.55,.58,6.5},Bronze);
            Beam(Circle(A,25.55,4.85),Circle(B,25.55,4.85),.16,.10,Amber,false);
        }
        DeckLight(Circle(A,22.3,.016),Circle(B,22.3,.016),Bronze,.22);
        if (I%4==2)
        {
            Beam(Circle(A,23.8,6.4),Circle(A,6.,5.05),.48,.45,Bronze,false);
            Beam(Circle(A,22.8,6.11),Circle(A,7.,4.86),.12,.055,Amber,false);
        }
    }
    for (double Side : {-1.,1.})
    {
        Part({Side*4.0,8.08,2.37},{2.5,1.1,4.74},Bronze);
        Part({Side*2.80,8.71,2.13},{.14,.14,4.26},Amber,false);
        for (double Y : {-9.,-17.,-25.})
        {
            Part({Side*17.8,Y,.54},{2.35,4.,1.08},Graphite);
            Part({Side*17.8,Y,1.12},{2.6,4.2,.14},Bronze);
        }
        DeckLight({Side*2.25,8,.025},{Side*2.25,-35,.025},Amber);
    }
    Part({0,8.08,5.54},{10.55,1.1,2.32},Ceramic);
    Part({0,8.71,5.29},{9.75,.13,1.21},Graphite,false);
    Sign(TEXT("DELTA TREE  /  ARRIVALS 06"),{0,8.81,5.28},.46,Gold);
    Lamp({7.,12.,4.6},Gold,1550,26);
    for (double X : {-10.,10.}) Lamp({X,-17,4.45},Gold,2300,30);

    // Twin jambs and lintels provide real 2.8 by 3 m crew openings.
    Part({0,-52,-.38},{8.45,31.,.76},Ceramic);
    for (double Y : {-39.4,-45.4})
    {
        for (double Side : {-1.,1.})
        {
            Part({Side*2.83,Y,2.65},{2.86,.76,5.3},Bronze);
            Part({Side*1.47,Y+.43,1.5},{.11,.095,3.},Amber,false);
        }
        Part({0,Y,4.13},{3.05,.76,2.26},Bronze);
        Part({0,Y+.43,3.9},{3.85,.10,.85},Graphite,false);
    }
    Sign(TEXT("BRANCH AIRLOCK"),{0,-38.87,3.9},.34,Gold);
    Sign(TEXT("BRANCH  01"),{0,-44.87,3.9},.37,Blue);
    for (double Side : {-1.,1.})
    {
        Part({Side*3.84,-53.0,2.75},{.55,27.7,5.5},Graphite);
        for (double Y=-41.2; Y>=-64.; Y-=3.75)
        {
            Part({Side*3.53,Y,2.58},{.16,3.25,3.45},Ceramic);
            Part({Side*3.41,Y,3.01},{.12,2.45,1.30},Window,false);
            Part({Side*3.33,Y,3.87},{.07,2.85,.08},Ice,false);
        }
        DeckLight({Side*1.08,-37,.025},{Side*1.08,-64,.025},Ice);
    }
    Part({0,-53.0,5.65},{8.15,27.8,.48},Graphite);
    for (double Y : {-42.5,-51.,-59.})
    {
        Part({0,Y,5.32},{3.85,.28,.085},Ice,false);
        Lamp({0,Y,4.75},Blue,950,15);
    }

    // Each side of the first junction can be walked up to its destination doors.
    Part({0,-72,-.35},{42.,14.8,.7},Ceramic);
    Part({0,-72,6.45},{42.,14.8,.65},Graphite);
    Part({0,-79.1,3.15},{42.,.75,6.3},Ceramic);
    for (double Side : {-1.,1.})
    {
        Part({Side*12.6,-64.85,3.15},{16.8,.55,6.3},Bronze);
        if (Side<0)
        {
            Part({-20.65,-72,3.15},{.7,14.8,6.3},Graphite);
            Part({-20.19,-72.6,2.15},{.22,3.65,4.3},Bronze);
            for (double Leaf : {-1.,1.})
                Part({-20.03,-72.6+Leaf*.82,1.67},{.12,1.58,3.34},Ceramic);
        }
        else
        {
            Part({20.65,-76.95,3.15},{.7,4.9,6.3},Graphite);
            Part({20.65,-67.65,3.15},{.7,6.1,6.3},Graphite);
            Part({20.65,-72.6,4.85},{.7,3.8,2.9},Bronze);
            Sign(TEXT("COMMAND LIFT"),{20.23,-72.6,4.13},.41,Blue,180);
        }
        const EFinish Guide=Side<0?Amber:Ice;
        DeckLight({Side*1.08,-64,.027},{Side*7.8,-72.6,.027},Guide,.19);
        DeckLight({Side*7.8,-72.6,.027},{Side*19.4,-72.6,.027},Guide,.19);
        Part({Side*3.85,-78.60,3.1},{6.65,.15,2.05},Graphite);
        Sign(Side<0?TEXT("RESEARCH"):TEXT("COMMAND"),{Side*3.85,-78.46,3.32},.67,Side<0?Gold:Blue);
        Part({Side*3.85,-78.48,2.40},{4.8,.10,.07},Guide,false);
        Lamp({Side*10.,-72,4.75},Side<0?Gold:Blue,1900,21);
    }
    Part({0,-78.64,4.95},{6.8,.16,1.05},Graphite);
    Sign(TEXT("JUNCTION  01"),{0,-78.48,4.94},.53,Gold);
}
