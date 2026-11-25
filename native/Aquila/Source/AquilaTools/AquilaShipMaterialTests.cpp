#if WITH_DEV_AUTOMATION_TESTS
#include "AquilaShipMaterials.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"
#include "Misc/AutomationTest.h"
#include "StaticMeshAttributes.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FAquilaShipMaterialTest,"Aquila.Art.PlayableShip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FAquilaShipMaterialTest::GetTests(TArray<FString>& Names,TArray<FString>& Commands) const
{
    Names.Add(TEXT("DeltaTree")); Commands.Add(TEXT("Delta"));
    Names.Add(TEXT("GlacierTongue")); Commands.Add(TEXT("Glacier"));
}

bool FAquilaShipMaterialTest::RunTest(const FString& Ship)
{
    const bool bDelta=Ship==TEXT("Delta");
    const FString Name=TEXT("SM_")+Ship+TEXT("BoardingHull");
    auto* Mesh=LoadObject<UStaticMesh>(nullptr,*(TEXT("/Game/Foundation/")+Name+TEXT(".")+Name));
    if (!TestNotNull(TEXT("Prepared playable hull"),Mesh)) return false;
    const auto* D=Mesh->GetMeshDescription(0);
    if (!TestNotNull(TEXT("Authored mesh description"),D)) return false;
    FStaticMeshConstAttributes A(*D);
    const auto UV=A.GetVertexInstanceUVs();
    const auto Points=A.GetVertexPositions();
    const auto Names=A.GetPolygonGroupMaterialSlotNames();
    int32 Tagged=0, Visible=0, Mapped=0;
    for (const auto& Surface : AquilaHullSurfaces(bDelta))
    {
        auto* Material=Mesh->GetMaterial(Mesh->GetMaterialIndex(Surface.Slot));
        if (!TestNotNull(*Surface.Slot.ToString(),Material)) return false;
        TestTrue(TEXT("Assigned ship finish"),Material==AquilaBakedSurface(bDelta,Surface.Slot));
        for (FName Channel : {FName(TEXT("BaseMap")),FName(TEXT("NormalMap")),FName(TEXT("SurfaceMap")),FName(TEXT("LightMap"))})
        {
            UTexture* Map=nullptr;
            if (!TestTrue(TEXT("Ship texture is present"),Material->GetTextureParameterValue(Channel,Map) && Map)) return false;
            TestTrue(TEXT("Texture belongs to this ship"),Map->GetName().StartsWith(bDelta ? TEXT("T_DT_") : TEXT("T_GT_")));
            TestTrue(TEXT("Color and emission are sRGB. Normals and packed masks are linear."),
                Map->SRGB==(Channel==TEXT("BaseMap") || Channel==TEXT("LightMap")));
            if (Channel==TEXT("NormalMap")) TestTrue(TEXT("Normal uses DirectX green"),Map->CompressionSettings==TC_Normalmap && Map->bFlipGreenChannel);
            if (Channel==TEXT("SurfaceMap")) TestTrue(TEXT("Occlusion, roughness and metalness use mask compression"),Map->CompressionSettings==TC_Masks);
        }
        float Emission=0;
        TestTrue(TEXT("Only the window and optic finishes emit light"),
            Material->GetScalarParameterValue(TEXT("Emission"),Emission) && FMath::IsNearlyEqual(Emission,Surface.Emission));
    }
    for (const FPolygonGroupID Group : D->PolygonGroups().GetElementIDs())
    {
        const auto Triangles=D->GetPolygonGroupTriangles(Group);
        TestTrue(*FString::Printf(TEXT("%s has assigned faces"),*Names[Group].ToString()),!Triangles.IsEmpty());
        Tagged+=Triangles.Num();
        for (auto Triangle : Triangles)
        {
            const auto C=D->GetTriangleVertexInstances(Triangle);
            const auto P0=Points[D->GetVertexInstanceVertex(C[0])],P1=Points[D->GetVertexInstanceVertex(C[1])],P2=Points[D->GetVertexInstanceVertex(C[2])];
            if (FVector3f::CrossProduct(P1-P0,P2-P0).SizeSquared()<=1.e-4f) continue;
            ++Visible;
            const auto U=UV.Get(C[1],0)-UV.Get(C[0],0),V=UV.Get(C[2],0)-UV.Get(C[0],0);
            if (FMath::Abs(double(U.X)*V.Y-double(U.Y)*V.X)>1.e-12) ++Mapped;
        }
    }
    TestEqual(TEXT("All authored triangles remain"),Tagged,bDelta ? 10364 : 5596);
    TestEqual(TEXT("Every nondegenerate triangle has nonzero texture area"),Mapped,Visible);
    AddInfo(FString::Printf(TEXT("%s: %d textured material sections, %d mapped triangles of %d nondegenerate faces."),
        *Ship,D->PolygonGroups().Num(),Mapped,Visible));
    return true;
}
#endif
