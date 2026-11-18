using UnrealBuildTool;

public class Aquila : ModuleRules
{
    public Aquila(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "Json", "Slate", "SlateCore"
        });
    }
}
