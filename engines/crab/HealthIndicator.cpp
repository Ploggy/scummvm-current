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


#include "crab/HealthIndicator.h"

using namespace pyrodactyl::ui;

void HealthIndicator::Load(rapidxml::xml_node<char> *node) {
	if (NodeValid(node)) {
		LoadXY(x, y, node);

		for (auto n = node->first_node("img"); n != NULL; n = n->next_sibling("img")) {
			HealthImage hi;
			LoadImgKey(hi.normal, "normal", n);
			LoadImgKey(hi.glow, "glow", n);
			LoadNum(hi.val, "val", n);

			img.push_back(hi);
		}
	}
}

void HealthIndicator::Draw(int num) {
	for (auto i = img.begin(); i != img.end(); ++i)
		if (num == i->val) {
			using namespace pyrodactyl::image;
			gImageManager.Draw(x, y, i->normal);
			gImageManager.GetTexture(i->glow).Alpha(alpha);
			gImageManager.Draw(x, y, i->glow);

			if (inc) {
				alpha += 2;
				if (alpha >= 250)
					inc = false;
			} else {
				alpha -= 2;
				if (alpha < 4)
					inc = true;
			}

			break;
		}
}
