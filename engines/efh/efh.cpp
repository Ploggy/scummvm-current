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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/system.h"
#include "common/random.h"
#include "common/error.h"
#include "common/config-manager.h"
#include "common/events.h"
#include "engines/util.h"
#include "graphics/cursorman.h"

#include "efh/efh.h"
#include "engines/util.h"

namespace Efh {

EfhEngine *EfhEngine::s_Engine = nullptr;

EfhGraphicsStruct::EfhGraphicsStruct() {
	_vgaLineBuffer = nullptr;
	_shiftValue = 0;
	_width = 0;
	_height = 0;
	_area = Common::Rect(0, 0, 0, 0);
}
EfhGraphicsStruct::EfhGraphicsStruct(int16 *lineBuf, int16 x, int16 y, int16 width, int16 height) {
	_vgaLineBuffer = lineBuf;
	_shiftValue = 0;
	_width = width;
	_height = height;
	_area = Common::Rect(x, y, x + width - 1, y + height - 1);
}

void EfhGraphicsStruct::copy(EfhGraphicsStruct *src) {
	// Same buffer address
	_vgaLineBuffer = src->_vgaLineBuffer;
	_shiftValue = src->_shiftValue;
	_width = src->_width;
	_height = src->_height;
	_area = src->_area;
}

EfhEngine::EfhEngine(OSystem *syst, const EfhGameDescription *gd) : Engine(syst), _gameDescription(gd) {
	_system = syst;
	_rnd = nullptr;

	_shouldQuit = false;
	_eventMan = nullptr;
	_lastTime = 0;
	_gameType = kGameTypeNone;
	_platform = Common::kPlatformUnknown;
	_mainSurface = nullptr;

	_vgaGraphicsStruct = new EfhGraphicsStruct(_vgaLineBuffer, 0, 0, 320, 200);
	
	_videoMode = 0;
	_graphicsStruct = nullptr;
	_mapBitmapRef = nullptr;
	_mapUnknownPtr = nullptr;
	_mapMonstersPtr = nullptr;
	_mapGameMapPtr = nullptr;

	_defaultBoxColor = 0;

	_fontDescr._widthArray = nullptr;
	_fontDescr._extraLines = nullptr;
	_fontDescr._fontData = nullptr;
	_fontDescr._charHeight = 0;
	_fontDescr._extraHorizontalSpace = _fontDescr._extraVerticalSpace = 0;

	_word31E9E = 0;
	_oldAnimImageSetId = -1;
	_animImageSetId = 254;
	_paletteTransformationConstant = 10;

	for (int i = 0; i < 12; ++i)
		_circleImageSubFileArray[i] = nullptr;

	_imageDataPtr._dataPtr = nullptr;
	_imageDataPtr._width = 0;
	_imageDataPtr._startX = _imageDataPtr._startY = 0;
	_imageDataPtr._height = 0;
	_imageDataPtr._fieldA = 0;
	_imageDataPtr._paletteTransformation = 0;
	_imageDataPtr._fieldD = 0;

	for (int i = 0; i < 3; ++i)
		_currentTileBankImageSetId[i] = -1;

	_unkRelatedToAnimImageSetId = 0;
	_techId = 0;
	_currentAnimImageSetId = 0xFF;

	for (int i = 0; i < 20; ++i)
		_portraitSubFilesArray[i] = nullptr;

	_unkAnimRelatedIndex = -1;
}

EfhEngine::~EfhEngine() {
	delete _rnd;
	delete _graphicsStruct;
	delete _vgaGraphicsStruct;
}

bool EfhEngine::hasFeature(EngineFeature f) const {
	return (f == kSupportsReturnToLauncher) || (f == kSupportsLoadingDuringRuntime) || (f == kSupportsSavingDuringRuntime);
}

const char *EfhEngine::getCopyrightString() const {
	return "Escape From Hell (C) Electronic Arts, 1990";
}

GameType EfhEngine::getGameType() const {
	return _gameType;
}

Common::Platform EfhEngine::getPlatform() const {
	return _platform;
}

Common::Error EfhEngine::run() {
	s_Engine = this;
	initialize();
	initGraphics(320, 200);

	_mainSurface = new Graphics::Surface();
	_mainSurface->create(320, 200, Graphics::PixelFormat::createFormatCLUT8());
/*
	// Setup mixer
	syncSoundSettings();
	_soundHandler->init();

	CursorMan.replaceCursor(_normalCursor, 16, 16, 0, 0, 0);
	CursorMan.showMouse(true);
*/
	initEngine();
	sub15150(-1);
	sub12A7F();
	displayLowStatusScreen(-1);

	warning("STUB - Main loop");

	return Common::kNoError;
}

void EfhEngine::initialize() {
	_rnd = new Common::RandomSource("Hell");
	_rnd->setSeed(666);                              // Kick random number generator
	_shouldQuit = false;
}

int32 EfhEngine::readFileToBuffer(Common::String &filename, uint8 *destBuffer) {
	Common::File f;
	if (!f.open(filename))
		error("Unable to find file %s", filename.c_str());

	int size = f.size();
	
	return f.read(destBuffer, size);
}

void EfhEngine::readAnimInfo() {
	Common::String fileName = "gendata\\animinfo";
	readFileToBuffer(fileName, _animInfo);
}

void EfhEngine::findMapFile(int16 mapId) {
	if (_word31E9E == 0)
		return;

	Common::String fileName = Common::String::format("map\\map.%d", mapId);
	Common::File f;
	// The original was checking for the file and eventually asking to change floppies
	if (!f.open(fileName))
		error("File not found: %s", fileName.c_str());

	f.close();
}

void EfhEngine::loadNewPortrait() {
	static int16 unkConstRelatedToAnimImageSetId[19] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2};
	_unkRelatedToAnimImageSetId = unkConstRelatedToAnimImageSetId[_techId];

