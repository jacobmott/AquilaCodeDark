#include "AquilaShipMaterials.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureCube.h"
#include "Math/Float16Color.h"
#include "ImageUtils.h"
#include "ImageCore.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionPerInstanceCustomData.h"
#include "Materials/MaterialExpressionPreSkinnedNormal.h"
#include "Materials/MaterialExpressionPreSkinnedPosition.h"
#include "Materials/MaterialExpressionVertexInterpolator.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "UObject/SavePackage.h"

namespace
{
const FAquilaHullSurface Delta[] = {
    {TEXT("Armor"), TEXT("ivory"), TEXT("plate"), {180,180}},
    {TEXT("Trim"), TEXT("bronze"), TEXT("trim"), {55,55}},
    {TEXT("Spine"), TEXT("graphite"), TEXT("service"), {105,105}},
    {TEXT("Exhaust"), TEXT("beacon"), TEXT("optic"), {48,48}, 3.3f},
    {TEXT("Island"), TEXT("limestone"), TEXT("plate"), {90,90}},
    {TEXT("Windows"), TEXT("cabin"), TEXT("window"), {30,16}, 2.6f},
    {TEXT("Hardware"), TEXT("joint"), TEXT("service"), {68,68}},
    {TEXT("Silver"), TEXT("silver"), TEXT("plate"), {78,78}},
};
const FAquilaHullSurface Glacier[] = {
    {TEXT("Armor"), TEXT("silver-strake"), TEXT("strake"), {640,320}},
    {TEXT("Trim"), TEXT("pewter"), TEXT("plate"), {80,80}},
    {TEXT("Spine"), TEXT("shadow"), TEXT("service"), {120,120}},
    {TEXT("Band"), TEXT("blue-strake"), TEXT("strake"), {520,260}},
    {TEXT("Island"), TEXT("frost"), TEXT("plate"), {85,85}},
    {TEXT("Windows"), TEXT("cabin"), TEXT("window"), {48,24}, 1.15f},
    {TEXT("Hardware"), TEXT("graphite"), TEXT("service"), {68,68}},
};

const FString Folder = TEXT("/Game/Foundation/Ships/");
FString Prefix(bool bDelta) { return bDelta ? TEXT("DT_") : TEXT("GT_"); }
FString FinishName(bool bDelta, FName Slot) { return TEXT("MI_") + Prefix(bDelta) + Slot.ToString(); }

template<typename T> T* Asset(const FString& Name)
{
    UPackage* Package = CreatePackage(*(Folder + Name));
    Package->FullyLoad();
    if (auto* Existing = FindObject<T>(Package, *Name)) return Existing;
    return NewObject<T>(Package, *Name, RF_Public | RF_Standalone);
}

bool Save(UObject* Object)
{
    Object->MarkPackageDirty();
    auto* Package = Object->GetOutermost();
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    return UPackage::SavePackage(Package,Object,*FPackageName::LongPackageNameToFilename(Package->GetName(),TEXT(".uasset")),Args);
}

// Broad off-camera illumination for reflective hulls in a black space environment.
bool BuildSpaceReflection()
{
    constexpr int32 Size=64;
    const FVector Warm=FVector(.35,-.45,.82).GetSafeNormal(), Cool=FVector(-.68,.7,.22).GetSafeNormal();
    TArray<FFloat16Color> Pixels;
    Pixels.Reserve(6*Size*Size);
    for (int32 Face=0; Face<6; ++Face)
        for (int32 Y=0; Y<Size; ++Y)
            for (int32 X=0; X<Size; ++X)
            {
                const double U=(X+.5)*2./Size-1., V=(Y+.5)*2./Size-1.;
                const FVector Axes[]={{1,-V,-U},{-1,-V,U},{U,1,V},{U,-1,-V},{U,-V,1},{-U,-V,-1}};
                const FVector Dir=Axes[Face].GetSafeNormal();
                const float Key=FMath::Pow(FMath::Max(0.,FVector::DotProduct(Dir,Warm)),3.);
                const float Fill=FMath::Pow(FMath::Max(0.,FVector::DotProduct(Dir,Cool)),4.);
                Pixels.Emplace(FLinearColor(.17f,.20f,.25f)+FLinearColor(1.f,.96f,.85f)*Key+FLinearColor(.40f,.52f,.70f)*Fill);
            }
    auto* Cube=Asset<UTextureCube>(TEXT("T_SpaceReflection"));
    Cube->PreEditChange(nullptr);
    Cube->Source.Init(Size,Size,6,1,TSF_RGBA16F,reinterpret_cast<const uint8*>(Pixels.GetData()));
    Cube->SRGB=false;
    Cube->CompressionSettings=TC_HDR;
    Cube->LODGroup=TEXTUREGROUP_Skybox;
    Cube->PostEditChange();
    return Save(Cube);
}

UTexture2D* ImportMap(const FString& Dir, bool bDelta, const FString& File, TMap<FString,UTexture2D*>& Cache)
{
    const FString Name = TEXT("T_") + Prefix(bDelta) + File.Replace(TEXT("-"), TEXT("_"));
    if (auto* Cached = Cache.Find(Name)) return *Cached;
    FImage Image;
    const FString Path = Dir / (File + TEXT(".png"));
    if (!FPaths::FileExists(Path))
    {
        // A prepared checkout can rebuild its geometry while the source art library is offline.
        auto* Baked=LoadObject<UTexture2D>(nullptr,*(Folder+Name+TEXT(".")+Name),nullptr,LOAD_NoWarn);
        if (Baked && Baked->Source.IsValid())
        {
            Cache.Add(Name,Baked);
            return Baked;
        }
    }
    if (!FImageUtils::LoadImage(*Path, Image))
    {
        UE_LOG(LogTemp,Error,TEXT("Cannot read ship finish map %s. Set AQUILA_SHIP_FINISHES to the R003 source directory."), *Path);
        return nullptr;
    }
    const bool bNormal = File.EndsWith(TEXT("-normal")), bMasks = File.EndsWith(TEXT("-orm"));
    // The authored PNG bytes already have the required transfer function. Swizzle channels without changing samples.
    Image.GammaSpace = bNormal || bMasks ? EGammaSpace::Linear : EGammaSpace::sRGB;
    Image.ChangeFormat(ERawImageFormat::BGRA8, Image.GammaSpace);
    auto* Tex = Asset<UTexture2D>(Name);
    Tex->PreEditChange(nullptr);
    Tex->Source.Init(Image.SizeX,Image.SizeY,1,1,TSF_BGRA8,Image.RawData.GetData());
    Tex->CompressionSettings = bNormal ? TC_Normalmap : bMasks ? TC_Masks : TC_Default;
    Tex->SRGB = !bNormal && !bMasks;
    Tex->bFlipGreenChannel = bNormal; // Blender authored OpenGL tangent normals. Unreal uses DirectX normals.
    Tex->LODGroup = bNormal ? TEXTUREGROUP_WorldNormalMap : TEXTUREGROUP_World;
    Tex->PostEditChange();
    if (!Save(Tex)) return nullptr;
    Cache.Add(Name,Tex);
    return Tex;
}

template<typename T> T* Node(UMaterial* M)
{
    auto* Expr = NewObject<T>(M);
    M->GetExpressionCollection().AddExpression(Expr);
    return Expr;
}

UMaterial* Master(UTexture2D* Albedo, UTexture2D* Normal, UTexture2D* Masks)
{
    auto* M = Asset<UMaterial>(TEXT("M_ShipSurface"));
    M->GetExpressionCollection().Empty();
    M->SetShadingModel(MSM_DefaultLit);
    M->SetMaterialUsage(MATUSAGE_InstancedStaticMeshes);
    auto* UV = Node<UMaterialExpressionTextureCoordinate>(M);
    auto* Span=Node<UMaterialExpressionPerInstanceCustomData3Vector>(M);
    Span->DataIndex=0; Span->ConstDefaultValue=FLinearColor::Black;
    auto* Face=Node<UMaterialExpressionPreSkinnedNormal>(M);
    auto* Scale=Node<UMaterialExpressionCustom>(M);
    Scale->Description=TEXT("Room panel scale");
    Scale->Inputs.Reset();
    auto Input=[&](const TCHAR* Name, UMaterialExpression* Expr) {
        auto& Pin=Scale->Inputs.AddDefaulted_GetRef();
        Pin.InputName=Name; Pin.Input.Connect(0,Expr);
    };
    Input(TEXT("UV"),UV); Input(TEXT("Face"),Face); Input(TEXT("Span"),Span);
    Input(TEXT("Position"),Node<UMaterialExpressionPreSkinnedPosition>(M));
    Scale->OutputType=CMOT_Float2;
    Scale->Code=TEXT("if (Span.x <= 0) return UV; float3 p=Position*.01*Span; float3 n=abs(Face); return n.z>.5 ? float2(p.x,-p.y*sign(Face.z)) : (n.x>.5 ? float2(-p.y*sign(Face.x),-p.z) : float2(p.x*sign(Face.y),-p.z));");
    auto* ScaledUV=Node<UMaterialExpressionVertexInterpolator>(M);
    ScaledUV->Input.Connect(0,Scale);
    auto Map = [&](const TCHAR* Name, UTexture2D* Default, EMaterialSamplerType Type)
    {
        auto* Sample = Node<UMaterialExpressionTextureSampleParameter2D>(M);
        Sample->ParameterName=Name; Sample->Texture=Default; Sample->SamplerType=Type;
        Sample->Coordinates.Connect(0,ScaledUV);
        return Sample;
    };
    auto* Color=Map(TEXT("BaseMap"),Albedo,SAMPLERTYPE_Color);
    auto* Relief=Map(TEXT("NormalMap"),Normal,SAMPLERTYPE_Normal);
    auto* ORM=Map(TEXT("SurfaceMap"),Masks,SAMPLERTYPE_Masks);
    auto* Satin=Node<UMaterialExpressionScalarParameter>(M);
    Satin->ParameterName=TEXT("SatinCoat"); Satin->DefaultValue=0;
    auto Coated=[&](int32 Channel,float Target) {
        auto* Result=Node<UMaterialExpressionLinearInterpolate>(M);
        Result->A.Connect(Channel,ORM); Result->ConstB=Target; Result->Alpha.Connect(0,Satin);
        return Result;
    };
    auto* Lights=Map(TEXT("LightMap"),Albedo,SAMPLERTYPE_Color);
    auto* Tint=Node<UMaterialExpressionVectorParameter>(M);
    Tint->ParameterName=TEXT("Color"); Tint->DefaultValue=FLinearColor::White;
    auto* Painted=Node<UMaterialExpressionMultiply>(M);
    Painted->A.Connect(0,Color); Painted->B.Connect(0,Tint);
    auto* Flat=Node<UMaterialExpressionConstant3Vector>(M);
    Flat->Constant=FLinearColor(0,0,1);
    auto* Bump=Node<UMaterialExpressionLinearInterpolate>(M);
    Bump->A.Connect(0,Flat); Bump->B.Connect(0,Relief); Bump->ConstAlpha=.53f;
    auto* Power=Node<UMaterialExpressionScalarParameter>(M);
    Power->ParameterName=TEXT("Emission"); Power->DefaultValue=0;
    auto* Lit=Node<UMaterialExpressionMultiply>(M);
    Lit->A.Connect(0,Lights); Lit->B.Connect(0,Power);
    auto* Data=M->GetEditorOnlyData();
    Data->BaseColor.Connect(0,Painted); Data->Normal.Connect(0,Bump);
    Data->AmbientOcclusion.Connect(1,ORM);
    Data->Roughness.Connect(0,Coated(2,.65f)); Data->Metallic.Connect(0,Coated(3,.32f));
    Data->EmissiveColor.Connect(0,Lit);
    M->PostEditChange();
    return Save(M) ? M : nullptr;
}
}

