using UnrealBuildTool;

public class Aquila : ModuleRules
{
    public Aquila(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "Json", "Slate", "SlateCore"
        });
        if (Target.Platform == UnrealTargetPlatform.Win64 && Target.Configuration != UnrealTargetConfiguration.Shipping)
            PrivateDependencyModuleNames.Add("ImageCore");
    }
}
