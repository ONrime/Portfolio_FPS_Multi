// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Dalos : ModuleRules
{
	public Dalos(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "OnlineSubsystemUtils", "Steamworks", "Networking", "UMG" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		PrivateDependencyModuleNames.Add("OnlineSubsystem");

		PrivateIncludePaths.Add("Dalos/");
		PublicIncludePaths.Add("Dalos/");
	}
}
