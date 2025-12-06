// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GEB_Project : ModuleRules
{
	public GEB_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "GameplayTasks", "UMG", "Niagara" });
        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate","SlateCore"
        });
    }
}
