#include "AquilaTurretMeshes.h"
#include "AquilaWeapons.h"
#include "AquilaDefenses.h"
#include "AquilaAmmunition.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"
#include "Misc/PackageName.h"
#include "PhysicsEngine/BodySetup.h"
#include "UObject/SavePackage.h"

namespace
{
enum EFinish { Plate, Frame, Edge, Emitter, Chip };
const FName Finishes[] = {TEXT("Plate"), TEXT("Frame"), TEXT("Edge"), TEXT("Emitter"), TEXT("Chip")};

// Convex panels share flat normals per face. The same authored mesh serves the showroom, hull and shield copy.
struct FFittingSculpt
{
    FMeshDescription Description;
    FMeshDescriptionBuilder Builder;
    FPolygonGroupID Groups[UE_ARRAY_COUNT(Finishes)];

    FFittingSculpt()
    {
        FStaticMeshAttributes(Description).Register();
        Builder.SetMeshDescription(&Description);
        Builder.SetNumUVLayers(1);
        for (int32 I = 0; I < UE_ARRAY_COUNT(Finishes); ++I) Groups[I] = Builder.AppendPolygonGroup(Finishes[I]);
    }

    void Face(const TArray<FVector>& Points, FVector Inside, EFinish Finish)
    {
        FVector Normal = FVector::CrossProduct(Points[1] - Points[0], Points[2] - Points[0]).GetSafeNormal();
        const bool bOutward = FVector::DotProduct(Normal, Points[0] - Inside) > 0;
        if (!bOutward) Normal = -Normal;
        const FVector Tangent = (Points[1] - Points[0]).GetSafeNormal();
        const FVector Across = FVector::CrossProduct(Normal, Tangent);
        TArray<FVertexInstanceID, TInlineAllocator<20>> Corners;
        for (const FVector& P : Points)
        {
            const auto Corner = Builder.AppendInstance(Builder.AppendVertex(P));
            Builder.SetInstanceTangentSpace(Corner, Normal, Tangent, 1);
            Builder.SetInstanceUV(Corner, FVector2D(P.Dot(Tangent), P.Dot(Across)) / 100.);
            Corners.Add(Corner);
        }
        // Unreal's exterior triangle winding is clockwise.
        for (int32 I = 1; I + 1 < Corners.Num(); ++I)
            Builder.AppendTriangle(Corners[0], Corners[bOutward ? I+1 : I], Corners[bOutward ? I : I+1], Groups[Finish]);
    }

    void Join(const TArray<FVector>& A, const TArray<FVector>& B, FVector Inside, EFinish Finish)
    {
        for (int32 I = 0; I < A.Num(); ++I)
        {
            const int32 J = (I+1) % A.Num();
            Face({A[I], A[J], B[J], B[I]}, Inside, Finish);
        }
    }

    void Panel(FVector Center, FVector Size, EFinish Finish)
    {
        const double X = Size.X*.5, Y = Size.Y*.5, H = Size.Z*.5;
        const double Cut = FMath::Min(X,Y)*.42;
        const double Bevel = FMath::Min(H*.48, FMath::Min(X,Y)*.18);
        auto Ring = [&](double Inset, double Z) {
            const double A = X-Inset, B = Y-Inset;
            TArray<FVector> Points{{-A+Cut,-B,Z},{A-Cut,-B,Z},{A,-B+Cut,Z},{A,B-Cut,Z},
                {A-Cut,B,Z},{-A+Cut,B,Z},{-A,B-Cut,Z},{-A,-B+Cut,Z}};
            for (FVector& P : Points) P += Center;
            return Points;
        };
        const auto Bottom = Ring(0,-H), Shoulder = Ring(0,H-Bevel), Top = Ring(Bevel,H);
        Face(Bottom, Center, Finish);
        Join(Bottom, Shoulder, Center, Finish);
        Join(Shoulder, Top, Center, Finish);
        Face(Top, Center, Finish);
    }

    void Spindle(FVector Start, FVector End, double R0, double R1, EFinish Finish, int32 Sides = 12)
    {
        const FVector Axis = (End - Start).GetSafeNormal();
        FVector Right, Up;
        Axis.FindBestAxisVectors(Right, Up);
        TArray<FVector> Near, Far;
        for (int32 I = 0; I < Sides; ++I)
        {
            const double Angle = UE_TWO_PI * (I + .5) / Sides;
            const FVector Radial = FMath::Cos(Angle)*Right + FMath::Sin(Angle)*Up;
            Near.Add(Start + Radial*R0);
            Far.Add(End + Radial*R1);
        }
        const FVector Inside = (Start+End)*.5;
        Face(Near, Inside, Finish);
        Join(Near, Far, Inside, Finish);
        Face(Far, Inside, Finish);
    }

