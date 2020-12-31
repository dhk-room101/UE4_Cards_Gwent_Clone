// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Cards : ModuleRules
{
	public Cards(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		PublicDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore" }); //for UMG
	    PublicDependencyModuleNames.AddRange(new string[] { "MoviePlayer", "MediaAssets" }); // for loading screen
		
		PrivateDependencyModuleNames.AddRange(new string[] { "XmlParser" });
		
		AdditionalPropertiesForReceipt.Add("AndroidPlugin", System.IO.Path.Combine(ModuleDirectory, "AndroidSanitizePermissions_UPL.xml"));
	}
}