TConstArrayView<FAquilaHullSurface> AquilaHullSurfaces(bool bDelta)
{
    return bDelta ? MakeArrayView(Delta) : MakeArrayView(Glacier);
}

UMaterialInterface* AquilaBakedSurface(bool bDelta, FName Slot)
{
    const FString Name=FinishName(bDelta,Slot);
    return LoadObject<UMaterialInterface>(nullptr,*(Folder+Name+TEXT(".")+Name));
}

bool PrepareAquilaShipMaterials(const FString& Library)
{
    if (!BuildSpaceReflection()) return false;
    TMap<FString,UTexture2D*> Cache;
    UMaterial* Base=nullptr;
    for (const bool bDelta : {true, false})
    {
        const FString Dir = Library / (bDelta ? TEXT("delta-tree") : TEXT("glacier-tongue")) / TEXT("textures");
        for (const auto& S : AquilaHullSurfaces(bDelta))
        {
            auto* Color=ImportMap(Dir,bDelta,FString(S.ColorMap)+TEXT("-color"),Cache);
            auto* Normal=ImportMap(Dir,bDelta,FString(S.MapFamily)+TEXT("-normal"),Cache);
            auto* ORM=ImportMap(Dir,bDelta,FString(S.ColorMap)+TEXT("-orm"),Cache);
            auto* Light=S.Emission>0 ? ImportMap(Dir,bDelta,FString(S.MapFamily)+TEXT("-emission"),Cache) : Color;
            if (!Color || !Normal || !ORM || !Light) return false;
            if (!Base) Base=Master(Color,Normal,ORM);
            if (!Base) return false;
            auto* Finish=Asset<UMaterialInstanceConstant>(FinishName(bDelta,S.Slot));
            Finish->SetParentEditorOnly(Base);
            Finish->SetTextureParameterValueEditorOnly(TEXT("BaseMap"),Color);
            Finish->SetTextureParameterValueEditorOnly(TEXT("NormalMap"),Normal);
            Finish->SetTextureParameterValueEditorOnly(TEXT("SurfaceMap"),ORM);
            Finish->SetTextureParameterValueEditorOnly(TEXT("LightMap"),Light);
            Finish->SetScalarParameterValueEditorOnly(TEXT("Emission"),S.Emission);
            const float Coating=!bDelta && S.Slot==TEXT("Armor") ? .52f : !bDelta && S.Slot==TEXT("Band") ? .32f : 0.f;
            Finish->SetScalarParameterValueEditorOnly(TEXT("SatinCoat"),Coating);
            Finish->PostEditChange();
            if (!Save(Finish)) return false;
        }
    }
    UE_LOG(LogTemp,Display,TEXT("Aquila ship finishes: %d texture maps, %d Delta and %d Glacier materials."),
        Cache.Num(),UE_ARRAY_COUNT(Delta),UE_ARRAY_COUNT(Glacier));
    return true;
}
