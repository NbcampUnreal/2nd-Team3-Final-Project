using UnrealBuildTool;

public class EmberUtils : ModuleRules
{
    public EmberUtils(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "EmberUtils" });
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core","UMG"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "GameplayTags"
            }
        );
    }
}