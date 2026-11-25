#include "MeshDescriptionBuilder.h"
#include "AquilaHullLayout.h"
#include "AquilaShipMaterials.h"
#include "StaticMeshAttributes.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshSocket.h"
#include "Materials/Material.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "PhysicsEngine/BodySetup.h"
#include "UObject/SavePackage.h"

namespace
{
// OBJ coordinates here are study centimetres. Glacier's playable hull doubles its 640 m study.
struct FHullPart
{
    FString Name;
    FBox Bounds{ForceInit};
    TMap<int32,FBox> Rings;
    bool bWrap = false;

    explicit FHullPart(FString InName = {}) : Name(MoveTemp(InName))
    {
        bWrap = Name.StartsWith(TEXT("Drive room")) || Name.StartsWith(TEXT("Overlapping armor")) ||
            Name==TEXT("Overhanging main vault") || Name==TEXT("Calved outer blade") || Name==TEXT("Lower forebody");
    }
    void Vertex(const FVector& P)
    {
        Bounds += P;
        if (bWrap) Rings.FindOrAdd(FMath::RoundToInt(P.X),FBox(ForceInit)) += P;
    }
    FName Finish(bool bDelta, FName Original, const FVector& N, const FVector& Middle) const
    {
        const double Height = (Middle.Z-Bounds.Min.Z)/FMath::Max(Bounds.GetSize().Z,1.);
        if (bDelta)
        {
            if (Name==TEXT("Inhabited branch deck"))
            {
                if (N.Z>.34) return TEXT("Armor");
                if (FMath::Abs(N.Z)<.40 && Height>.48 && Height<.74) return TEXT("Windows");
                return TEXT("Spine");
            }
            if (Name.StartsWith(TEXT("Round docking island")) || Name==TEXT("Observation deck"))
                return N.Z>.36 ? TEXT("Island") : N.Z>-.27 ? TEXT("Windows") : TEXT("Hardware");
            if (Name.StartsWith(TEXT("Drive room"))) return TEXT("Spine");
            if (Name.StartsWith(TEXT("Drive end"))) return N.X<-.48 ? TEXT("Exhaust") : TEXT("Hardware");
            if (Name==TEXT("Dish")) return TEXT("Silver");
        }
        else
        {
            if (Name.StartsWith(TEXT("Overlapping armor")) || Name==TEXT("Lower forebody")) return TEXT("Band");
            if (Name==TEXT("Overhanging main vault") || Name==TEXT("Calved outer blade"))
                return N.Z<-.45 ? TEXT("Band") : TEXT("Armor");
            if (Name.StartsWith(TEXT("Observation crown")))
                return N.Z>.45 ? TEXT("Island") : N.Z>-.2 ? TEXT("Windows") : TEXT("Hardware");
            if (Name.StartsWith(TEXT("Enclosed crossing")))
                return FMath::Abs(N.Z)<.40 ? TEXT("Windows") : TEXT("Trim");
            if (Name==TEXT("Set-back dock and aft spine"))
                return FMath::Abs(N.Z)<.4 && Height>.44 && Height<.76 ? TEXT("Windows") : TEXT("Spine");
        }
        if (Name==TEXT("Mast") || Name==TEXT("Dish support")) return TEXT("Hardware");
        return Original;
    }
    double Angle(const FVector& P) const
    {
        const FBox& Ring=Rings.FindChecked(FMath::RoundToInt(P.X));
        const FBox& Section = Ring.GetSize().Y>1 && Ring.GetSize().Z>1 ? Ring : Bounds;
        const FVector R=Section.GetExtent(), D=P-Section.GetCenter();
        return FMath::Atan2(D.Z/FMath::Max(R.Z,1.),D.Y/FMath::Max(R.Y,1.))/UE_TWO_PI;
    }
    void UVs(const FVector (&P)[3], const FVector& N, FVector2D TileCm, FVector2D (&UV)[3]) const
    {
        if (bWrap && FMath::Abs(N.X)<.82)
        {
            const double Start=Angle(P[0]);
            const FVector Radius=Bounds.GetExtent();
            const double Perimeter=UE_TWO_PI*FMath::Sqrt((Radius.Y*Radius.Y+Radius.Z*Radius.Z)*.5);
            // A whole number of tiles around the girth keeps the final seam continuous.
            const double Repeats=FMath::Max(1.,FMath::RoundToDouble(Perimeter/TileCm.Y));
            for (int I=0; I<3; ++I)
            {
                double A=Angle(P[I]);
                A-=FMath::RoundToDouble(A-Start);
                UV[I]={P[I].X/TileCm.X,-A*Repeats};
            }
            const auto AB=UV[1]-UV[0], AC=UV[2]-UV[0];
            if (FMath::Abs(AB.X*AC.Y-AB.Y*AC.X)>1.e-9) return;
        }
        // Flat decks and caps share one metric projection across triangle boundaries.
        const FVector Abs=N.GetAbs();
        for (int I=0; I<3; ++I)
        {
            const FVector& V=P[I];
            if (Abs.Z>=Abs.X && Abs.Z>=Abs.Y) UV[I]={V.X,-V.Y*FMath::Sign(N.Z)};
            else if (Abs.Y>=Abs.X) UV[I]={V.X*FMath::Sign(N.Y),-V.Z};
            else UV[I]={-V.Y*FMath::Sign(N.X),-V.Z};
            UV[I]/=TileCm;
        }
    }
};

// Keep the creases between plates, but let highlights flow around a curved body.
void SmoothHullCorners(FMeshDescription& Mesh)
{
    FStaticMeshAttributes Attributes(Mesh);
    auto Normals=Attributes.GetVertexInstanceNormals();
    auto Tangents=Attributes.GetVertexInstanceTangents();
    for (const FVertexID Vertex : Mesh.Vertices().GetElementIDs())
    {
        const auto Corners=Mesh.GetVertexVertexInstanceIDs(Vertex);
        TArray<FVector3f,TInlineAllocator<16>> Original;
        for (auto Corner : Corners) Original.Add(Normals[Corner]);
        for (int32 I=0; I<Corners.Num(); ++I)
        {
            FVector3f Sum=FVector3f::ZeroVector;
            for (const FVector3f& Other : Original)
                if (FVector3f::DotProduct(Original[I],Other)>.82f) Sum+=Other;
            const FVector3f N=Sum.GetSafeNormal();
            const auto Corner=Corners[I];
            const FVector3f T=Tangents[Corner];
            Normals[Corner]=N;
            Tangents[Corner]=(T-N*FVector3f::DotProduct(N,T)).GetSafeNormal();
        }
    }
}
}

