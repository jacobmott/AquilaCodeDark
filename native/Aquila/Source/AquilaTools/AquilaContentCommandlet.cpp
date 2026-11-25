#include "AquilaContentCommandlet.h"
#include "AquilaShipMaterials.h"
#include "AquilaTurretMeshes.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionSine.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionPreSkinnedPosition.h"
#include "Materials/MaterialExpressionWorldPosition.h"
#include "Materials/MaterialExpressionPixelNormalWS.h"
#include "Materials/MaterialExpressionCameraVectorWS.h"
#include "Materials/MaterialExpressionPerInstanceCustomData.h"
#include "Materials/MaterialExpressionVertexInterpolator.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace
{
bool BuildCombatMaterial(UMaterial* Material, const FString& Name)
{
    Material->BlendMode = BLEND_Additive;
    Material->TwoSided = Name == TEXT("M_CombatBurst");
    Material->SetShadingModel(MSM_Unlit);
    auto* Shader = NewObject<UMaterialExpressionCustom>(Material);
    const FString Source = FPaths::ProjectDir() / TEXT("Shaders") / (Name + TEXT(".ush"));
    if (!FFileHelper::LoadFileToString(Shader->Code, *Source))
    {
        UE_LOG(LogTemp, Error, TEXT("Missing Aquila combat material source: %s"), *Source);
        return false;
    }
    Shader->OutputType = CMOT_Float3;
    Shader->Description = Name;
    Shader->Inputs.Reset();
    auto Input = [&](FName Pin, UMaterialExpression* Expression)
    {
        Material->GetExpressionCollection().AddExpression(Expression);
        auto& Slot = Shader->Inputs.AddDefaulted_GetRef();
        Slot.InputName = Pin;
        Slot.Input.Connect(0, Expression);
    };
    auto Vector = [&](FName Pin)
    {
        auto* Parameter = NewObject<UMaterialExpressionVectorParameter>(Material);
        Parameter->ParameterName = Pin;
        Input(Pin, Parameter);
    };
    if (Name == TEXT("M_ShieldContact"))
    {
        auto* Position = NewObject<UMaterialExpressionPreSkinnedPosition>(Material);
        Material->GetExpressionCollection().AddExpression(Position);
        auto* Local = NewObject<UMaterialExpressionVertexInterpolator>(Material);
        Local->Input.Connect(0, Position);
        Input(TEXT("SpherePosition"), Local);
        for (FName Pin : {FName(TEXT("Axes")), FName(TEXT("Eye")), FName(TEXT("HitA")), FName(TEXT("HitB")), FName(TEXT("HitC"))})
            Vector(Pin);
        auto* IdleGain = NewObject<UMaterialExpressionScalarParameter>(Material);
        IdleGain->ParameterName = TEXT("IdleGain");
        IdleGain->DefaultValue = 1;
        Input(TEXT("IdleGain"), IdleGain);
        auto* Strength = NewObject<UMaterialExpressionScalarParameter>(Material);
        Strength->ParameterName = TEXT("ShellStrength");
        Strength->DefaultValue = 1;
        Material->GetExpressionCollection().AddExpression(Strength);
        Material->GetEditorOnlyData()->Opacity.Connect(0, Strength);
    }
    else if (Name == TEXT("M_HullShield"))
    {
        Input(TEXT("PositionWS"), NewObject<UMaterialExpressionWorldPosition>(Material));
        Input(TEXT("NormalWS"), NewObject<UMaterialExpressionPixelNormalWS>(Material));
        Input(TEXT("ViewWS"), NewObject<UMaterialExpressionCameraVectorWS>(Material));
        Input(TEXT("UV"), NewObject<UMaterialExpressionTextureCoordinate>(Material));
        for (FName Pin : {FName(TEXT("HitA")), FName(TEXT("HitB")), FName(TEXT("HitC")), FName(TEXT("Ages"))}) Vector(Pin);
        auto* Reach = NewObject<UMaterialExpressionScalarParameter>(Material);
        Reach->ParameterName = TEXT("Reach");
        Reach->DefaultValue = 750;
        Input(TEXT("Reach"), Reach);
    }
    else
    {
        Input(TEXT("UV"), NewObject<UMaterialExpressionTextureCoordinate>(Material));
        auto* Data = NewObject<UMaterialExpressionPerInstanceCustomData3Vector>(Material);
        Data->DataIndex = 0;
        Material->GetExpressionCollection().AddExpression(Data);
        auto* Varying = NewObject<UMaterialExpressionVertexInterpolator>(Material);
        Varying->Input.Connect(0, Data);
        Input(TEXT("Settings"), Varying);
    }
    Material->GetExpressionCollection().AddExpression(Shader);
    Material->GetEditorOnlyData()->EmissiveColor.Connect(0, Shader);
    Material->GetEditorOnlyData()->Opacity.Constant = 1;
    return true;
}
}

bool BuildAquilaBoardingMesh();

