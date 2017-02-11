/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef MOHAWK_RIVEN_GRAPHICS_H
#define MOHAWK_RIVEN_GRAPHICS_H

#include "mohawk/graphics.h"

namespace Mohawk {

class MohawkEngine_Riven;
class FliesEffect;

enum RivenTransition {
	kRivenTransitionNone      = -1,
	kRivenTransitionWipeLeft  = 0,
	kRivenTransitionWipeRight = 1,
	kRivenTransitionWipeUp    = 2,
	kRivenTransitionWipeDown  = 3,
	kRivenTransitionPanLeft   = 12,
	kRivenTransitionPanRight  = 13,
	kRivenTransitionPanUp     = 14,
	kRivenTransitionPanDown   = 15,
	kRivenTransitionBlend     = 16,
	kRivenTransitionBlend2    = 17
};

class RivenGraphics : public GraphicsManager {
public:
	RivenGraphics(MohawkEngine_Riven *vm);
	~RivenGraphics();

	// Screen updates
	void beginScreenUpdate();
	void applyScreenUpdate(bool force = false);

	void copyImageToScreen(uint16 image, uint32 left, uint32 top, uint32 right, uint32 bottom);
	void updateScreen(Common::Rect updateRect = Common::Rect(0, 0, 608, 392));
	void drawRect(Common::Rect rect, bool active);
	void drawImageRect(uint16 id, Common::Rect srcRect, Common::Rect dstRect);
	void drawExtrasImage(uint16 id, Common::Rect dstRect);
	void drawExtrasImageToScreen(uint16 id, const Common::Rect &rect);

	Graphics::Surface *getEffectScreen();
	Graphics::Surface *getBackScreen();

	// Water Effect
	void scheduleWaterEffect(uint16);
	void clearWaterEffects();
	bool runScheduledWaterEffects();

	// Flies Effect
	void setFliesEffect(uint16 count, bool fireflies);
	void clearFliesEffect();
	void runFliesEffect();

	// Transitions
	void scheduleTransition(RivenTransition id, Common::Rect rect = Common::Rect(0, 0, 608, 392));
	void runScheduledTransition();
	void fadeToBlack();
	void setTransitionSpeed(uint32 speed) { _transitionSpeed = speed; }

	// Credits
	void beginCredits();
	void updateCredits();
	uint getCurCreditsImage() { return _creditsImage; }

protected:
	MohawkSurface *decodeImage(uint16 id);
	MohawkEngine *getVM() { return (MohawkEngine *)_vm; }

private:
	MohawkEngine_Riven *_vm;
	MohawkBitmap *_bitmapDecoder;
	int _screenUpdateNesting;
	bool _screenUpdateRunning;

	// Water Effects
	struct SFXERecord {
		// Record values
		uint16 frameCount;
		Common::Rect rect;
		uint16 speed;
		Common::Array<Common::SeekableReadStream *> frameScripts;

		// Cur frame
		uint16 curFrame;
		uint32 lastFrameTime;
	};
	Common::Array<SFXERecord> _waterEffects;

	// Flies Effect
	FliesEffect *_fliesEffect;

	// Transitions
	RivenTransition _scheduledTransition;
	Common::Rect _transitionRect;
	uint32 _transitionSpeed;

	// Screen Related
	Graphics::Surface *_mainScreen;
	Graphics::Surface *_effectScreen;
	bool _dirtyScreen;

	Graphics::PixelFormat _pixelFormat;
	void clearMainScreen();

	// Credits
	uint _creditsImage, _creditsPos;
};

/**
 * The flies effect draws flies in the scene
 *
 * It can draw either regular flies or fireflies.
 * The flies' movement is simulated in 3 dimensions.
 */
class FliesEffect {
public:
	FliesEffect(MohawkEngine_Riven *vm, uint16 count, bool fireflies);
	~FliesEffect();

	/** Simulate the flies' movement and draw them to the screen */
	void update();

private:
	struct FliesEffectEntry	{
		bool light;
		int posX;
		int posY;
		int posZ;
		const uint16 *alphaMap;
		uint width;
		uint height;
		int framesTillLightSwitch;
		bool hasBlur;
		int blurPosX;
		int blurPosY;
		const uint16 *blurAlphaMap;
		uint blurWidth;
		uint blurHeight;
		float posXFloat;
		float posYFloat;
		float posZFloat;
		float directionAngleRad;
		float directionAngleRadZ;
		float speed;
	};

	struct FliesEffectData {
		bool lightable;
		bool unlightIfTooBright;
		bool isLarge;
		bool canBlur;
		float maxSpeed;
		float minSpeed;
		int maxAcceleration;
		float blurSpeedTreshold;
		float blurDistance;
		uint32 color32;
		int minFramesLit;
		int maxLightDuration;
	};

	MohawkEngine_Riven *_vm;

	uint _nextUpdateTime;
	int _updatePeriodMs;

	Common::Rect _gameRect;
	Graphics::Surface *_effectSurface;
	Graphics::Surface *_backSurface;
	Common::Array<Common::Rect> _screenSurfaceDirtyRects;
	Common::Array<Common::Rect> _effectsSurfaceDirtyRects;

	const FliesEffectData *_parameters;
	static const FliesEffectData _firefliesParameters;
	static const FliesEffectData _fliesParameters;

	Common::Array<FliesEffectEntry> _fly;

	void initFlies(uint16 count);
	void initFlyRandomPosition(uint index);
	void initFlyAtPosition(uint index, int posX, int posY, int posZ);

	void updateFlies();
	void updateFlyPosition(uint index);

	void draw();
	void updateScreen();

	void selectAlphaMap(bool horGridOffset, bool vertGridoffset, const uint16 **alphaMap, uint *width, uint *height);
	void colorBlending(uint32 flyColor, byte &r, byte &g, byte &b, int alpha);

	void addToScreenDirtyRects(const Common::Rect &rect);
	void addToEffectsDirtyRects(const Common::Rect &rect);
	void restoreEffectsSurface();

	int randomBetween(int min, int max);
};

} // End of namespace Mohawk

#endif
