#include "AquilaContentCommandlet.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionSine.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"

int32 UAquilaContentCommandlet::Main(const FString& Params)
{
    // Generated local assets have authored source here. No copied art, textures or binary assets enter Git.
    for (int32 Kind = 0; Kind < 3; ++Kind)
    {
        const bool bGlow = Kind == 1;
        const FString Name = Kind == 2 ? TEXT("M_Planet") : bGlow ? TEXT("M_Glow") : TEXT("M_Hull");
        const FString Path = TEXT("/Game/Foundation/") + Name;
        UPackage* Package = CreatePackage(*Path);
        Package->FullyLoad();
        UMaterial* Material = FindObject<UMaterial>(Package, *Name);
        if (!Material) Material = NewObject<UMaterial>(Package, *Name, RF_Public | RF_Standalone);
        Material->GetExpressionCollection().Empty();
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
        bool bNeedsRecompile = false;
        Material->SetMaterialUsage(bNeedsRecompile, MATUSAGE_InstancedStaticMeshes);
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
    return 0;
}