    void Model(EAquilaWeapon Type)
    {
        Spindle({-9,0,-8}, {-9,0,-3}, 25,28,Frame);
        Spindle({-9,0,-3}, {-9,0,0}, 28,25,Edge);
        Spindle({-9,0,0}, {-9,0,6}, 22,21,Frame);
        if (Type == EAquilaWeapon::TwinTurrets)
        {
            Panel({-10,0,12},{42,48,20},Plate);
            for (int32 Side : {-1,1})
            {
                const double Y = 15.*Side;
                Panel({3,Y,19},{41,16,14},Plate);
                Spindle({16,Y,19},{49,Y,19},6,4,Frame);
                Spindle({35,Y,19},{39,Y,19},6,6,Edge);
                Spindle({48,Y,19},{56,Y,19},7,7,Edge);
                Spindle({56,Y,19},{57,Y,19},4.6,4.6,Frame);
                Spindle({57,Y,19},{57.4,Y,19},2.5,2.5,Emitter);
                Panel({-6,Y,26},{16,4,2},Emitter);
            }
        }
        else if (Type == EAquilaWeapon::KineticCannon)
        {
            Panel({-14,0,13},{52,45,27},Plate);
            Panel({-15,0,29},{31,23,7},Frame);
            Spindle({3,0,21},{55,0,21},10,5.5,Edge,8);
            Spindle({16,0,21},{30,0,21},12,10,Frame,8);
            Spindle({30,0,21},{33,0,21},10,9,Emitter,8);
            Panel({57,0,21},{20,21,15},Plate);
            Panel({67,0,21},{2.2,14,9},Frame);
            for (int32 Side : {-1,1}) Panel({-16,Side*22.,16},{32,5,8},Emitter);
        }
        else if (Type == EAquilaWeapon::PlasmaCaster)
        {
            Spindle({-12,0,4},{-12,0,16},26,22,Plate);
            Spindle({-12,0,16},{-12,0,25},22,12,Frame);
            for (int32 Side : {-1,1})
            {
                const FVector Rear(-12,Side*15.,15), Joint(12,Side*28.,20), Nose(40,Side*20.,29);
                Spindle(Rear,Joint,10,7,Plate,6);
                Spindle(Joint,Nose,8,4.5,Edge,6);
                Spindle(Nose,Nose + FVector(6,-Side*2.,3),4.8,1.5,Emitter,6);
            }
            Spindle({12,0,22},{22,0,22},5,13,Emitter);
            Spindle({22,0,22},{33,0,22},13,4,Emitter);
            Spindle({20,0,22},{23,0,22},14,14,Frame);
        }
        else
        {
            Panel({-15,0,13},{40,38,18},Frame);
            Spindle({-14,0,19},{10,0,19},21,25,Plate,8);
            Spindle({10,0,19},{17,0,19},25,22,Frame,8);
            for (int32 I = 0; I < 3; ++I)
            {
                const double Angle = UE_TWO_PI * I / 3;
                const FVector Offset(0, 13.*FMath::Sin(Angle), 13.*FMath::Cos(Angle));
                Spindle(FVector(14,0,19)+Offset, FVector(38,0,19)+Offset,8,5.4,Edge,8);
                Spindle(FVector(24,0,19)+Offset, FVector(28,0,19)+Offset,8.4,8.4,Emitter,8);
                Spindle(FVector(38,0,19)+Offset, FVector(39,0,19)+Offset,4,4,Emitter,8);
            }
        }
        Panel({-23,0,Type == EAquilaWeapon::KineticCannon ? 33. : Type == EAquilaWeapon::PlasmaCaster ? 25. : 24.}, {6,9,3}, Chip);
    }

