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

#ifndef CRAB_GENERALSETTINGMENU_H
#define CRAB_GENERALSETTINGMENU_H

#include "crab/RadioButtonMenu.h"
#include "crab/ToggleButton.h"
#include "crab/common_header.h"
#include "crab/slider.h"

namespace pyrodactyl {
namespace ui {
class GeneralSettingMenu {
	// The volume sliders and their caption
	Slider vol_music, vol_effects;
	HoverInfo notice_volume;

	// Other settings
	ToggleButton save_on_exit, mouse_trap;

	// The menu for select pop-up text speed
	RadioButtonMenu text_speed;

public:
	GeneralSettingMenu() {}
	~GeneralSettingMenu() {}

	void Load(rapidxml::xml_node<char> *node);
	void HandleEvents(const SDL_Event &Event);
	void InternalEvents();

	void Draw();
	void SetUI();

	void CreateBackup() {
		vol_music.CreateBackup();
		vol_effects.CreateBackup();
	}

	void RestoreBackup();
};
} // End of namespace ui
} // End of namespace pyrodactyl

#endif // CRAB_GENERALSETTINGMENU_H
