#include "AquilaBoardingBay.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

AAquilaBoardingBay::AAquilaBoardingBay()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DockDatum"));
    HullFrame = CreateDefaultSubobject<USceneComponent>(TEXT("HullFrame"));
    HullFrame->SetupAttachment(RootComponent);
    ShuttleFrame = CreateDefaultSubobject<USceneComponent>(TEXT("Peregrine"));
    ShuttleFrame->SetupAttachment(RootComponent);
    BerthStructure = CreateDefaultSubobject<USceneComponent>(TEXT("BerthStructure"));
    BerthStructure->SetupAttachment(RootComponent);
}

void AAquilaBoardingBay::BeginPlay()
{
    Super::BeginPlay();
    Cube = LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
    Sphere = LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    Cylinder = LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    auto* Glow = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Foundation/M_Glow.M_Glow"));
    const FLinearColor Colors[] = {{.40f,.53f,.62f},{.14f,.23f,.29f},{.075f,.13f,.17f},{.09f,.135f,.155f},{.028f,.046f,.059f},
        {.50f,.59f,.59f},{.69f,.58f,.405f},{.53f,.285f,.125f},{.125f,.105f,.086f},
        {1,.43f,.09f},{.28f,.81f,1},{.014f,.058f,.095f},{.58f,.77f,.91f},{.96f,.51f,.185f},{.008f,.03f,.045f}};
    const FName RoomSurfaces[]={TEXT("Armor"),TEXT("Trim"),TEXT("Spine"),TEXT("Hardware"),TEXT("Spine"),
        TEXT("Island"),TEXT("Island"),TEXT("Trim"),TEXT("Hardware")};
    static_assert(UE_ARRAY_COUNT(RoomSurfaces)==Amber);
    for (int I = 0; I < FinishCount; ++I)
    {
        auto* Base=I>=Amber ? Glow : AquilaHulls::Surface(!bDeltaTree,RoomSurfaces[I]);
        auto* M = UMaterialInstanceDynamic::Create(Base, this);
        M->SetVectorParameterValue(TEXT("Color"), I>=Amber ? Colors[I] : FMath::Lerp(Colors[I],FLinearColor::White,.45f));
        if (I >= Amber) M->SetScalarParameterValue(TEXT("Glow"), I == Window ? .65f : 2.f);
        Materials.Add(M);
    }
    if (bDuelDeck)
    {
        if (bDeltaTree) CommandRoom();
        else GlacierCommandRoom();
        return;
    }
    for (int I = 0; I < 2; ++I)
    {
        auto* Sun = NewObject<UDirectionalLightComponent>(this);
        Sun->SetupAttachment(RootComponent);
        Sun->SetMobility(EComponentMobility::Movable);
        Sun->SetRelativeRotation(I == 0 ? FRotator(-35,-115,0) : FRotator(18,48,0));
        Sun->SetIntensity(I == 0 ? 2.8f : .72f);
        Sun->SetLightColor(I == 0 ? FLinearColor(1,.94f,.84f) : FLinearColor(.48f,.72f,1));
        Sun->SetCastShadows(I == 0);
        Sun->RegisterComponent();
    }
    Exterior();
    Shuttle();
    if (bDeltaTree)
    {
        DeltaTerminal();
        DeltaBridge();
        TrainingRange();
        SetActorTickEnabled(true);
    }
    else
    {
        Arrivals();
        FitHullToBerth();
    }
}

