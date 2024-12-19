// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "TODManagerStyle.h"

class FTODManagerCommands : public TCommands<FTODManagerCommands>
{
public:

	FTODManagerCommands()
		: TCommands<FTODManagerCommands>(TEXT("TODManager"), NSLOCTEXT("Contexts", "TODManager", "TODManager Plugin"), NAME_None, FTODManagerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