    void Model(EAquilaAmmo Type)
    {
        auto Section = [&](double From, double To, double Rear, double Front, EFinish Finish, int32 Sides = 12) {
            Spindle(FVector(From,0,0), FVector(To,0,0), Rear, Front, Finish, Sides);
        };
        if (Type == EAquilaAmmo::Standard)
        {
            // A compact brass capsule with a blunt ceramic cap and twin signal bands.
            Section(-38,-34,13,18,Edge);
            Section(-34,-29,18,18,Frame);
            Section(-29,12,17,17,Plate);
            for (double X : {-20.,-13.}) Section(X,X+3.,18,18,Emitter);
            Section(12,18,17,15,Frame);
            Section(18,27,15,12,Plate);
            Section(27,34,12,6,Edge);
            Section(34,36,6,4,Edge);
            for (int32 I=0; I<3; ++I)
            {
                const FVector Rim = FRotator(0,0,I*120.).RotateVector(FVector(0,0,16.8));
                Spindle(FVector(-4,0,0)+Rim,FVector(9,0,0)+Rim,1.2,1.2,Frame,5);
            }
        }
        else if (Type == EAquilaAmmo::Piercing)
        {
            // Long faceted silver point, swept shoulders and a cyan spine.
            Section(-46,-37,5,9,Frame,6);
            Section(-37,-20,9,11,Plate,6);
            Section(-20,-15,11.5,11.5,Emitter,6);
            Section(-15,0,11,9,Plate,6);
            Section(0,45,9,1,Edge,5);
            for (int32 I=0; I<3; ++I)
            {
                const double A = UE_TWO_PI*I/3.;
                const FVector Out(0,FMath::Cos(A),FMath::Sin(A)), Across(0,-Out.Z,Out.Y);
                const TArray<FVector> Ridge = {FVector(-39,0,0)+Out*7., FVector(-39,0,0)+Out*17.,
                    FVector(-30,0,0)+Out*17., FVector(6,0,0)+Out*7.};
                TArray<FVector> Left, Right;
                for (const FVector& P : Ridge) { Left.Add(P-Across*1.6); Right.Add(P+Across*1.6); }
                const FVector Inside = (Ridge[0]+Ridge[1]+Ridge[2]+Ridge[3])*.25;
                Face(Left,Inside,Plate);
                Face(Right,Inside,Plate);
                Join(Left,Right,Inside,Edge);
                Spindle(Ridge[2],Ridge[3],1.15,.9,Emitter,5);
            }
        }
        else
        {
            // Separated crystal shards float inside two open halos.
            Section(-39,-30,1.5,8,Plate,5);
            Section(-30,-22,8,3.5,Emitter,5);
            Section(-15,-2,2.5,13,Plate,5);
            Section(-2,1.5,13,13,Emitter,5);
            Section(1.5,13,13,3,Plate,5);
            Section(20,26,3,8,Emitter,5);
            Section(26,41,8,1,Plate,5);
            for (int32 Ring=0; Ring<2; ++Ring)
            {
                auto Point = [Ring](int32 Corner) {
                    const double A = UE_TWO_PI*(Corner+.45*Ring)/10.;
                    return FVector(Ring ? 17. : -19., 20.*FMath::Cos(A), 20.*FMath::Sin(A));
                };
                for (int32 I=0; I<10; ++I)
                {
                    if ((I+2*Ring)%5 == 0) continue;
                    Spindle(Point(I),Point(I+1),2.45,2.45,I%5==2 ? Emitter : Edge,6);
                }
            }
        }
    }

    void Crystal(FVector A, FVector B, FVector C, double Height)
    {
        const FVector Center = (A+B+C)/3.;
        const FVector Peak = Center + FVector(0,0,Height);
        const FVector Inside = Center + FVector(0,0,Height*.25);
        Face({A,B,C}, Inside, Frame);
        Face({A,B,Peak}, Inside, Plate);
        Face({B,C,Peak}, Inside, Edge);
        Face({C,A,Peak}, Inside, Plate);
    }

