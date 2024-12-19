// Copyright Epic Games, Inc. All Rights Reserved.

#include "TODManager.h"
#include "TODManagerStyle.h"
#include "TODManagerCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "Editor/Blutility/Classes/EditorUtilityWidget.h"
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Editor/Blutility/Public/IBlutilityModule.h"
#include "Editor/Blutility/Classes/EditorUtilityWidgetBlueprint.h"

static const FName TODManagerTabName("TODManager");

#define LOCTEXT_NAMESPACE "FTODManagerModule"

void FTODManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FTODManagerStyle::Initialize();
	FTODManagerStyle::ReloadTextures();

	FTODManagerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTODManagerCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FTODManagerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTODManagerModule::RegisterMenus));
}

void FTODManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FTODManagerStyle::Shutdown();

	FTODManagerCommands::Unregister();
}

void FTODManagerModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here

	const FString EditorPanelPath =
	"/Script/Blutility.EditorUtilityWidgetBlueprint'/TODManager/UI/BP_TODEditorPanel.BP_TODEditorPanel'";

	UWidgetBlueprint* Blueprint = LoadObject<UWidgetBlueprint>(nullptr, *EditorPanelPath);
	// check(Blueprint->GeneratedClass->IsChildOf(UEditorUtilityWidget::StaticClass()));
	UEditorUtilityWidgetBlueprint* EditorPanelBlueprint = Cast<UEditorUtilityWidgetBlueprint>(Blueprint);
	if (!EditorPanelBlueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("Editor panel initialized fails"));
	}

	auto EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	EditorPanelWidget = EditorUtilitySubsystem->SpawnAndRegisterTab(EditorPanelBlueprint);
}

void FTODManagerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FTODManagerCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTODManagerCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTODManagerModule, TODManager)