// The source study has separate bodies. Preserve their surfaces and the open space between them.
static bool BuildBoardingMesh(const TCHAR* SourceName, const TCHAR* AssetName, bool bDelta)
{
    const FString Source = FPaths::ProjectSavedDir() / TEXT("Boarding") / SourceName;
    TArray<FString> Lines;
    if (!FFileHelper::LoadFileToStringArray(Lines, *Source))
    {
        UE_LOG(LogTemp, Error, TEXT("Missing boarding study %s. Run scripts/native.ps1 -Action Prepare."),SourceName);
        return false;
    }
    FMeshDescription Description;
    FStaticMeshAttributes(Description).Register();
    FMeshDescriptionBuilder Builder;
    Builder.SetMeshDescription(&Description);
    Builder.SetNumUVLayers(1);
    const auto Finishes=AquilaHullSurfaces(bDelta);
    TOptional<FTransform> BridgeAnchor;
    const FName AnchorName(bDelta ? TEXT("BoardingDock") : TEXT("CommandBridge"));
    TMap<FName,FTransform> Surfaces;
    TMap<FName,FPolygonGroupID> Slots;
    TMap<FName,FVector2D> TileCm;
    for (const auto& Finish : Finishes)
    {
        Slots.Add(Finish.Slot,Builder.AppendPolygonGroup(Finish.Slot));
        TileCm.Add(Finish.Slot,Finish.TileMetres*(bDelta ? 100. : 50.));
    }
    FName Original=TEXT("Armor");
    FHullPart Part;
    TArray<FVertexID> Vertices;
    for (const FString& Line : Lines)
    {
        if (Line.StartsWith(TEXT("#socket ")))
        {
            TArray<FString> P;
            if (Line.ParseIntoArrayWS(P)!=6 || FName(*P[1])!=AnchorName || BridgeAnchor.IsSet()) return false;
            BridgeAnchor.Emplace(FRotator(0,FCString::Atod(*P[5]),0),FVector(FCString::Atod(*P[2]),FCString::Atod(*P[3]),FCString::Atod(*P[4]))*100.);
        }
        else if (Line.StartsWith(TEXT("#surface ")))
        {
            TArray<FString> P;
            if (Line.ParseIntoArrayWS(P)!=8) return false;
            const FName Name(*P[1]);
            if (Surfaces.Contains(Name)) return false;
            const FVector Point(FCString::Atod(*P[2]),FCString::Atod(*P[3]),FCString::Atod(*P[4]));
            const FVector Normal(FCString::Atod(*P[5]),FCString::Atod(*P[6]),FCString::Atod(*P[7]));
            Surfaces.Add(Name,FTransform(FRotationMatrix::MakeFromZX(Normal,FVector::ForwardVector).ToQuat(),Point*100.));
        }
        else if (Line.StartsWith(TEXT("o "))) Part=FHullPart(Line.Mid(2));
        else if (Line.StartsWith(TEXT("usemtl ")))
        {
            Original=FName(*Line.Mid(7));
            if (!Slots.Contains(Original)) return false;
        }
        else if (Line.StartsWith(TEXT("v ")))
        {
            TArray<FString> P;
            if (Line.ParseIntoArrayWS(P) != 4) return false;
            const FVector Point=FVector(FCString::Atod(*P[1]),FCString::Atod(*P[2]),FCString::Atod(*P[3]))*100.;
            Vertices.Add(Builder.AppendVertex(Point));
            Part.Vertex(Point);
        }
        else if (Line.StartsWith(TEXT("f ")))
        {
            TArray<FString> P;
            if (Line.ParseIntoArrayWS(P) != 4) return false;
            FVertexID V[3];
            for (int I = 0; I < 3; ++I)
            {
                const int32 Index = FCString::Atoi(*P[I+1]) - 1;
                if (!Vertices.IsValidIndex(Index)) return false;
                V[I] = Vertices[Index];
            }
            const FVector Points[]={Builder.GetPosition(V[0]),Builder.GetPosition(V[1]),Builder.GetPosition(V[2])};
            const FVector AB=Points[1]-Points[0], AC=Points[2]-Points[0];
            const FVector Normal = FVector::CrossProduct(AB, AC).GetSafeNormal();
            const FName Surface=Part.Finish(bDelta,Original,Normal,(Points[0]+Points[1]+Points[2])/3.);
            FVector2D UV[3];
            Part.UVs(Points,Normal,TileCm.FindChecked(Surface),UV);
            const FVector2D DU=UV[1]-UV[0], DV=UV[2]-UV[0];
            const double Det=DU.X*DV.Y-DU.Y*DV.X;
            const FVector Tangent=FMath::Abs(Det)>1.e-12 ? ((AB*DV.Y-AC*DU.Y)/Det).GetSafeNormal() : AB.GetSafeNormal();
            FVertexInstanceID Corners[3];
            for (int I = 0; I < 3; ++I)
            {
                Corners[I] = Builder.AppendInstance(V[I]);
                Builder.SetInstanceTangentSpace(Corners[I],Normal,Tangent,Det<0 ? -1 : 1);
                Builder.SetInstanceUV(Corners[I],UV[I]);
            }
            Builder.AppendTriangle(Corners[0], Corners[2], Corners[1], Slots.FindChecked(Surface));
        }
    }
    if (Description.Triangles().Num()==0 || !BridgeAnchor.IsSet() || Surfaces.Num()!=AquilaHulls::GunCount+3) return false;
    for (int32 I=0; I<AquilaHulls::GunCount; ++I) if (!Surfaces.Contains(AquilaHulls::GunSocket(I))) return false;
    for (FName Name : {FName(TEXT("Armor")),FName(TEXT("Shield")),FName(TEXT("Reserve"))}) if (!Surfaces.Contains(Name)) return false;
    const FString Path = FString(TEXT("/Game/Foundation/")) + AssetName;
    UPackage* Package = CreatePackage(*Path);
    Package->FullyLoad();
    auto* Mesh = FindObject<UStaticMesh>(Package, AssetName);
    if (!Mesh) Mesh = NewObject<UStaticMesh>(Package, FName(AssetName), RF_Public | RF_Standalone);
    Surfaces.Add(AnchorName,BridgeAnchor.GetValue());
    for (const auto& Entry : Surfaces)
    {
        UStaticMeshSocket* Socket=Mesh->FindSocket(Entry.Key);
        if (!Socket)
        {
            Socket=NewObject<UStaticMeshSocket>(Mesh);
            Socket->SocketName=Entry.Key;
            Mesh->AddSocket(Socket);
        }
        Socket->RelativeLocation=Entry.Value.GetLocation();
        Socket->RelativeRotation=Entry.Value.Rotator();
        Socket->RelativeScale=FVector::OneVector;
    }
    Mesh->GetStaticMaterials().Reset();
    for (const auto& Finish : Finishes)
    {
        auto* M=AquilaBakedSurface(bDelta,Finish.Slot);
        if (!M) return false;
        Mesh->GetStaticMaterials().Emplace(M,Finish.Slot,Finish.Slot);
    }
    Mesh->SetNumSourceModels(1);
    auto& Settings = Mesh->GetSourceModel(0).BuildSettings;
    Settings.bUseFullPrecisionUVs=true;
    Settings.bRecomputeNormals = Settings.bRecomputeTangents = Settings.bGenerateLightmapUVs = false;
    SmoothHullCorners(Description);
    Mesh->CreateMeshDescription(0, MoveTemp(Description));
    Mesh->CommitMeshDescription(0);
    Mesh->CreateBodySetup();
    Mesh->GetBodySetup()->CollisionTraceFlag = CTF_UseComplexAsSimple;
    Mesh->Build(false);
    Package->MarkPackageDirty();
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    return UPackage::SavePackage(Package, Mesh, *FPackageName::LongPackageNameToFilename(Path,TEXT(".uasset")),Args);
}

bool BuildAquilaBoardingMesh()
{
    return BuildBoardingMesh(TEXT("glacier.obj"),TEXT("SM_GlacierBoardingHull"),false) &&
           BuildBoardingMesh(TEXT("delta.obj"),TEXT("SM_DeltaBoardingHull"),true);
}