// Layout lengths are metres. Unreal static shapes have one-metre source bounds.
void AAquilaBoardingBay::Part(FVector Center, FVector Size, EFinish Finish, bool bSolid, FRotator Rotation, EShape Shape, USceneComponent* Parent)
{
    if (!Parent) Parent = RootComponent.Get();
    const int32 Key = (Finish*3 + Shape)*2 + int32(bSolid);
    auto*& Batch = Batches.FindOrAdd(TPair<USceneComponent*, int32>(Parent, Key));
    if (!Batch)
    {
        Batch = NewObject<UInstancedStaticMeshComponent>(this);
        Batch->SetupAttachment(Parent);
        Batch->SetMobility(EComponentMobility::Movable);
        UStaticMesh* Shapes[] = {Cube.Get(),Sphere.Get(),Cylinder.Get()};
        Batch->SetStaticMesh(Shapes[Shape]);
        Batch->SetMaterial(0,Materials[Finish]);
        Batch->SetCollisionProfileName(bSolid ? TEXT("BlockAll") : TEXT("NoCollision"));
        Batch->SetGenerateOverlapEvents(false);
        Batch->SetCastShadow(Finish < Amber);
        if (Finish < Amber && Shape == BoxShape) Batch->SetNumCustomDataFloats(3);
        Batch->RegisterComponent();
    }
    const int32 Index = Batch->AddInstance(FTransform(Rotation,Center*100,Size));
    if (Finish < Amber && Shape == BoxShape)
    {
        // Eight metres per panel repeat on floors, walls and structural beams.
        const float Span[] = {float(Size.X/8.),float(Size.Y/8.),float(Size.Z/8.)};
        Batch->SetCustomData(Index,MakeArrayView(Span),true);
    }
}

void AAquilaBoardingBay::Beam(FVector A, FVector B, double Width, double Height, EFinish Finish, bool bSolid, USceneComponent* Parent)
{
    const FVector Delta = B-A;
    Part((A+B)*.5, FVector(Width,Delta.Size(),Height), Finish, bSolid,
        FRotationMatrix::MakeFromYZ(Delta, FVector::UpVector).Rotator(),BoxShape,Parent);
}

void AAquilaBoardingBay::Lamp(FVector Position, FColor Color, float Lumens, float Reach, USceneComponent* Parent)
{
    auto* Light = NewObject<UPointLightComponent>(this);
    Light->SetupAttachment(Parent ? Parent : RootComponent.Get());
    Light->SetMobility(EComponentMobility::Movable);
    Light->SetRelativeLocation(Position*100);
    Light->SetLightColor(FLinearColor(Color));
    Light->SetIntensityUnits(ELightUnits::Lumens);
    Light->SetIntensity(Lumens);
    Light->SetAttenuationRadius(Reach*100);
    Light->SetCastShadows(false);
    Light->RegisterComponent();
}

UTextRenderComponent* AAquilaBoardingBay::Sign(const TCHAR* Text, FVector Position, double Height, FColor Color, double Yaw,USceneComponent* Parent)
{
    auto* Label = NewObject<UTextRenderComponent>(this);
    Label->SetupAttachment(Parent ? Parent : RootComponent.Get());
    Label->SetRelativeLocation(Position*100);
    Label->SetRelativeRotation(FRotator(0,Yaw,0));
    Label->SetText(FText::FromString(Text));
    Label->SetHorizontalAlignment(EHTA_Center);
    Label->SetVerticalAlignment(EVRTA_TextCenter);
    Label->SetWorldSize(Height*100);
    Label->SetTextRenderColor(Color);
    Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Label->SetCastShadow(false);
    Label->RegisterComponent();
    return Label;
}

void AAquilaBoardingBay::Exterior()
{
    auto* Hull = NewObject<UStaticMeshComponent>(this,bDeltaTree ? TEXT("DeltaTreeStudy") : TEXT("GlacierTongueStudy"));
    ExteriorMesh = Hull;
    Hull->SetupAttachment(HullFrame);
    Hull->SetMobility(EComponentMobility::Movable);
    Hull->SetStaticMesh(AquilaHulls::LoadMesh(!bDeltaTree));
    Hull->SetCollisionProfileName(TEXT("BlockAll"));
    if (bDeltaTree)
    {
        // The Blender socket gives the island centre and the branch direction. The shuttle's common boarding datum stays fixed.
        const FTransform Dock = Hull->GetSocketTransform(TEXT("BoardingDock"),RTS_Component);
        Hull->SetRelativeTransform(Dock.Inverse()*FTransform(FVector(0,-1700,0)));
    }
    Hull->RegisterComponent();
    if (!bDeltaTree) GlacierExteriorLights();
    FRandomStream Stars(41152);
    for (int I=0; I<2400; ++I)
    {
        const FVector Direction = Stars.VRand();
        const double Radius = Stars.FRandRange(1450,2100);
        const double Diameter = Stars.FRandRange(1.1,3.5);
        Part(Direction*Radius,FVector(Diameter),Star,false,FRotator::ZeroRotator,OvalShape);
    }
}