	if (_currentAnimImageSetId == 200 + _unkRelatedToAnimImageSetId)
		return;

	findMapFile(_techId);
	_currentAnimImageSetId = 200 + _unkRelatedToAnimImageSetId;
	int imageSetId = _unkRelatedToAnimImageSetId + 13;
	loadImageSet(imageSetId, _portraitBuf, _portraitSubFilesArray, 0, _paletteTransformationConstant, _hiResImageBuf, _loResImageBuf);
}

void EfhEngine::loadAnimImageSet() {
	warning("STUB - loadAnimImageSet");
	if (_currentAnimImageSetId == _animImageSetId || _animImageSetId == 0xFF)
		return;

	findMapFile(_techId);

	_unkAnimRelatedIndex = 0;
	_currentAnimImageSetId = _animImageSetId;

	int16 animSetId = _animImageSetId + 17;
	loadImageSet(animSetId, _portraitBuf, _portraitSubFilesArray, 0, _paletteTransformationConstant, _hiResImageBuf, _loResImageBuf);
}

void EfhEngine::drawUnknownMenuBox() {
	warning("STUB - drawUnknownMenuBox");
}

void EfhEngine::displayAnimFrame() {
	// The original had a parameter. As it was always equal to zero, it was removed in ScummVM
	warning("STUB - displayAnimFrame");
}

void EfhEngine::displayAnimFrames(int16 animId, bool displayMenuBoxFl) {
	if (animId == 0xFF)
		return;

	_animImageSetId = animId;
	if (_animImageSetId == 0xFE)
		loadNewPortrait();
	else
		loadAnimImageSet();

	if (!displayMenuBoxFl)
		return;
	
	for (int i = 0; i < 2; ++i) {
		drawUnknownMenuBox();
		displayAnimFrame();

		if (i == 0)
			displayFctFullScreen();
	}
}

void EfhEngine::readTileFact() {
	Common::String fileName = "gendata\\tilefact";
	readFileToBuffer(fileName, _tileFact);
}

void EfhEngine::readItems() {
	Common::String fileName = "gendata\\items";
	readFileToBuffer(fileName, _items);
}

void EfhEngine::loadNPCS() {
	Common::String fileName = "gendata\\npcs";
	readFileToBuffer(fileName, _npcBuf);
}

void EfhEngine::syncSoundSettings() {
	Engine::syncSoundSettings();

//	_sound->syncVolume();
}

Common::String EfhEngine::getSavegameFilename(int slot) {
	return _targetName + Common::String::format("-%02d.SAV", slot);
}