int32 UAquilaContentCommandlet::Main(const FString& Params)
{
    FString Library = FPlatformMisc::GetEnvironmentVariable(TEXT("AQUILA_SHIP_FINISHES"));
    if (Library.IsEmpty())
        Library = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../../../asset-pipeline/Assets/AssetPipelineLab/Aquila_Research_Battleships/R003"));
    if (!PrepareAquilaShipMaterials(Library)) return 1;
    // Generated local assets have authored source here. No texture or binary assets enter Git.
    const TCHAR* Names[] = { TEXT("M_Hull"), TEXT("M_Glow"), TEXT("M_Planet"), TEXT("M_ShieldContact"), TEXT("M_CombatBurst"), TEXT("M_HullShield") };
    for (int32 Kind = 0; Kind < UE_ARRAY_COUNT(Names); ++Kind)
    {
        const bool bGlow = Kind == 1;
        const FString Name = Names[Kind];
        const FString Path = TEXT("/Game/Foundation/") + Name;
        UPackage* Package = CreatePackage(*Path);
        Package->FullyLoad();
        UMaterial* Material = FindObject<UMaterial>(Package, *Name);
        if (!Material) Material = NewObject<UMaterial>(Package, *Name, RF_Public | RF_Standalone);
        Material->GetExpressionCollection().Empty();
        if (Kind >= 3)
        {
            if (!BuildCombatMaterial(Material, Name)) return 1;
        }
        else
        {
            Material->SetShadingModel(bGlow ? MSM_Unlit : MSM_DefaultLit);
            auto* Color = NewObject<UMaterialExpressionVectorParameter>(Material);
            Color->ParameterName = TEXT("Color");
            Color->DefaultValue = FLinearColor(0.16f, 0.24f, 0.32f);
            Material->GetExpressionCollection().AddExpression(Color);
            if (bGlow)
            {
                auto* Strength = NewObject<UMaterialExpressionScalarParameter>(Material);
                Strength->ParameterName = TEXT("Glow");
                Strength->DefaultValue = 2;
                Material->GetExpressionCollection().AddExpression(Strength);
                auto* Multiply = NewObject<UMaterialExpressionMultiply>(Material);
                Multiply->A.Connect(0, Color);
                Multiply->B.Connect(0, Strength);
                Material->GetExpressionCollection().AddExpression(Multiply);
                Material->GetEditorOnlyData()->EmissiveColor.Connect(0, Multiply);
            }
            else
            {
                Material->GetEditorOnlyData()->BaseColor.Connect(0, Color);
                Material->GetEditorOnlyData()->Roughness.Constant = 0.72f;
                Material->GetEditorOnlyData()->Metallic.Constant = 0.35f;
            }
            if (Kind == 2)
            {
                // Two sine waves make a cheap stylized gas-world backdrop, without texture downloads or heavy noise.
                auto* UV = NewObject<UMaterialExpressionTextureCoordinate>(Material);
                UV->UTiling = 3; UV->VTiling = 14;
                auto* U = NewObject<UMaterialExpressionComponentMask>(Material);
                U->Input.Connect(0, UV); U->R=true; U->G=false;
                auto* V = NewObject<UMaterialExpressionComponentMask>(Material);
                V->Input.Connect(0, UV); V->R=false; V->G=true;
                auto* Wave = NewObject<UMaterialExpressionSine>(Material);
                Wave->Input.Connect(0,U);
                auto* Distortion = NewObject<UMaterialExpressionMultiply>(Material);
                Distortion->A.Connect(0,Wave); Distortion->ConstB=0.35f;
                auto* Latitude = NewObject<UMaterialExpressionAdd>(Material);
                Latitude->A.Connect(0,V); Latitude->B.Connect(0,Distortion);
                auto* Bands = NewObject<UMaterialExpressionSine>(Material);
                Bands->Input.Connect(0,Latitude);
                auto* Warm = NewObject<UMaterialExpressionVectorParameter>(Material);
                Warm->ParameterName=TEXT("Clouds"); Warm->DefaultValue=FLinearColor(0.16f,0.38f,0.46f);
                Color->DefaultValue=FLinearColor(0.018f,0.065f,0.13f);
                auto* Blend = NewObject<UMaterialExpressionLinearInterpolate>(Material);
                Blend->A.Connect(0,Color); Blend->B.Connect(0,Warm); Blend->Alpha.Connect(0,Bands);
                for (UMaterialExpression* E : TArray<UMaterialExpression*>{UV,U,V,Wave,Distortion,Latitude,Bands,Warm,Blend})
                    Material->GetExpressionCollection().AddExpression(E);
                Material->GetEditorOnlyData()->BaseColor.Connect(0,Blend);
                Material->GetEditorOnlyData()->Metallic.Constant=0;
                Material->GetEditorOnlyData()->Roughness.Constant=1;
            }
        }
        Material->SetMaterialUsage(MATUSAGE_InstancedStaticMeshes);
        Material->PostEditChange();
        Package->MarkPackageDirty();
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        Args.SaveFlags = SAVE_None;
        if (!UPackage::SavePackage(Package, Material, *FPackageName::LongPackageNameToFilename(Path, TEXT(".uasset")), Args))
        {
            UE_LOG(LogTemp, Error, TEXT("Aquila failed to save generated material: %s"), *Path);
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("Aquila content generated: %s"), *Path);
    }
    return BuildAquilaTurretMeshes() && BuildAquilaBoardingMesh() ? 0 : 1;
}