FVector AAquilaBoardingBay::HullViewCenter() const
{
    const FBox Bounds=ExteriorMesh->GetStaticMesh()->GetBoundingBox();
    FVector Focus=Bounds.GetCenter();
    // In the rear three-quarter view the nearby drive bank fills more of the frame than the distant bow.
    if (bDeltaTree) Focus.X-=Bounds.GetSize().X*.125;
    return ExteriorMesh->GetComponentTransform().TransformPosition(Focus);
}

void AAquilaBoardingBay::GlacierExteriorLights()
{
    auto* Hull=ExteriorMesh.Get();
    // Hangar floods reveal the overhanging plates when looking up from the shuttle ramp.
    Lamp({-36,-23,25},FColor(198,223,248),1250000.f,245.f,HullFrame);
    Lamp({31,8,21},FColor(231,218,192),680000.f,210.f,HullFrame);
    // Trace the actual underside so the bay lights reveal its height from Peregrine's ramp.
    const FCollisionQueryParams TraceParams(NAME_None,true);
    const FTransform HullToWorld=Hull->GetComponentTransform();
    for (double X : {-85.,-54.,-23.,8.,39.,70.})
    {
        for (double Y : {-80.,-52.,-24.,4.,32.,60.})
        {
            FHitResult Hit;
            const FVector Start=HullToWorld.TransformPosition(FVector(X,Y,18)*100);
            const FVector End=HullToWorld.TransformPosition(FVector(X,Y,150)*100);
            if (!Hull->LineTraceComponent(Hit,Start,End,TraceParams)) continue;
            const FVector Normal=HullToWorld.InverseTransformVectorNoScale(Hit.ImpactNormal);
            if (Normal.Z>-.28) continue;
            const FVector Surface=HullToWorld.InverseTransformPosition(Hit.ImpactPoint)/100.;
            const FRotator Mount=FRotationMatrix::MakeFromZX(Normal,FVector::ForwardVector).Rotator();
            Part(Surface+Normal*.16,{5.8,1.25,.32},Trim,false,Mount,BoxShape,HullFrame);
            Part(Surface+Normal*.36,{4.6,.58,.11},Ice,false,Mount,BoxShape,HullFrame);
        }
    }

    // Position lights follow the three sealed crossings and the separate outboard blade.
    for (const FVector& P : {FVector(63,100,-5),FVector(-78,124,-.5),FVector(-194,122,12)})
        Part(P,{1.4,1.4,1.4},Ice,false,FRotator::ZeroRotator,OvalShape,HullFrame);
    for (int I=0; I<11; ++I)
    {
        const double T = I/10.;
        Part({74-246*T,105+24*FMath::Sin(T*2.25),-7+15*T*T},{1.8,.72,.45},Amber,false,FRotator::ZeroRotator,BoxShape,HullFrame);
    }
}

