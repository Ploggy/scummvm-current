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

#include "backends/graphics/opengl/opengl-graphics.h"
#include "backends/graphics/opengl/framebuffer.h"
#include "graphics/opengl/debug.h"

#include "backends/platform/libretro/include/libretro-defs.h"
#include "backends/platform/libretro/include/libretro-core.h"
#include "backends/platform/libretro/include/libretro-os.h"
#include "backends/platform/libretro/include/libretro-timer.h"
#include "backends/platform/libretro/include/libretro-graphics-opengl.h"

LibretroOpenGLGraphics::LibretroOpenGLGraphics(OpenGL::ContextType contextType) {
	const Graphics::PixelFormat rgba8888 =
#ifdef SCUMM_LITTLE_ENDIAN
									   Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24);
#else
									   Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0);
#endif
	notifyContextCreate(contextType, new LibretroHWFramebuffer(), rgba8888, rgba8888);
	handleResize(RES_W_OVERLAY, RES_H_OVERLAY);
}

void LibretroOpenGLGraphics::refreshScreen(){
	dynamic_cast<LibretroTimerManager *>(LIBRETRO_G_SYSTEM->getTimerManager())->checkThread(THREAD_SWITCH_UPDATE);
}

void LibretroOpenGLGraphics::contextDestroy(void){
	notifyContextDestroy();
}

void LibretroHWFramebuffer::activateInternal(){
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, retro_get_hw_fb()));
}