    void Model(EAquilaDefense Type)
    {
        if (Type == EAquilaDefense::LightPlate)
        {
            Panel({0,0,2},{85,68,4},Frame);
            Panel({0,0,5},{80,64,4},Plate);
            for (double Y : {-23.,23.})
            {
                Panel({0,Y,7.3},{56,2.8,1.5},Edge);
                Panel({16,Y,8.2},{14,2.1,1},Emitter);
            }
        }
        else if (Type == EAquilaDefense::ReinforcedPlate)
        {
            Panel({0,0,3},{89,72,6},Frame);
            Panel({0,0,8},{84,69,9},Edge);
            for (double X : {-27.,0.,27.})
            {
                Panel({X,0,14},{24,66,13},Plate);
                for (double Y : {-23.,23.}) Spindle({X,Y,19.5},{X,Y,21.5},2.8,2.5,Edge,6);
            }
        }
        else if (Type == EAquilaDefense::PrismPlate)
        {
            Panel({0,0,2},{85,67,4},Frame);
            Panel({0,0,4.4},{72,55,2},Emitter);
            for (double X : {-17.,17.})
            for (double Y : {-12.,12.})
            {
                const FVector C(X,Y,5.5);
                Crystal(C+FVector(-16,-11,0), C+FVector(15,-11,0), C+FVector(-16,10,0), 9);
                Crystal(C+FVector(16,11,0), C+FVector(-15,11,0), C+FVector(16,-10,0), 6);
            }
        }
        else if (Type == EAquilaDefense::ShieldEmitter)
        {
            Spindle({0,0,0},{0,0,6},31,34,Frame,12);
            Spindle({0,0,6},{0,0,11},32,27,Plate,12);
            Spindle({0,0,10},{0,0,19},17,13,Emitter,10);
            Spindle({0,0,19},{0,0,26},13,2,Emitter,10);
            for (int32 I = 0; I < 6; ++I)
            {
                const double A = UE_TWO_PI*I/6.;
                const FVector Radial(FMath::Cos(A),FMath::Sin(A),0);
                Spindle(Radial*26+FVector(0,0,8), Radial*21+FVector(0,0,23), 5.4,3,Edge,6);
            }
        }
        else if (Type == EAquilaDefense::ReserveCapacitor)
        {
            Panel({0,0,4},{63,59,8},Frame);
            for (double Y : {-17.,0.,17.})
            {
                Spindle({-26,Y,14},{26,Y,14},8,8,Emitter,10);
                for (double X : {-21.,0.,21.}) Spindle({X-2,Y,14},{X+2,Y,14},9,9,Edge,10);
            }
            for (double X : {-29.,29.}) Panel({X,0,12},{7,58,17},Plate);
        }
        else
        {
            // Open contacts surround a visibly empty socket.
            for (int32 I = 0; I < 6; ++I)
            {
                const FVector A = FRotator(0,I*60.,0).RotateVector(FVector(30,0,4));
                const FVector B = FRotator(0,(I+1)*60.,0).RotateVector(FVector(30,0,4));
                Spindle(A,B,4,4,Frame,6);
                Spindle(A-FVector(0,0,3),A+FVector(0,0,6),5,4,Edge,6);
            }
        }
    }
};

template <typename TModel> bool SaveFittingMesh(const TCHAR* Asset, TModel Type)
{
    const FString Path = FString(TEXT("/Game/Foundation/")) + Asset;
    UPackage* Package = CreatePackage(*Path);
    Package->FullyLoad();
    auto* Mesh = FindObject<UStaticMesh>(Package, Asset);
    if (!Mesh) Mesh = NewObject<UStaticMesh>(Package, Asset, RF_Public | RF_Standalone);
    Mesh->GetStaticMaterials().Reset();
    for (FName Finish : Finishes) Mesh->GetStaticMaterials().Emplace(UMaterial::GetDefaultMaterial(MD_Surface), Finish, Finish);
    Mesh->SetNumSourceModels(1);
    auto& Settings = Mesh->GetSourceModel(0).BuildSettings;
    Settings.bRecomputeNormals = Settings.bRecomputeTangents = Settings.bGenerateLightmapUVs = false;
    FFittingSculpt Sculpt;
    Sculpt.Model(Type);
    Mesh->CreateMeshDescription(0, MoveTemp(Sculpt.Description));
    Mesh->CommitMeshDescription(0);
    Mesh->Build(false);
    Mesh->CreateBodySetup();
    Mesh->GetBodySetup()->CollisionTraceFlag = CTF_UseComplexAsSimple;
    Package->MarkPackageDirty();
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    if (!UPackage::SavePackage(Package, Mesh, *FPackageName::LongPackageNameToFilename(Path,TEXT(".uasset")),Args)) return false;
    UE_LOG(LogTemp, Display, TEXT("Prepared fitting mesh %s"), *Path);
    return true;
}
}

bool BuildAquilaTurretMeshes()
{
    for (const auto& Model : AquilaWeapons::Models)
        if (!SaveFittingMesh(Model.Asset, Model.Weapon)) return false;
    for (const auto& Model : AquilaDefenses::Models)
        if (!SaveFittingMesh(Model.Asset, Model.Type)) return false;
    for (const auto& Model : AquilaAmmunition::Models)
        if (!SaveFittingMesh(Model.Asset, Model.Type)) return false;
    return true;
}