void AAquilaBoardingBay::Shuttle()
{
    auto ShuttlePart = [this](FVector Center,FVector Size,EFinish Finish,bool bSolid=true,FRotator Rot=FRotator::ZeroRotator,EShape Shape=BoxShape)
    {
        Part(Center,Size,Finish,bSolid,Rot,Shape,ShuttleFrame);
    };
    auto ShuttleBeam = [this](FVector A,FVector B,double Width,double Height,EFinish Finish,bool bSolid=true)
    {
        Beam(A,B,Width,Height,Finish,bSolid,ShuttleFrame);
    };
    // Parked nose-out. Wing tips span 64 m. Nose and aft sill are 68 m apart.
    ShuttlePart({0,65,8.4},{20,60,13},Pale,true,FRotator::ZeroRotator,OvalShape);
    ShuttlePart({0,73,13.65},{11,27,4.7},Window,true,FRotator::ZeroRotator,OvalShape);
    ShuttlePart({0,42,7.5},{15,24,10},Armor,true,FRotator::ZeroRotator,OvalShape);
    for (double Side : {-1.,1.})
    {
        ShuttlePart({Side*16,62,6.7},{32,30,3.6},Armor,true,FRotator::ZeroRotator,OvalShape);
        ShuttlePart({Side*13,53,6.3},{7,32,6.4},Trim,true,FRotator::ZeroRotator,OvalShape);
        ShuttlePart({Side*13,37.3,6.3},{4.5,.6,3.0},Ice,false,FRotator::ZeroRotator,OvalShape);
        for (double Y : {43.,73.})
        {
            ShuttlePart({Side*7,Y,2.45},{1.2,1.6,4.7},Spine);
            ShuttlePart({Side*7,Y,.22},{3.2,3.6,.44},Trim);
        }
        ShuttlePart({Side*2.12,31,3.7},{.55,8,3.7},Pale);
        ShuttlePart({Side*1.88,30,4.72},{.07,5.5,.12},Amber,false);
    }
    ShuttlePart({0,31,5.68},{4.8,8,.38},Pale);
    ShuttlePart({0,31,1.80},{4.8,10,.40},Deck);
    ShuttlePart({0,35.7,3.65},{4.5,.45,3.7},Spine);
    Sign(TEXT("PEREGRINE"),{0,26.96,5.56},.45,FColor::White,-90);
    // Deck end is at Y=15 m. The top surface meets the 2 m shuttle sill at Y=26 m.
    const FVector Rise(0,11,2);
    const FRotator Tilt = FRotationMatrix::MakeFromYZ(Rise,FVector::UpVector).Rotator();
    const FVector Normal = Tilt.RotateVector(FVector::UpVector);
    ShuttlePart(FVector(0,20.5,1)-Normal*.10,{3.7,Rise.Size(),.20},Deck,true,Tilt);
    for (double Side : {-1.,1.})
    {
        ShuttleBeam({Side*1.87,15,.93},{Side*1.87,26,2.93},.12,.14,Trim);
        ShuttleBeam({Side*1.87,15,1.02},{Side*1.87,26,3.02},.06,.045,Amber,false);
        for (double Y : {15.2,18.7,22.2,25.7})
            ShuttlePart({Side*1.87,Y,(Y-15)*2/11+.48},{.13,.13,.96},Trim);
    }
    for (int I=1; I<15; ++I)
    {
        const double T=I/15.;
        ShuttlePart(FVector(0,15+11*T,2*T)+Normal*.009,{3.26,.085,.018},Inset,false,Tilt);
    }
}

void AAquilaBoardingBay::SwitchHullSize()
{
    ScaleFactor = ScaleFactor == 2 ? 1 : 2;
    FitHullToBerth();
}

FBox2D AAquilaBoardingBay::LandingPadMetres() const
{
    // Grow away from the crew entrance so the ramp still meets the airlock walkway.
    const double HalfWidth = 35.5 * ScaleFactor;
    constexpr double WalkwayEnd = 7.;
    return FBox2D({-HalfWidth, WalkwayEnd}, {HalfWidth, WalkwayEnd + 90. * ScaleFactor});
}

