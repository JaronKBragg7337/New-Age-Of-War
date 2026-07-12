using UnrealBuildTool;

public class NewAgeOfWar : ModuleRules
{
    public NewAgeOfWar(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput"
        });
    }
}
