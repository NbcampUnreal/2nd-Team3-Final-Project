using UnrealBuildTool;

public class EmberAbilitySystem : ModuleRules
{
    public EmberAbilitySystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "EmberAbilitySystem", "EmberUtils" });
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "GameplayAbilities",
                "EmberUtils"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "GameplayTasks",
                "GameplayTags"
            }
        );
    }
}