void AAquilaBoardingBay::FitHullToBerth()
{
    HullFrame->SetRelativeScale3D(FVector(ScaleFactor));
    for (USceneComponent* Child : BerthStructure->GetAttachChildren())
        CastChecked<UInstancedStaticMeshComponent>(Child)->ClearInstances();
    BuildLandingPad();

    const double Front = -20.3, Back = -30.7*ScaleFactor, Base = -.5, Top = 30.5*ScaleFactor;
    Part({0,(Front+Back)/2,(Top+Base)/2},{13.6,Front-Back,Top-Base},Spine,true,FRotator::ZeroRotator,BoxShape,BerthStructure);
    const double InnerEdge = LandingPadMetres().Min.Y;
    for (double Side : {-1.,1.})
    {
        const FVector PierFoot(Side*19*ScaleFactor,InnerEdge+5*ScaleFactor,-2.4*ScaleFactor);
        Beam(PierFoot,FVector(Side*19,-42,26)*ScaleFactor,2.5*ScaleFactor,3.4*ScaleFactor,Spine,true,BerthStructure);
        Beam(PierFoot,FVector(Side*19,-36,5.4)*ScaleFactor,2.1*ScaleFactor,2.2*ScaleFactor,Trim,true,BerthStructure);
    }
}

void AAquilaBoardingBay::BuildLandingPad()
{
    const FBox2D Pad = LandingPadMetres();
    const FVector2D Size = Pad.GetSize();
    const double MidY = Pad.GetCenter().Y;
    auto AddPart = [this](FVector Center, FVector Dimensions, EFinish Finish, bool bSolid = true)
    {
        Part(Center,Dimensions,Finish,bSolid,FRotator::ZeroRotator,BoxShape,BerthStructure);
    };
    const double Thickness = 1.3 * ScaleFactor;
    AddPart({0,MidY,-Thickness*.5},{Size.X,Size.Y,Thickness},Deck);
    for (double Side : {-1.,1.})
    {
        const double JoistX = Side*19*ScaleFactor;
        const double RailX = Side*(Pad.Max.X-.6);
        Beam({JoistX,Pad.Min.Y+3*ScaleFactor,-1.5*ScaleFactor},{JoistX,Pad.Max.Y-8*ScaleFactor,-1.5*ScaleFactor},
             1.5*ScaleFactor,2.2*ScaleFactor,Spine,true,BerthStructure);
        for (double Offset : {6.,24.,42.,60.,78.})
        {
            const double Y = Pad.Min.Y + Offset*ScaleFactor;
            Beam({Side*34*ScaleFactor,Y,-1.*ScaleFactor},{JoistX,Y,-2.3*ScaleFactor},
                 .9*ScaleFactor,1.4*ScaleFactor,Trim,true,BerthStructure);
        }
        AddPart({Side*35.3*ScaleFactor,MidY,-1.75*ScaleFactor},{.8*ScaleFactor,Size.Y,2.2*ScaleFactor},Trim);
        // Crew rails and painted marks keep their real-world size, repeated along the longer edge.
        for (double Y=Pad.Min.Y+8.6; Y+2.7<Pad.Max.Y; Y+=18.)
        {
            AddPart({RailX,Y,.55},{.18,5.4,1.1},Trim);
            AddPart({RailX,Y,1.14},{.22,5.4,.08},Amber,false);
        }
        for (double Y=Pad.Min.Y+3; Y+1.3<Pad.Max.Y; Y+=7.)
            AddPart({Side*(Pad.Max.X-.7),Y,.025},{.32,2.6,.05},Amber,false);
        for (double X : {5.2,11.,21.,31.})
            AddPart({Side*X*ScaleFactor,MidY,.012},{.07,Size.Y-1,.024},Inset,false);
    }
    for (double Y=Pad.Min.Y+1.5; Y<Pad.Max.Y; Y+=6.5)
        AddPart({0,Y,.011},{Size.X-.7,.07,.022},Inset,false);
}

