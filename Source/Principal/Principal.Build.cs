// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Principal : ModuleRules
{
	public Principal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Principal",
			"Principal/Variant_Platforming",
			"Principal/Variant_Platforming/Animation",
			"Principal/Variant_Combat",
			"Principal/Variant_Combat/AI",
			"Principal/Variant_Combat/Animation",
			"Principal/Variant_Combat/Gameplay",
			"Principal/Variant_Combat/Interfaces",
			"Principal/Variant_Combat/UI",
			"Principal/Variant_SideScrolling",
			"Principal/Variant_SideScrolling/AI",
			"Principal/Variant_SideScrolling/Gameplay",
			"Principal/Variant_SideScrolling/Interfaces",
			"Principal/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