void EfhEngine::initEngine() {
	_videoMode = 2; // In the original, 2 = VGA/MCGA, EGA = 4, Tandy = 6, cga = 8.
	memset(_bufferCharBM, 0, sizeof(_bufferCharBM));
	_graphicsStruct = new EfhGraphicsStruct;
	_graphicsStruct->copy(_vgaGraphicsStruct);

	for (int i = 0; i < 3; ++i) {
		memset(_tileBank[i], 0, sizeof(_tileBank[i]));
	}

	memset(_circleImageBuf, 0, sizeof(_circleImageBuf));
	memset(_portraitBuf, 0, sizeof(_portraitBuf));
	memset(_hiResImageBuf, 0, sizeof(_hiResImageBuf));
	memset(_loResImageBuf, 0, sizeof(_loResImageBuf));
	memset(_menuBuf, 0, sizeof(_menuBuf));
	memset(_windowWithBorderBuf, 0, sizeof(_windowWithBorderBuf));
	memset(_map, 0, sizeof(_map));
	memset(_places, 0, sizeof(_places));
	memset(_curPlace, 0, sizeof(_curPlace));
	memset(_npcBuf, 0, sizeof(_npcBuf));
	memset(_imp1, 0, sizeof(_imp1));
	memset(_imp2, 0, sizeof(_imp2));
	memset(_titleSong, 0, sizeof(_titleSong));
	memset(_items, 0, sizeof(_items));
	memset(_tileFact, 0, sizeof(_tileFact));
	memset(_animInfo, 0, sizeof(_animInfo));
	memset(_history, 0, sizeof(_history));
	memset(_techData, 0, sizeof(_techData));

	_mapBitmapRef = &_map[0];
	_mapUnknownPtr = &_map[2];
	_mapMonstersPtr = &_map[902];
	_mapGameMapPtr = &_map[2758];

	_graphicsStruct->_shiftValue = 0x2000;

	_defaultBoxColor = 7;

	// Init Font
	static uint8 fontWidthArray[96] = {
		3, 2, 3, 5, 5, 5, 5, 2, 3, 3, 5, 5, 3, 3, 2, 7, 4, 3, 4, 4, 5, 4, 4, 4, 4, 4, 3, 4, 4, 5, 4, 5, 1, 4, 4, 4,
		4, 4, 4, 4, 4, 3, 4, 4, 4, 7, 5, 4, 4, 4, 4, 4, 5, 4, 5, 7, 5, 5, 5, 3, 7, 3, 5, 0, 2, 4, 4, 4, 4, 4, 4, 4,
		4, 1, 2, 4, 1, 7, 4, 4, 4, 4, 4, 4, 3, 4, 5, 7, 4, 4, 5, 3, 0, 3, 0, 0
	};

	static uint8 fontExtraLinesArray[96] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 3,
		1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 0, 0
	};

	static Font fontData[96] = {
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x00},
		{0xA0, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x00, 0x00},
		{0x20, 0x78, 0xA0, 0x70, 0x28, 0xF0, 0x20, 0x00},
		{0xC8, 0xC8, 0x10, 0x20, 0x40, 0x98, 0x98, 0x00},
		{0x20, 0x50, 0x20, 0x40, 0xA8, 0x90, 0x68, 0x00},
		{0x40, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40},
		{0x40, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x40},
		{0x00, 0xA8, 0x70, 0xF8, 0x70, 0xA8, 0x00, 0x00},
		{0x00, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x40},
		{0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00},
		{0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00},
		{0x60, 0x90, 0x90, 0x90, 0x90, 0x90, 0x60, 0x00},
		{0x40, 0xC0, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00},
		{0x60, 0x90, 0x10, 0x20, 0x40, 0x80, 0xF0, 0x00},
		{0x60, 0x90, 0x10, 0x20, 0x10, 0x90, 0x60, 0x00},
		{0x10, 0x30, 0x50, 0x90, 0xF8, 0x10, 0x10, 0x00},
		{0xF0, 0x80, 0xE0, 0x10, 0x10, 0x90, 0x60, 0x00},
		{0x60, 0x90, 0x80, 0xE0, 0x90, 0x90, 0x60, 0x00},
		{0xF0, 0x10, 0x20, 0x20, 0x40, 0x40, 0x40, 0x00},
		{0x60, 0x90, 0x90, 0x60, 0x90, 0x90, 0x60, 0x00},
		{0x60, 0x90, 0x90, 0x70, 0x10, 0x90, 0x60, 0x00},
		{0x00, 0x00, 0x40, 0x00, 0x00, 0x40, 0x00, 0x00},
		{0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x40, 0x00},
		{0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x00},
		{0x00, 0x00, 0xF8, 0x00, 0x00, 0xF8, 0x00, 0x00},
		{0x80, 0x40, 0x20, 0x10, 0x20, 0x40, 0x80, 0x00},
		{0x70, 0x88, 0x08, 0x10, 0x20, 0x00, 0x20, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x60, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x90, 0x00},
		{0xE0, 0x90, 0x90, 0xE0, 0x90, 0x90, 0xE0, 0x00},
		{0x60, 0x90, 0x80, 0x80, 0x80, 0x90, 0x60, 0x00},
		{0xE0, 0x90, 0x90, 0x90, 0x90, 0x90, 0xE0, 0x00},
		{0xF0, 0x80, 0x80, 0xE0, 0x80, 0x80, 0xF0, 0x00},
		{0xF0, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x80, 0x00},
		{0x60, 0x90, 0x80, 0xB0, 0x90, 0x90, 0x70, 0x00},
		{0x90, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x90, 0x00},
		{0xE0, 0x40, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00},
		{0x10, 0x10, 0x10, 0x10, 0x10, 0x90, 0x60, 0x00},
		{0x90, 0x90, 0x90, 0xE0, 0x90, 0x90, 0x90, 0x00},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF0, 0x00},
		{0x82, 0xC6, 0xAA, 0x92, 0x82, 0x82, 0x82, 0x00},
		{0x88, 0x88, 0xC8, 0xA8, 0x98, 0x88, 0x88, 0x00},
		{0x60, 0x90, 0x90, 0x90, 0x90, 0x90, 0x60, 0x00},
		{0xE0, 0x90, 0x90, 0xE0, 0x80, 0x80, 0x80, 0x00},
		{0x60, 0x90, 0x90, 0x90, 0x90, 0x90, 0x60, 0x10},
		{0xE0, 0x90, 0x90, 0xE0, 0x90, 0x90, 0x90, 0x00},
		{0x60, 0x90, 0x80, 0x60, 0x10, 0x90, 0x60, 0x00},
		{0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00},
		{0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x60, 0x00},
		{0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x20, 0x00},
		{0x82, 0x82, 0x82, 0x92, 0xAA, 0xC6, 0x82, 0x00},
		{0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00},
		{0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x20, 0x00},
		{0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xF8, 0x00},
		{0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC0},
		{0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00},
		{0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x60},
		{0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x80, 0x80, 0x40, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x60, 0x10, 0x70, 0x90, 0x70, 0x00},
		{0x80, 0x80, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0x00},
		{0x00, 0x00, 0x60, 0x90, 0x80, 0x90, 0x60, 0x00},
		{0x10, 0x10, 0x70, 0x90, 0x90, 0x90, 0x70, 0x00},
		{0x00, 0x00, 0x60, 0x90, 0xF0, 0x80, 0x60, 0x00},
		{0x30, 0x40, 0xE0, 0x40, 0x40, 0x40, 0x40, 0x00},
		{0x70, 0x90, 0x90, 0x90, 0x70, 0x10, 0xE0, 0x00},
		{0x80, 0x80, 0xE0, 0x90, 0x90, 0x90, 0x90, 0x00},
		{0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00},
		{0x40, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x80},
		{0x80, 0x80, 0x90, 0x90, 0xE0, 0x90, 0x90, 0x00},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00},
		{0x00, 0x00, 0xEC, 0x92, 0x92, 0x92, 0x92, 0x00},
		{0x00, 0x00, 0xE0, 0x90, 0x90, 0x90, 0x90, 0x00},
		{0x00, 0x00, 0x60, 0x90, 0x90, 0x90, 0x60, 0x00},
		{0x00, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0x80, 0x80},
		{0x00, 0x70, 0x90, 0x90, 0x90, 0x70, 0x10, 0x10},
		{0x00, 0x00, 0xB0, 0xC0, 0x80, 0x80, 0x80, 0x00},
		{0x00, 0x00, 0x70, 0x80, 0x60, 0x10, 0xE0, 0x00},
		{0x40, 0x40, 0xE0, 0x40, 0x40, 0x40, 0x40, 0x00},
		{0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x70, 0x00},
		{0x00, 0x00, 0x88, 0x50, 0x50, 0x20, 0x20, 0x00},
		{0x00, 0x00, 0x92, 0x92, 0x92, 0x92, 0x6E, 0x00},
		{0x00, 0x00, 0x90, 0x90, 0x60, 0x90, 0x90, 0x00},
		{0x00, 0x90, 0x90, 0x90, 0x90, 0x70, 0x10, 0xE0},
		{0x00, 0x00, 0xF8, 0x10, 0x20, 0x40, 0xF8, 0x00},
		{0x20, 0x40, 0x40, 0x80, 0x40, 0x40, 0x20, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x80, 0x40, 0x40, 0x20, 0x40, 0x40, 0x80, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	};

	_fontDescr._widthArray = fontWidthArray;
	_fontDescr._extraLines = fontExtraLinesArray;
	_fontDescr._fontData = fontData;
	_fontDescr._charHeight = 8;
	_fontDescr._extraVerticalSpace = 3;
	_fontDescr._extraHorizontalSpace = 1;
	_word31E9E = 0;

	saveAnimImageSetId();

	// Save int 1C
	// Set new int 1C:
	// TODO: Implement that in the main loop
	// static uint8 counter = 0;
	// ++counter;
	// if (counter == 4) {
	//    counter = 0;
	//    tick220Fl = 1;
	// }

	// Load Title Screen
	loadImageSet(11, (uint8 *)_circleImageBuf, (uint8 **)_circleImageSubFileArray, 0, _paletteTransformationConstant, (uint8 *)_hiResImageBuf, (uint8 *)_loResImageBuf);
	displayFctFullScreen();
	sub10B77_unkDisplayFct1(_circleImageSubFileArray[0], 0, 0, _paletteTransformationConstant);
	displayFctFullScreen();
	sub10B77_unkDisplayFct1(_circleImageSubFileArray[0], 0, 0, _paletteTransformationConstant);

	// Load map tiles bitmaps
	loadImageSetToTileBank(1, 1);
	loadImageSetToTileBank(2, 2);

	// Load characters bitmaps
	loadImageSetToTileBank(3, 6);

	// Load 320*200 Menu screen
	Common::String fileName = Common::String::format("images\\imageset.%d", 10);
	readFileToBuffer(fileName, _menuBuf);

	// Load 96*64 Window with pink border and yellow bottom
	fileName = Common::String::format("images\\imageset.%d", 12);
	readFileToBuffer(fileName, _windowWithBorderBuf);

	readAnimInfo();

	displayAnimFrames(0xFE, false);
	saveAnimImageSetId();
	readTileFact();
	readItems();
	loadNPCS();

	warning("STUB - initEngine");
}

