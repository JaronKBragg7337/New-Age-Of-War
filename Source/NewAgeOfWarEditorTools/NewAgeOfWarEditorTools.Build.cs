using UnrealBuildTool;

public class NewAgeOfWarEditorTools : ModuleRules
{
    public NewAgeOfWarEditorTools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core", "CoreUObject", "Engine", "UnrealEd", "Landscape",
            "AssetRegistry", "NewAgeOfWar"
        });
    }
}
