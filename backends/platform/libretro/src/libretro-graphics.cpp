/* Copyright (C) 2024 Giovanni Cascione <ing.cascione@gmail.com>
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

#include "backends/platform/libretro/include/libretro-graphics.h"

void LibretroGraphics::setMousePosition(int x, int y){	
	WindowedGraphicsManager::setMousePosition(x, y);
}

Common::Point LibretroGraphics::getMousePosition(void) const{	
	return Common::Point(_cursorX, _cursorY);
}

Common::Point LibretroGraphics::getMouseVPosition(void) const {
		return WindowedGraphicsManager::convertWindowToVirtual(_cursorX, _cursorY);
}
