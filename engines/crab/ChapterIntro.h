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

#ifndef CRAB_CHAPTERINTRO_H
#define CRAB_CHAPTERINTRO_H

#include "crab/GameEventInfo.h"
#include "crab/button.h"
#include "crab/dialogbox.h"
#include "crab/sprite.h"

namespace pyrodactyl {
namespace ui {
class ChapterIntro {
	// This contains the background image info and start button
	GameDialogBox dialog;

	// This is where the sprite is drawn
	Element pos;

	// The traits button
	Button traits;

public:
	// Should we show the traits screen
	bool show_traits;

	ChapterIntro() { show_traits = false; }

	void Load(rapidxml::xml_node<char> *node);
	bool HandleEvents(SDL_Event &Event);
	void Draw(pyrodactyl::event::Info &info, std::string &text,
			  pyrodactyl::anim::Sprite *cur_sp, const pyrodactyl::people::PersonState &state);
};
} // End of namespace ui
} // End of namespace pyrodactyl

#endif // CRAB_CHAPTERINTRO_H
