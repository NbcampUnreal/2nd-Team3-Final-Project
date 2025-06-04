// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectEmber : ModuleRules
{
    public ProjectEmber(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //───────────────────────────────────────────────────────────────────────────────
        // PublicIncludePaths: 이 모듈(ProjectEmber)에서 헤더를 찾을 때 참조할 경로들
        PublicIncludePaths.AddRange(new string[]
        {
            "ProjectEmber",
            "EmberUtils"
        });

        //───────────────────────────────────────────────────────────────────────────────
        // PublicDependencyModuleNames: 런타임(게임/Shipping)에서 반드시 필요한 모듈들
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "NavigationSystem",
            "AIModule",
            "Niagara",
            "EnhancedInput",
            "GameplayTags",
            "UMG",
            "CinematicCamera",
            "Slate",
            "SlateCore"
        });

        //───────────────────────────────────────────────────────────────────────────────
        // PrivateDependencyModuleNames: 내부 구현에서 사용하는(링크해야 하는) 런타임 모듈들
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "ALS",
            "ALSCamera",
            "GameplayAbilities",
            "EmberAbilitySystem",
            "EmberUtils",
            "MeleeTrace",
            "EasyMultiSave",
            "Water"
        });

        //───────────────────────────────────────────────────────────────────────────────
        // 아래 블록 안의 모듈들은 **에디터 전용**
        if (Target.Type == TargetType.Editor)
        {
            PublicDependencyModuleNames.Add("DeveloperSettings");
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",      
                "UMGEditor",     
                "SequencerCore", 
                "CurveEditor",   
                "PropertyEditor",
                "EditorConfig"
            });
        }
    }
}
