using UnrealBuildTool;
using System.Collections.Generic;

public class AquilaTarget : TargetRules
{
    public AquilaTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("Aquila");
    }
}
