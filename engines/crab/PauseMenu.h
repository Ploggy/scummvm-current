/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * This code is based on the CRAB engine
 *
 * Copyright (c) Arvind Raja Yadav
 *
 * Licensed under MIT
 *
 */

#ifndef CRAB_PAUSEMENU_H
#define CRAB_PAUSEMENU_H

#include "crab/FileMenu.h"
#include "crab/GameEventInfo.h"
#include "crab/OptionMenu.h"
#include "crab/SaveGameMenu.h"
#include "crab/SlideShow.h"
#include "crab/common_header.h"
#include "crab/menu.h"
#include "crab/slider.h"
#include "crab/textarea.h"

namespace pyrodactyl {
namespace ui {
enum PauseSignal { PS_NONE,
				   PS_RESUME,
				   PS_SAVE,
				   PS_LOAD,
				   PS_HELP,
				   PS_QUIT_MENU,
				   PS_QUIT_GAME };

class PauseMenu {
	enum PauseState { STATE_NORMAL,
					  STATE_SAVE,
					  STATE_OPTION,
					  STATE_LOAD } state;

	// The pause menu background
	ImageData bg;

	// The buttons in the menu
	ButtonMenu menu;

	// Save game menu
	GameSaveMenu save;

	// The selected main menu button
	int choice;

public:
	PauseMenu(void) {
		state = STATE_NORMAL;
		choice = -1;
	}
	~PauseMenu(void) {}

	void UpdateMode(const bool &ironman) {
		menu.element.at(PS_SAVE - 1).visible = !ironman;
		menu.element.at(PS_LOAD - 1).visible = !ironman;
	}

	void Load(rapidxml::xml_node<char> *node);
	PauseSignal HandleEvents(const SDL_Event &Event, Button &back);

	// Returns true if inside options menu, false otherwise
	bool Draw(Button &back);

	void Reset() { state = STATE_NORMAL; }
	void ScanDir() { save.ScanDir(); }
	std::string SaveFile() { return save.SelectedPath(); }
	bool DisableHotkeys();

	// Should we allow the pause key(default escape) to quit to main menu?
	// This is done because esc is both the "go back on menu level" and the pause key
	bool ShowLevel() { return state == STATE_NORMAL; }

	void SetUI();
};
} // End of namespace ui
} // End of namespace pyrodactyl

#endif // CRAB_PAUSEMENU_H
