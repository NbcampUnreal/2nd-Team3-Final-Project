// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectEmber : ModuleRules
{
	public ProjectEmber(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] { "ProjectEmber", "EmberUtils" });
		
        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput",
	        "NavigationSystem", "GameplayTags","UMG"
        });
        
        PrivateDependencyModuleNames.AddRange(new string[]
        {
	        "ALS",
	        "ALSCamera",
	        "GameplayAbilities",
	        "EmberAbilitySystem",
	        "EmberUtils",
	        "MeleeTrace", "EasyMultiSave"
        });
    }
}