void AAquilaBoardingBay::Arrivals()
{
    Part({0,-7,-.52},{20,28,1.04},Deck);
    for (double Side : {-1.,1.})
    {
        Part({Side*2.02,9.4,.016},{.09,12.6,.032},Ice,false);
        Part({Side*2.03,-8.6,.016},{.06,23,.032},Ice,false);
    }
    for (int I=0; I<5; ++I)
        for (double Side : {-1.,1.})
            Beam({Side*.62,5.5+2.0*I,.035},{0,5.0+2.0*I,.035},.11,.025,Amber,false);

    // The airlock is open on both ends for the scale trial. Both apertures are 2.8 m by 3 m.
    for (double Y : {2.6,-4.9})
    {
        for (double Side : {-1.,1.})
        {
            Part({Side*4.2,Y,2.55},{5.6,.68,5.1},Trim);
            Part({Side*1.52,Y+.38,1.52},{.24,.12,3.04},Pale);
            Part({Side*1.55,Y+.46,1.56},{.075,.06,2.84},Amber,false);
        }
        Part({0,Y,4.05},{3.1,.68,2.1},Trim);
        Part({0,Y+.38,3.12},{3.25,.12,.24},Pale);
    }
    // Bay-side bulkhead and recessed service ribs give the pedestrian opening a visible scale.
    for (double Side : {-1.,1.})
    {
        Part({Side*16.2,.6,3.7},{18.4,2.6,7.4},Spine);
        for (int I=0; I<6; ++I)
        {
            const double X=Side*(8.1+3*I);
            Part({X,2.04,3.45},{2.55,.32,6.6},Trim);
            Part({X,2.24,4.5},{1.95,.05,.52},Window,false);
            Part({X,2.25,6.2},{1.4,.06,.08},Ice,false);
        }
        Part({Side*2.55,-1.16,2.43},{.4,7.5,4.86},Inset);
        Part({Side*2.31,-1.16,1.05},{.08,6.6,.11},Amber,false);
        for (double Y : {-3.7,-1.15,1.3})
            Part({Side*2.28,Y,2.0},{.16,.18,3.9},Trim);
        Part({Side*5.1,-12.8,2.61},{.45,15.8,5.22},Trim);
        for (int I=0; I<5; ++I)
        {
            const double Y=-6.65-2.8*I;
            Part({Side*4.82,Y,2.7},{.14,2.48,3.94},Armor);
            Part({Side*4.72,Y,3.62},{.09,1.84,.28},Window,false);
            Part({Side*4.68,Y,4.2},{.06,1.65,.08},Ice,false);
        }
    }
    Part({0,-1.16,4.46},{5.5,7.5,.32},Spine);
    Part({0,-12.8,5.05},{10.5,16,.36},Deck);
    Part({0,-20.4,2.6},{10.6,.8,5.2},Spine);
    for (double X : {-2.45,2.45})
    {
        Part({X,-19.94,2.02},{3.05,.24,4.04},Inset);
        for (double Side : {-1.,1.})
            Part({X+Side*.65,-19.77,1.60},{1.27,.18,3.16},Trim);
        Part({X,-19.60,3.33},{2.65,.10,.10},Ice,false);
        Part({X,-19.74,3.78},{2.72,.10,.61},Window,false);
    }
    Part({0,2.99,4.01},{6,.14,1.04},Window,false);
    Part({0,-4.50,3.89},{4.85,.12,.82},Window,false);
    Sign(TEXT("AIRLOCK  01"),{0,3.10,4.01},.66,FColor(255,208,137));
    Sign(TEXT("LIFT LOBBY"),{0,-4.40,3.89},.54,FColor(160,230,255));
    Sign(TEXT("LIFT  01"),{-2.45,-19.66,3.76},.40,FColor::White);
    Sign(TEXT("LIFT  02"),{2.45,-19.66,3.76},.40,FColor::White);
    for (double Y : {-.3,-8.,-16.})
    {
        const bool bLock = Y > -4;
        Part({0,Y,bLock ? 4.23 : 4.81},{bLock ? 2.4 : 4.8,.38,.065},bLock ? Amber : Ice,false);
        Lamp({0,Y,bLock ? 3.86 : 4.49},bLock ? FColor(255,210,162) : FColor(202,230,255),330,12);
    }
    Lamp({0,8,5.2},FColor(219,233,243),1300,27);
    Lamp({0,23,6},FColor(230,221,201),900,26);
}