void EfhEngine::saveAnimImageSetId() {
	_oldAnimImageSetId = _animImageSetId;
	_animImageSetId = 255;
}

void EfhEngine::sub15150(int i) {
	warning("STUB - sub15150");
}

void EfhEngine::sub12A7F() {
	warning("STUB - sub12A7F");
}

void EfhEngine::displayLowStatusScreen(int i) {
	warning("STUB - displayLowStatusScreen");
}

void EfhEngine::loadImageSet(int imageSetId, uint8 *buffer, uint8 **subFilesArray, char CGAVal, char EGAVal, uint8 *destBuffer, uint8 *transfBuffer) {
	warning("STUB - loadImageSet");
}

void EfhEngine::displayFctFullScreen() {
	// CHECKME: 200 is in the original but looks suspicious.
	displayBitmapAtPos(0, 0, 319, 200);
}

void EfhEngine::displayBitmapAtPos(int16 minX, int16 minY, int16 maxX, int16 maxY) {
	warning("STUB - displayBitmapAtPos");
	_graphicsStruct->copy(_vgaGraphicsStruct);
}

void EfhEngine::sub24D92(BufferBM *bufferBM, int16 posX, int16 posY) {
	warning("STUB - sub24D92");
}

void EfhEngine::loadImageSetToTileBank(int16 tileBankId, int16 imageSetId) {
	// TODO: all the values of titleBankId and imageSetId are hardcoded. When all the calls are implemented, fix the values to avoid to have to decrease them
	int16 bankId = tileBankId - 1;
	int16 setId = imageSetId - 1;

	if (_currentTileBankImageSetId[bankId] == setId)
		return;

	_currentTileBankImageSetId[bankId] = setId;

	if (bankId == 0 || bankId == 1)
		_mapBitmapRef[bankId] = setId;

	int16 ptrIndex = bankId * 72;
	loadImageSet(setId, _tileBank[bankId], &_imageSetSubFilesArray[ptrIndex], 0, _paletteTransformationConstant, _hiResImageBuf, _loResImageBuf);
}

void EfhEngine::sub10B77_unkDisplayFct1(uint8 *imagePtr, int16 posX, int16 posY, uint8 guess_paletteTransformation) {
	uint16 height = READ_LE_INT16(imagePtr);
	uint16 width = READ_LE_INT16(imagePtr + 2);
	uint8 *imageData = imagePtr + 4;

	_imageDataPtr._fieldA = width;
	_imageDataPtr._dataPtr = imageData;
	_imageDataPtr._height = height;
	_imageDataPtr._width = width * 2;
	_imageDataPtr._startX = _imageDataPtr._startY = 0;
	
	sub24D92(&_imageDataPtr, posX, posY);
}
} // End of namespace Efh
