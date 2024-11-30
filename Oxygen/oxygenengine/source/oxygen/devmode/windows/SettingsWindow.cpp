﻿/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/devmode/windows/SettingsWindow.h"

#if defined(SUPPORT_IMGUI)

#include "oxygen/devmode/ImGuiHelpers.h"
#include "oxygen/devmode/ImGuiIntegration.h"

#include <optional>


namespace
{
	struct ExternalCodeEditor
	{
		std::string mName;
		std::string mExecutableName;
		std::string mConfigType;
		std::wstring* mConfigPathVariable = nullptr;
		std::wstring mDefaultPath;
		ImGuiHelpers::WideInputString mInputText;
		std::optional<bool> mCheckResult;
	};

	std::vector<ExternalCodeEditor> buildExternalCodeEditorsList()
	{
		std::vector<ExternalCodeEditor> editors;
		Configuration::ExternalCodeEditor& config = Configuration::instance().mDevMode.mExternalCodeEditor;

	#if defined(PLATFORM_WINDOWS)
		{
			ExternalCodeEditor& editor = vectorAdd(editors);
			editor.mName = "Visual Studio Code";
			editor.mExecutableName = "Code.exe";
			editor.mConfigType = "vscode";
			editor.mConfigPathVariable = &config.mVisualStudioCodePath;
			editor.mDefaultPath = Configuration::instance().mAppDataPath + L"../../Local/Programs/Microsoft VS Code/Code.exe";
			FTX::FileSystem->normalizePath(editor.mDefaultPath, false);
		}

		{
			ExternalCodeEditor& editor = vectorAdd(editors);
			editor.mName = "Notepad++";
			editor.mExecutableName = "Notepad++.exe";
			editor.mConfigType = "npp";
			editor.mConfigPathVariable = &config.mNotepadPlusPlusPath;
			editor.mDefaultPath = L"C:/Program Files/Notepad++/notepad++.exe";
		}
	#endif

		// Note: When adding a new editor here, make sure to also extend the Configuration class and update "OpenCodeLocation::open"

		{
			ExternalCodeEditor& editor = vectorAdd(editors);
			editor.mName = "Custom";
			editor.mExecutableName = "Executable";
			editor.mConfigType = "custom";
			editor.mConfigPathVariable = &config.mCustomEditorPath;
		}

		for (ExternalCodeEditor& editor : editors)
		{
			if (editor.mConfigPathVariable->empty())
				editor.mInputText.set(*editor.mConfigPathVariable);
			else
				editor.mInputText.set(editor.mDefaultPath);
		}

		return editors;
	}
}


SettingsWindow::SettingsWindow() :
	DevModeWindowBase("Settings", Category::MISC, 0)
{
}

void SettingsWindow::buildContent()
{
	ImGui::SetWindowPos(ImVec2(350.0f, 10.0f), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(500.0f, 250.0f), ImGuiCond_FirstUseEver);

	if (ImGui::CollapsingHeader("Game View Window", 0))
	{
		ImGuiHelpers::ScopedIndent si;

		Configuration& config = Configuration::instance();
		ImGui::SliderFloat("Size", &config.mDevMode.mGameViewScale, 0.2f, 1.0f, "%.2f");

		ImGui::SliderFloat("Alignment X", &config.mDevMode.mGameViewAlignment.x, -1.0f, 1.0f, "%.3f");
		ImGui::SameLine();
		if (ImGui::Button("Center##x"))
			config.mDevMode.mGameViewAlignment.x = 0.0f;

		ImGui::SliderFloat("Alignment Y", &config.mDevMode.mGameViewAlignment.y, -1.0f, 1.0f, "%.3f");
		ImGui::SameLine();
		if (ImGui::Button("Center##y"))
			config.mDevMode.mGameViewAlignment.y = 0.0f;
	}

	Color& accentColor = ImGuiIntegration::getAccentColor();
	if (ImGui::ColorEdit3("Dev Mode UI Accent Color", accentColor.data, ImGuiColorEditFlags_NoInputs))
	{
		ImGuiIntegration::refreshImGuiStyle();
	}

	ImGui::DragFloat("UI Scale", &ImGui::GetIO().FontGlobalScale, 0.003f, 0.5f, 4.0f, "%.1f");

	// External file editor settings
	//  -> Only really makes sense on desktop platforms
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
	if (ImGui::CollapsingHeader("External Code Editor"))
	{
		ImGuiHelpers::ScopedIndent si(8);

		Configuration::ExternalCodeEditor& config = Configuration::instance().mDevMode.mExternalCodeEditor;

		static std::vector<ExternalCodeEditor> editors = buildExternalCodeEditorsList();
		for (ExternalCodeEditor& editor : editors)
		{
			ImGui::PushID(&editor);
			bool active = (config.mActiveType == editor.mConfigType);

			if (ImGui::RadioButton(*String(0, "Use %s", editor.mName.c_str()), active))
			{
				config.mActiveType = editor.mConfigType;
				active = true;
			}

			ImGuiHelpers::ScopedIndent si2(24);

			ImGui::BeginDisabled(!active);

			if (active)
				editor.mInputText.set(*editor.mConfigPathVariable);

			if (ImGui::InputText(*String(0, "%s File Location", editor.mExecutableName.c_str()), editor.mInputText.mInternalUTF8, sizeof(editor.mInputText.mInternalUTF8)))
			{
				editor.mInputText.refreshFromInternal();
				*editor.mConfigPathVariable = editor.mInputText.get();
				editor.mCheckResult.reset();
			}

			if (active)
			{
				if (!editor.mDefaultPath.empty())
				{
					if (ImGui::SmallButton("Use Default Path"))
					{
						*editor.mConfigPathVariable = editor.mDefaultPath;
						editor.mCheckResult.reset();
					}
				}

				if (!editor.mConfigPathVariable->empty())
				{
					if (ImGui::SmallButton("Check"))
					{
						editor.mCheckResult = FTX::FileSystem->isFile(*editor.mConfigPathVariable);
					}
					if (editor.mCheckResult.has_value())
					{
						ImGui::SameLine();
						if (*editor.mCheckResult)
							ImGui::Text("Ready to go!");
						else
							ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "File not found, please enter to full file path");
					}
				}
			}

			if (editor.mConfigType == "custom")
			{
				static ImGuiHelpers::WideInputString argsText;
				argsText.set(config.mCustomEditorArgs);

				if (ImGui::InputText("Arguments Format", argsText.mInternalUTF8, sizeof(argsText.mInternalUTF8)))
				{
					argsText.refreshFromInternal();
					config.mCustomEditorArgs = argsText.get();
				}
			}

			ImGui::EndDisabled();
			ImGui::PopID();
		}
	}
#endif
}

#endif
