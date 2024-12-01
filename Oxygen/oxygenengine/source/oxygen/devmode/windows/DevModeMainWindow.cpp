﻿/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/devmode/windows/DevModeMainWindow.h"

#if defined(SUPPORT_IMGUI)

#include "oxygen/devmode/ImGuiHelpers.h"
#include "oxygen/devmode/windows/AudioBrowserWindow.h"
#include "oxygen/devmode/windows/GameSimWindow.h"
#include "oxygen/devmode/windows/GameVisualizationsWindow.h"
#include "oxygen/devmode/windows/MemoryHexViewWindow.h"
#include "oxygen/devmode/windows/PaletteViewWindow.h"
#include "oxygen/devmode/windows/ScriptBuildWindow.h"
#include "oxygen/devmode/windows/SettingsWindow.h"
#include "oxygen/devmode/windows/SpriteBrowserWindow.h"
#include "oxygen/devmode/windows/WatchesWindow.h"


DevModeMainWindow::DevModeMainWindow() :
	DevModeWindowBase("Dev Mode (F1)", Category::MISC, ImGuiWindowFlags_AlwaysAutoResize)
{
	mIsWindowOpen = true;

	// Create windows
	//  -> Note that the order of creation defines he listing order isnide each categories
	{
		createWindow(mGameSimWindow);

		createWindow(mGameVisualizationsWindow);
		createWindow(mPaletteViewWindow);
		createWindow(mMemoryHexViewWindow);
		createWindow(mWatchesWindow);

		createWindow(mScriptBuildWindow);

		createWindow(mAudioBrowserWindow);
		createWindow(mSpriteBrowserWindow);

		createWindow(mSettingsWindow);
	}
}

DevModeMainWindow::~DevModeMainWindow()
{
	for (DevModeWindowBase* window : mAllWindows)
		delete window;
}

bool DevModeMainWindow::buildWindow()
{
	const bool result = DevModeWindowBase::buildWindow();

	for (DevModeWindowBase* window : mAllWindows)
	{
		window->buildWindow();
	}

	// ImGui demo window for testing
	if (mShowImGuiDemo)
		ImGui::ShowDemoWindow();

	return result;
}

void DevModeMainWindow::buildContent()
{
	ImGui::SetWindowPos(ImVec2(5.0f, 5.0f), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(150.0f, 200.0f), ImGuiCond_FirstUseEver);
	ImGui::SetWindowCollapsed(true, ImGuiCond_FirstUseEver);

	const float uiScale = ImGui::GetIO().FontGlobalScale;

	const char* TEXT_BY_CATEGORY[] =
	{
		"Simulation",
		"Debugging",
		"Scripts",
		"Assets",
		"Misc"
	};
	constexpr int NUM_CATEGORIES = (int)DevModeWindowBase::Category::MISC + 1;
	static_assert(sizeof(TEXT_BY_CATEGORY) / sizeof(const char*) == NUM_CATEGORIES);

	if (ImGui::BeginTable("Table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp, ImVec2(270 * uiScale, 0)))
	{
		ImGui::TableNextRow();

		for (int categoryIndex = 0; categoryIndex < NUM_CATEGORIES; ++categoryIndex)
		{
			if (categoryIndex == 0 || categoryIndex == 2)
				ImGui::TableSetColumnIndex(categoryIndex / 2);

			ImGui::SeparatorText(TEXT_BY_CATEGORY[categoryIndex]);
			for (DevModeWindowBase* window : mAllWindows)
			{
				if (window->mCategory == (DevModeWindowBase::Category)categoryIndex)
				{
					ImGui::Checkbox(window->mTitle.c_str(), &window->mIsWindowOpen);
				}
			}
		}

	#ifdef DEBUG
		ImGui::Checkbox("ImGui Demo", &mShowImGuiDemo);

		// Just for debugging
		//ImGui::Text("ImGui Capture:   %s %s", ImGui::GetIO().WantCaptureMouse ? "[M]" : "      ", ImGui::GetIO().WantCaptureKeyboard ? "[K]" : "");
	#endif

		ImGui::EndTable();
	}
}

void DevModeMainWindow::openWatchesWindow()
{
	mWatchesWindow->mIsWindowOpen = true;
}

#endif
