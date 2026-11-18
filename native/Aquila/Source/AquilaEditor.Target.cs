using UnrealBuildTool;
using System.Collections.Generic;

public class AquilaEditorTarget : TargetRules
{
    public AquilaEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("Aquila");
        ExtraModuleNames.Add("AquilaTools");
    }
}
