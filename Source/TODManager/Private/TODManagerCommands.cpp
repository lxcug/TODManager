// Copyright Epic Games, Inc. All Rights Reserved.

#include "TODManagerCommands.h"

#define LOCTEXT_NAMESPACE "FTODManagerModule"

void FTODManagerCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "TODManager", "Execute TODManager action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
