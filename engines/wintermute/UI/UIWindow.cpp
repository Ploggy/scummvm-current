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

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#include "engines/wintermute/dcgf.h"
#include "engines/wintermute/UI/UIWindow.h"
#include "engines/wintermute/Base/BGame.h"
#include "engines/wintermute/Base/BParser.h"
#include "engines/wintermute/Base/BActiveRect.h"
#include "engines/wintermute/Base/BDynBuffer.h"
#include "engines/wintermute/Base/BKeyboardState.h"
#include "engines/wintermute/Base/scriptables/ScValue.h"
#include "engines/wintermute/UI/UIButton.h"
#include "engines/wintermute/UI/UIEdit.h"
#include "engines/wintermute/UI/UIText.h"
#include "engines/wintermute/UI/UITiledImage.h"
#include "engines/wintermute/Base/BViewport.h"
#include "engines/wintermute/Base/font/BFontStorage.h"
#include "engines/wintermute/Base/font/BFont.h"
#include "engines/wintermute/Base/BStringTable.h"
#include "engines/wintermute/Base/scriptables/ScScript.h"
#include "engines/wintermute/Base/scriptables/ScStack.h"
#include "engines/wintermute/Base/BSprite.h"
#include "engines/wintermute/Base/BFileManager.h"
#include "engines/wintermute/PlatformSDL.h"

namespace WinterMute {

IMPLEMENT_PERSISTENT(CUIWindow, false)

//////////////////////////////////////////////////////////////////////////
CUIWindow::CUIWindow(CBGame *inGame): CUIObject(inGame) {
	CBPlatform::setRectEmpty(&_titleRect);
	CBPlatform::setRectEmpty(&_dragRect);
	_titleAlign = TAL_LEFT;
	_transparent = false;

	_backInactive = NULL;
	_fontInactive = NULL;
	_imageInactive = NULL;

	_type = UI_WINDOW;
	_canFocus = true;

	_dragging = false;
	_dragFrom.x = _dragFrom.y = 0;

	_mode = WINDOW_NORMAL;
	_shieldWindow = NULL;
	_shieldButton = NULL;

	_fadeColor = 0x00000000;
	_fadeBackground = false;

	_ready = true;
	_isMenu = false;
	_inGame = false;

	_clipContents = false;
	_viewport = NULL;

	_pauseMusic = true;
}


//////////////////////////////////////////////////////////////////////////
CUIWindow::~CUIWindow() {
	close();
	cleanup();
}


//////////////////////////////////////////////////////////////////////////
void CUIWindow::cleanup() {
	delete _shieldWindow;
	delete _shieldButton;
	delete _viewport;
	_shieldWindow = NULL;
	_shieldButton = NULL;
	_viewport = NULL;

	delete _backInactive;
	if (!_sharedFonts && _fontInactive) _gameRef->_fontStorage->removeFont(_fontInactive);
	if (!_sharedImages && _imageInactive) delete _imageInactive;

	for (int i = 0; i < _widgets.getSize(); i++) delete _widgets[i];
	_widgets.removeAll();
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::display(int offsetX, int offsetY) {
	// go exclusive
	if (_mode == WINDOW_EXCLUSIVE || _mode == WINDOW_SYSTEM_EXCLUSIVE) {
		if (!_shieldWindow) _shieldWindow = new CUIWindow(_gameRef);
		if (_shieldWindow) {
			_shieldWindow->_posX = _shieldWindow->_posY = 0;
			_shieldWindow->_width = _gameRef->_renderer->_width;
			_shieldWindow->_height = _gameRef->_renderer->_height;

			_shieldWindow->display();
		}
	} else if (_isMenu) {
		if (!_shieldButton) {
			_shieldButton = new CUIButton(_gameRef);
			_shieldButton->setName("close");
			_shieldButton->setListener(this, _shieldButton, 0);
			_shieldButton->_parent = this;
		}
		if (_shieldButton) {
			_shieldButton->_posX = _shieldButton->_posY = 0;
			_shieldButton->_width = _gameRef->_renderer->_width;
			_shieldButton->_height = _gameRef->_renderer->_height;

			_shieldButton->display();
		}
	}

	if (!_visible)
		return STATUS_OK;

	if (_fadeBackground) _gameRef->_renderer->fadeToColor(_fadeColor);

	if (_dragging) {
		_posX += (_gameRef->_mousePos.x - _dragFrom.x);
		_posY += (_gameRef->_mousePos.y - _dragFrom.y);

		_dragFrom.x = _gameRef->_mousePos.x;
		_dragFrom.y = _gameRef->_mousePos.y;
	}

	if (!_focusedWidget || (!_focusedWidget->_canFocus || _focusedWidget->_disable || !_focusedWidget->_visible)) {
		moveFocus();
	}

	bool popViewport = false;
	if (_clipContents) {
		if (!_viewport) _viewport = new CBViewport(_gameRef);
		if (_viewport) {
			_viewport->setRect(_posX + offsetX, _posY + offsetY, _posX + _width + offsetX, _posY + _height + offsetY);
			_gameRef->pushViewport(_viewport);
			popViewport = true;
		}
	}


	CUITiledImage *back = _back;
	CBSprite *image = _image;
	CBFont *font = _font;

	if (!isFocused()) {
		if (_backInactive) back = _backInactive;
		if (_imageInactive) image = _imageInactive;
		if (_fontInactive) font = _fontInactive;
	}

	if (_alphaColor != 0)
		_gameRef->_renderer->_forceAlphaColor = _alphaColor;
	if (back)
		back->display(_posX + offsetX, _posY + offsetY, _width, _height);
	if (image)
		image->draw(_posX + offsetX, _posY + offsetY, _transparent ? NULL : this);

	if (!CBPlatform::isRectEmpty(&_titleRect) && font && _text) {
		font->drawText((byte *)_text, _posX + offsetX + _titleRect.left, _posY + offsetY + _titleRect.top, _titleRect.right - _titleRect.left, _titleAlign, _titleRect.bottom - _titleRect.top);
	}

	if (!_transparent && !image)
		_gameRef->_renderer->_rectList.add(new CBActiveRect(_gameRef,  this, NULL, _posX + offsetX, _posY + offsetY, _width, _height, 100, 100, false));

	for (int i = 0; i < _widgets.getSize(); i++) {
		_widgets[i]->display(_posX + offsetX, _posY + offsetY);
	}

	if (_alphaColor != 0)
		_gameRef->_renderer->_forceAlphaColor = 0;

	if (popViewport)
		_gameRef->popViewport();

	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::loadFile(const char *filename) {
	byte *buffer = _gameRef->_fileManager->readWholeFile(filename);
	if (buffer == NULL) {
		_gameRef->LOG(0, "CUIWindow::LoadFile failed for file '%s'", filename);
		return STATUS_FAILED;
	}

	bool ret;

	_filename = new char [strlen(filename) + 1];
	strcpy(_filename, filename);

	if (DID_FAIL(ret = loadBuffer(buffer, true))) _gameRef->LOG(0, "Error parsing WINDOW file '%s'", filename);

	delete [] buffer;

	return ret;
}


TOKEN_DEF_START
TOKEN_DEF(WINDOW)
TOKEN_DEF(ALPHA_COLOR)
TOKEN_DEF(ALPHA)
TOKEN_DEF(TEMPLATE)
TOKEN_DEF(DISABLED)
TOKEN_DEF(VISIBLE)
TOKEN_DEF(BACK_INACTIVE)
TOKEN_DEF(BACK)
TOKEN_DEF(IMAGE_INACTIVE)
TOKEN_DEF(IMAGE)
TOKEN_DEF(FONT_INACTIVE)
TOKEN_DEF(FONT)
TOKEN_DEF(TITLE_ALIGN)
TOKEN_DEF(TITLE_RECT)
TOKEN_DEF(TITLE)
TOKEN_DEF(DRAG_RECT)
TOKEN_DEF(X)
TOKEN_DEF(Y)
TOKEN_DEF(WIDTH)
TOKEN_DEF(HEIGHT)
TOKEN_DEF(FADE_ALPHA)
TOKEN_DEF(FADE_COLOR)
TOKEN_DEF(CURSOR)
TOKEN_DEF(NAME)
TOKEN_DEF(BUTTON)
TOKEN_DEF(STATIC)
TOKEN_DEF(TRANSPARENT)
TOKEN_DEF(SCRIPT)
TOKEN_DEF(CAPTION)
TOKEN_DEF(PARENT_NOTIFY)
TOKEN_DEF(MENU)
TOKEN_DEF(IN_GAME)
TOKEN_DEF(CLIP_CONTENTS)
TOKEN_DEF(PAUSE_MUSIC)
TOKEN_DEF(EDITOR_PROPERTY)
TOKEN_DEF(EDIT)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
bool CUIWindow::loadBuffer(byte *buffer, bool complete) {
	TOKEN_TABLE_START(commands)
	TOKEN_TABLE(WINDOW)
	TOKEN_TABLE(ALPHA_COLOR)
	TOKEN_TABLE(ALPHA)
	TOKEN_TABLE(TEMPLATE)
	TOKEN_TABLE(DISABLED)
	TOKEN_TABLE(VISIBLE)
	TOKEN_TABLE(BACK_INACTIVE)
	TOKEN_TABLE(BACK)
	TOKEN_TABLE(IMAGE_INACTIVE)
	TOKEN_TABLE(IMAGE)
	TOKEN_TABLE(FONT_INACTIVE)
	TOKEN_TABLE(FONT)
	TOKEN_TABLE(TITLE_ALIGN)
	TOKEN_TABLE(TITLE_RECT)
	TOKEN_TABLE(TITLE)
	TOKEN_TABLE(DRAG_RECT)
	TOKEN_TABLE(X)
	TOKEN_TABLE(Y)
	TOKEN_TABLE(WIDTH)
	TOKEN_TABLE(HEIGHT)
	TOKEN_TABLE(FADE_ALPHA)
	TOKEN_TABLE(FADE_COLOR)
	TOKEN_TABLE(CURSOR)
	TOKEN_TABLE(NAME)
	TOKEN_TABLE(BUTTON)
	TOKEN_TABLE(STATIC)
	TOKEN_TABLE(TRANSPARENT)
	TOKEN_TABLE(SCRIPT)
	TOKEN_TABLE(CAPTION)
	TOKEN_TABLE(PARENT_NOTIFY)
	TOKEN_TABLE(MENU)
	TOKEN_TABLE(IN_GAME)
	TOKEN_TABLE(CLIP_CONTENTS)
	TOKEN_TABLE(PAUSE_MUSIC)
	TOKEN_TABLE(EDITOR_PROPERTY)
	TOKEN_TABLE(EDIT)
	TOKEN_TABLE_END

	byte *params;
	int cmd = 2;
	CBParser parser(_gameRef);

	int fadeR = 0, fadeG = 0, fadeB = 0, fadeA = 0;
	int ar = 0, ag = 0, ab = 0, alpha = 0;

	if (complete) {
		if (parser.getCommand((char **)&buffer, commands, (char **)&params) != TOKEN_WINDOW) {
			_gameRef->LOG(0, "'WINDOW' keyword expected.");
			return STATUS_FAILED;
		}
		buffer = params;
	}

	while (cmd >= PARSERR_TOKENNOTFOUND && (cmd = parser.getCommand((char **)&buffer, commands, (char **)&params)) >= PARSERR_TOKENNOTFOUND) {
		switch (cmd) {
		case TOKEN_TEMPLATE:
			if (DID_FAIL(loadFile((char *)params))) cmd = PARSERR_GENERIC;
			break;

		case TOKEN_NAME:
			setName((char *)params);
			break;

		case TOKEN_CAPTION:
			setCaption((char *)params);
			break;

		case TOKEN_BACK:
			delete _back;
			_back = new CUITiledImage(_gameRef);
			if (!_back || DID_FAIL(_back->loadFile((char *)params))) {
				delete _back;
				_back = NULL;
				cmd = PARSERR_GENERIC;
			}
			break;

		case TOKEN_BACK_INACTIVE:
			delete _backInactive;
			_backInactive = new CUITiledImage(_gameRef);
			if (!_backInactive || DID_FAIL(_backInactive->loadFile((char *)params))) {
				delete _backInactive;
				_backInactive = NULL;
				cmd = PARSERR_GENERIC;
			}
			break;

		case TOKEN_IMAGE:
			delete _image;
			_image = new CBSprite(_gameRef);
			if (!_image || DID_FAIL(_image->loadFile((char *)params))) {
				delete _image;
				_image = NULL;
				cmd = PARSERR_GENERIC;
			}
			break;

		case TOKEN_IMAGE_INACTIVE:
			delete _imageInactive,
			       _imageInactive = new CBSprite(_gameRef);
			if (!_imageInactive || DID_FAIL(_imageInactive->loadFile((char *)params))) {
				delete _imageInactive;
				_imageInactive = NULL;
				cmd = PARSERR_GENERIC;
			}
			break;

		case TOKEN_FONT:
			if (_font) _gameRef->_fontStorage->removeFont(_font);
			_font = _gameRef->_fontStorage->addFont((char *)params);
			if (!_font) cmd = PARSERR_GENERIC;
			break;

		case TOKEN_FONT_INACTIVE:
			if (_fontInactive) _gameRef->_fontStorage->removeFont(_fontInactive);
			_fontInactive = _gameRef->_fontStorage->addFont((char *)params);
			if (!_fontInactive) cmd = PARSERR_GENERIC;
			break;

		case TOKEN_TITLE:
			setText((char *)params);
			_gameRef->_stringTable->expand(&_text);
			break;

		case TOKEN_TITLE_ALIGN:
			if (scumm_stricmp((char *)params, "left") == 0) _titleAlign = TAL_LEFT;
			else if (scumm_stricmp((char *)params, "right") == 0) _titleAlign = TAL_RIGHT;
			else _titleAlign = TAL_CENTER;
			break;

		case TOKEN_TITLE_RECT:
			parser.scanStr((char *)params, "%d,%d,%d,%d", &_titleRect.left, &_titleRect.top, &_titleRect.right, &_titleRect.bottom);
			break;

		case TOKEN_DRAG_RECT:
			parser.scanStr((char *)params, "%d,%d,%d,%d", &_dragRect.left, &_dragRect.top, &_dragRect.right, &_dragRect.bottom);
			break;

		case TOKEN_X:
			parser.scanStr((char *)params, "%d", &_posX);
			break;

		case TOKEN_Y:
			parser.scanStr((char *)params, "%d", &_posY);
			break;

		case TOKEN_WIDTH:
			parser.scanStr((char *)params, "%d", &_width);
			break;

		case TOKEN_HEIGHT:
			parser.scanStr((char *)params, "%d", &_height);
			break;

		case TOKEN_CURSOR:
			delete _cursor;
			_cursor = new CBSprite(_gameRef);
			if (!_cursor || DID_FAIL(_cursor->loadFile((char *)params))) {
				delete _cursor;
				_cursor = NULL;
				cmd = PARSERR_GENERIC;
			}
			break;

		case TOKEN_BUTTON: {
			CUIButton *btn = new CUIButton(_gameRef);
			if (!btn || DID_FAIL(btn->loadBuffer(params, false))) {
				delete btn;
				btn = NULL;
				cmd = PARSERR_GENERIC;
			} else {
				btn->_parent = this;
				_widgets.add(btn);
			}
		}
		break;

		case TOKEN_STATIC: {
			CUIText *text = new CUIText(_gameRef);
			if (!text || DID_FAIL(text->loadBuffer(params, false))) {
				delete text;
				text = NULL;
				cmd = PARSERR_GENERIC;
			} else {
				text->_parent = this;
				_widgets.add(text);
			}
		}
		break;

		case TOKEN_EDIT: {
			CUIEdit *edit = new CUIEdit(_gameRef);
			if (!edit || DID_FAIL(edit->loadBuffer(params, false))) {
				delete edit;
				edit = NULL;
				cmd = PARSERR_GENERIC;
			} else {
				edit->_parent = this;
				_widgets.add(edit);
			}
		}
		break;

		case TOKEN_WINDOW: {
			CUIWindow *win = new CUIWindow(_gameRef);
			if (!win || DID_FAIL(win->loadBuffer(params, false))) {
				delete win;
				win = NULL;
				cmd = PARSERR_GENERIC;
			} else {
				win->_parent = this;
				_widgets.add(win);
			}
		}
		break;


		case TOKEN_TRANSPARENT:
			parser.scanStr((char *)params, "%b", &_transparent);
			break;

		case TOKEN_SCRIPT:
			addScript((char *)params);
			break;

		case TOKEN_PARENT_NOTIFY:
			parser.scanStr((char *)params, "%b", &_parentNotify);
			break;

		case TOKEN_PAUSE_MUSIC:
			parser.scanStr((char *)params, "%b", &_pauseMusic);
			break;

		case TOKEN_DISABLED:
			parser.scanStr((char *)params, "%b", &_disable);
			break;

		case TOKEN_VISIBLE:
			parser.scanStr((char *)params, "%b", &_visible);
			break;

		case TOKEN_MENU:
			parser.scanStr((char *)params, "%b", &_isMenu);
			break;

		case TOKEN_IN_GAME:
			parser.scanStr((char *)params, "%b", &_inGame);
			break;

		case TOKEN_CLIP_CONTENTS:
			parser.scanStr((char *)params, "%b", &_clipContents);
			break;

		case TOKEN_FADE_COLOR:
			parser.scanStr((char *)params, "%d,%d,%d", &fadeR, &fadeG, &fadeB);
			_fadeBackground = true;
			break;

		case TOKEN_FADE_ALPHA:
			parser.scanStr((char *)params, "%d", &fadeA);
			_fadeBackground = true;
			break;

		case TOKEN_EDITOR_PROPERTY:
			parseEditorProperty(params, false);
			break;

		case TOKEN_ALPHA_COLOR:
			parser.scanStr((char *)params, "%d,%d,%d", &ar, &ag, &ab);
			break;

		case TOKEN_ALPHA:
			parser.scanStr((char *)params, "%d", &alpha);
			break;


		default:
			if (DID_FAIL(_gameRef->windowLoadHook(this, (char **)&buffer, (char **)params))) {
				cmd = PARSERR_GENERIC;
			}
		}
	}
	if (cmd == PARSERR_TOKENNOTFOUND) {
		_gameRef->LOG(0, "Syntax error in WINDOW definition");
		return STATUS_FAILED;
	}
	if (cmd == PARSERR_GENERIC) {
		_gameRef->LOG(0, "Error loading WINDOW definition");
		return STATUS_FAILED;
	}

	correctSize();

	if (alpha != 0 && ar == 0 && ag == 0 && ab == 0) {
		ar = ag = ab = 255;
	}
	_alphaColor = BYTETORGBA(ar, ag, ab, alpha);

	if (_fadeBackground)
		_fadeColor = BYTETORGBA(fadeR, fadeG, fadeB, fadeA);

	_focusedWidget = NULL;

	return STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CUIWindow::saveAsText(CBDynBuffer *buffer, int indent) {
	buffer->putTextIndent(indent, "WINDOW\n");
	buffer->putTextIndent(indent, "{\n");

	buffer->putTextIndent(indent + 2, "NAME=\"%s\"\n", _name);
	buffer->putTextIndent(indent + 2, "CAPTION=\"%s\"\n", getCaption());

	buffer->putTextIndent(indent + 2, "\n");

	if (_back && _back->_filename)
		buffer->putTextIndent(indent + 2, "BACK=\"%s\"\n", _back->_filename);
	if (_backInactive && _backInactive->_filename)
		buffer->putTextIndent(indent + 2, "BACK_INACTIVE=\"%s\"\n", _backInactive->_filename);

	if (_image && _image->_filename)
		buffer->putTextIndent(indent + 2, "IMAGE=\"%s\"\n", _image->_filename);
	if (_imageInactive && _imageInactive->_filename)
		buffer->putTextIndent(indent + 2, "IMAGE_INACTIVE=\"%s\"\n", _imageInactive->_filename);

	if (_font && _font->_filename)
		buffer->putTextIndent(indent + 2, "FONT=\"%s\"\n", _font->_filename);
	if (_fontInactive && _fontInactive->_filename)
		buffer->putTextIndent(indent + 2, "FONT_INACTIVE=\"%s\"\n", _fontInactive->_filename);

	if (_cursor && _cursor->_filename)
		buffer->putTextIndent(indent + 2, "CURSOR=\"%s\"\n", _cursor->_filename);

	buffer->putTextIndent(indent + 2, "\n");

	if (_text)
		buffer->putTextIndent(indent + 2, "TITLE=\"%s\"\n", _text);

	switch (_titleAlign) {
	case TAL_LEFT:
		buffer->putTextIndent(indent + 2, "TITLE_ALIGN=\"%s\"\n", "left");
		break;
	case TAL_RIGHT:
		buffer->putTextIndent(indent + 2, "TITLE_ALIGN=\"%s\"\n", "right");
		break;
	case TAL_CENTER:
		buffer->putTextIndent(indent + 2, "TITLE_ALIGN=\"%s\"\n", "center");
		break;
	default:
		error("UIWindow::SaveAsText - Unhandled enum-value NUM_TEXT_ALIGN");
	}

	if (!CBPlatform::isRectEmpty(&_titleRect)) {
		buffer->putTextIndent(indent + 2, "TITLE_RECT { %d, %d, %d, %d }\n", _titleRect.left, _titleRect.top, _titleRect.right, _titleRect.bottom);
	}

	if (!CBPlatform::isRectEmpty(&_dragRect)) {
		buffer->putTextIndent(indent + 2, "DRAG_RECT { %d, %d, %d, %d }\n", _dragRect.left, _dragRect.top, _dragRect.right, _dragRect.bottom);
	}

	buffer->putTextIndent(indent + 2, "\n");

	buffer->putTextIndent(indent + 2, "X=%d\n", _posX);
	buffer->putTextIndent(indent + 2, "Y=%d\n", _posY);
	buffer->putTextIndent(indent + 2, "WIDTH=%d\n", _width);
	buffer->putTextIndent(indent + 2, "HEIGHT=%d\n", _height);

	buffer->putTextIndent(indent + 2, "DISABLED=%s\n", _disable ? "TRUE" : "FALSE");
	buffer->putTextIndent(indent + 2, "VISIBLE=%s\n", _visible ? "TRUE" : "FALSE");
	buffer->putTextIndent(indent + 2, "PARENT_NOTIFY=%s\n", _parentNotify ? "TRUE" : "FALSE");

	buffer->putTextIndent(indent + 2, "TRANSPARENT=%s\n", _transparent ? "TRUE" : "FALSE");
	buffer->putTextIndent(indent + 2, "PAUSE_MUSIC=%s\n", _pauseMusic ? "TRUE" : "FALSE");
	buffer->putTextIndent(indent + 2, "MENU=%s\n", _isMenu ? "TRUE" : "FALSE");
	buffer->putTextIndent(indent + 2, "IN_GAME=%s\n", _inGame ? "TRUE" : "FALSE");
	buffer->putTextIndent(indent + 2, "CLIP_CONTENTS=%s\n", _clipContents ? "TRUE" : "FALSE");

	buffer->putTextIndent(indent + 2, "\n");

	if (_fadeBackground) {
		buffer->putTextIndent(indent + 2, "FADE_COLOR { %d, %d, %d }\n", RGBCOLGetR(_fadeColor), RGBCOLGetG(_fadeColor), RGBCOLGetB(_fadeColor));
		buffer->putTextIndent(indent + 2, "FADE_ALPHA=%d\n", RGBCOLGetA(_fadeColor));
	}

	buffer->putTextIndent(indent + 2, "ALPHA_COLOR { %d, %d, %d }\n", RGBCOLGetR(_alphaColor), RGBCOLGetG(_alphaColor), RGBCOLGetB(_alphaColor));
	buffer->putTextIndent(indent + 2, "ALPHA=%d\n", RGBCOLGetA(_alphaColor));

	buffer->putTextIndent(indent + 2, "\n");

	// scripts
	for (int i = 0; i < _scripts.getSize(); i++) {
		buffer->putTextIndent(indent + 2, "SCRIPT=\"%s\"\n", _scripts[i]->_filename);
	}

	buffer->putTextIndent(indent + 2, "\n");

	// editor properties
	CBBase::saveAsText(buffer, indent + 2);

	// controls
	for (int i = 0; i < _widgets.getSize(); i++)
		_widgets[i]->saveAsText(buffer, indent + 2);


	buffer->putTextIndent(indent, "}\n");
	return STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CUIWindow::enableWidget(const char *name, bool Enable) {
	for (int i = 0; i < _widgets.getSize(); i++) {
		if (scumm_stricmp(_widgets[i]->_name, name) == 0) _widgets[i]->_disable = !Enable;
	}
	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::showWidget(const char *name, bool Visible) {
	for (int i = 0; i < _widgets.getSize(); i++) {
		if (scumm_stricmp(_widgets[i]->_name, name) == 0) _widgets[i]->_visible = Visible;
	}
	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
bool CUIWindow::scCallMethod(CScScript *script, CScStack *stack, CScStack *thisStack, const char *name) {
	//////////////////////////////////////////////////////////////////////////
	// GetWidget / GetControl
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(name, "GetWidget") == 0 || strcmp(name, "GetControl") == 0) {
		stack->correctParams(1);
		CScValue *val = stack->pop();
		if (val->getType() == VAL_INT) {
			int widget = val->getInt();
			if (widget < 0 || widget >= _widgets.getSize()) stack->pushNULL();
			else stack->pushNative(_widgets[widget], true);
		} else {
			for (int i = 0; i < _widgets.getSize(); i++) {
				if (scumm_stricmp(_widgets[i]->_name, val->getString()) == 0) {
					stack->pushNative(_widgets[i], true);
					return STATUS_OK;
				}
			}
			stack->pushNULL();
		}

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// SetInactiveFont
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "SetInactiveFont") == 0) {
		stack->correctParams(1);

		if (_fontInactive) _gameRef->_fontStorage->removeFont(_fontInactive);
		_fontInactive = _gameRef->_fontStorage->addFont(stack->pop()->getString());
		stack->pushBool(_fontInactive != NULL);

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// SetInactiveImage
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "SetInactiveImage") == 0) {
		stack->correctParams(1);

		delete _imageInactive;
		_imageInactive = new CBSprite(_gameRef);
		const char *filename = stack->pop()->getString();
		if (!_imageInactive || DID_FAIL(_imageInactive->loadFile(filename))) {
			delete _imageInactive;
			_imageInactive = NULL;
			stack->pushBool(false);
		} else stack->pushBool(true);

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// GetInactiveImage
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "GetInactiveImage") == 0) {
		stack->correctParams(0);
		if (!_imageInactive || !_imageInactive->_filename) stack->pushNULL();
		else stack->pushString(_imageInactive->_filename);

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// GetInactiveImageObject
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "GetInactiveImageObject") == 0) {
		stack->correctParams(0);
		if (!_imageInactive) stack->pushNULL();
		else stack->pushNative(_imageInactive, true);

		return STATUS_OK;
	}


	//////////////////////////////////////////////////////////////////////////
	// Close
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "Close") == 0) {
		stack->correctParams(0);
		stack->pushBool(DID_SUCCEED(close()));
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// GoExclusive
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "GoExclusive") == 0) {
		stack->correctParams(0);
		goExclusive();
		script->waitFor(this);
		stack->pushNULL();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// GoSystemExclusive
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "GoSystemExclusive") == 0) {
		stack->correctParams(0);
		goSystemExclusive();
		script->waitFor(this);
		stack->pushNULL();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Center
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "Center") == 0) {
		stack->correctParams(0);
		_posX = (_gameRef->_renderer->_width - _width) / 2;
		_posY = (_gameRef->_renderer->_height - _height) / 2;
		stack->pushNULL();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// LoadFromFile
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "LoadFromFile") == 0) {
		stack->correctParams(1);

		CScValue *val = stack->pop();
		cleanup();
		if (!val->isNULL()) {
			stack->pushBool(DID_SUCCEED(loadFile(val->getString())));
		} else stack->pushBool(true);

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// CreateButton
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "CreateButton") == 0) {
		stack->correctParams(1);
		CScValue *val = stack->pop();

		CUIButton *btn = new CUIButton(_gameRef);
		if (!val->isNULL()) btn->setName(val->getString());
		stack->pushNative(btn, true);

		btn->_parent = this;
		_widgets.add(btn);

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// CreateStatic
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "CreateStatic") == 0) {
		stack->correctParams(1);
		CScValue *val = stack->pop();

		CUIText *sta = new CUIText(_gameRef);
		if (!val->isNULL()) sta->setName(val->getString());
		stack->pushNative(sta, true);

		sta->_parent = this;
		_widgets.add(sta);

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// CreateEditor
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "CreateEditor") == 0) {
		stack->correctParams(1);
		CScValue *val = stack->pop();

		CUIEdit *edi = new CUIEdit(_gameRef);
		if (!val->isNULL()) edi->setName(val->getString());
		stack->pushNative(edi, true);

		edi->_parent = this;
		_widgets.add(edi);

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// CreateWindow
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "CreateWindow") == 0) {
		stack->correctParams(1);
		CScValue *val = stack->pop();

		CUIWindow *win = new CUIWindow(_gameRef);
		if (!val->isNULL()) win->setName(val->getString());
		stack->pushNative(win, true);

		win->_parent = this;
		_widgets.add(win);

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// DeleteControl / DeleteButton / DeleteStatic / DeleteEditor / DeleteWindow
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "DeleteControl") == 0 || strcmp(name, "DeleteButton") == 0 || strcmp(name, "DeleteStatic") == 0 || strcmp(name, "DeleteEditor") == 0 || strcmp(name, "DeleteWindow") == 0) {
		stack->correctParams(1);
		CScValue *val = stack->pop();
		CUIObject *obj = (CUIObject *)val->getNative();

		for (int i = 0; i < _widgets.getSize(); i++) {
			if (_widgets[i] == obj) {
				delete _widgets[i];
				_widgets.removeAt(i);
				if (val->getType() == VAL_VARIABLE_REF) val->setNULL();
			}
		}
		stack->pushNULL();
		return STATUS_OK;
	} else if DID_SUCCEED(_gameRef->windowScriptMethodHook(this, script, stack, name)) return STATUS_OK;

	else return CUIObject::scCallMethod(script, stack, thisStack, name);
}


//////////////////////////////////////////////////////////////////////////
CScValue *CUIWindow::scGetProperty(const char *name) {
	_scValue->setNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(name, "Type") == 0) {
		_scValue->setString("window");
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// NumWidgets / NumControls (RO)
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "NumWidgets") == 0 || strcmp(name, "NumControls") == 0) {
		_scValue->setInt(_widgets.getSize());
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Exclusive
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "Exclusive") == 0) {
		_scValue->setBool(_mode == WINDOW_EXCLUSIVE);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// SystemExclusive
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "SystemExclusive") == 0) {
		_scValue->setBool(_mode == WINDOW_SYSTEM_EXCLUSIVE);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Menu
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "Menu") == 0) {
		_scValue->setBool(_isMenu);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// InGame
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "InGame") == 0) {
		_scValue->setBool(_inGame);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// PauseMusic
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "PauseMusic") == 0) {
		_scValue->setBool(_pauseMusic);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// ClipContents
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "ClipContents") == 0) {
		_scValue->setBool(_clipContents);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Transparent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "Transparent") == 0) {
		_scValue->setBool(_transparent);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// FadeColor
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "FadeColor") == 0) {
		_scValue->setInt((int)_fadeColor);
		return _scValue;
	}

	else return CUIObject::scGetProperty(name);
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::scSetProperty(const char *name, CScValue *value) {
	//////////////////////////////////////////////////////////////////////////
	// Name
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(name, "Name") == 0) {
		setName(value->getString());
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Menu
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "Menu") == 0) {
		_isMenu = value->getBool();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// InGame
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "InGame") == 0) {
		_inGame = value->getBool();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// PauseMusic
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "PauseMusic") == 0) {
		_pauseMusic = value->getBool();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// ClipContents
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "ClipContents") == 0) {
		_clipContents = value->getBool();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Transparent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "Transparent") == 0) {
		_transparent = value->getBool();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// FadeColor
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "FadeColor") == 0) {
		_fadeColor = (uint32)value->getInt();
		_fadeBackground = (_fadeColor != 0);
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Exclusive
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "Exclusive") == 0) {
		if (value->getBool())
			goExclusive();
		else {
			close();
			_visible = true;
		}
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// SystemExclusive
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "SystemExclusive") == 0) {
		if (value->getBool())
			goSystemExclusive();
		else {
			close();
			_visible = true;
		}
		return STATUS_OK;
	}

	else return CUIObject::scSetProperty(name, value);
}


//////////////////////////////////////////////////////////////////////////
const char *CUIWindow::scToString() {
	return "[window]";
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::handleKeypress(Common::Event *event, bool printable) {
//TODO
	if (event->type == Common::EVENT_KEYDOWN && event->kbd.keycode == Common::KEYCODE_TAB) {
		return DID_SUCCEED(moveFocus(!CBKeyboardState::isShiftDown()));
	} else {
		if (_focusedWidget) return _focusedWidget->handleKeypress(event, printable);
		else return false;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::handleMouseWheel(int Delta) {
	if (_focusedWidget) return _focusedWidget->handleMouseWheel(Delta);
	else return false;
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::handleMouse(TMouseEvent event, TMouseButton button) {
	bool res = CUIObject::handleMouse(event, button);

	// handle window dragging
	if (!CBPlatform::isRectEmpty(&_dragRect)) {
		// start drag
		if (event == MOUSE_CLICK && button == MOUSE_BUTTON_LEFT) {
			Rect32 dragRect = _dragRect;
			int offsetX, offsetY;
			getTotalOffset(&offsetX, &offsetY);
			CBPlatform::offsetRect(&dragRect, _posX + offsetX, _posY + offsetY);

			if (CBPlatform::ptInRect(&dragRect, _gameRef->_mousePos)) {
				_dragFrom.x = _gameRef->_mousePos.x;
				_dragFrom.y = _gameRef->_mousePos.y;
				_dragging = true;
			}
		}
		// end drag
		else if (_dragging && event == MOUSE_RELEASE && button == MOUSE_BUTTON_LEFT) {
			_dragging = false;
		}
	}

	return res;
}



//////////////////////////////////////////////////////////////////////////
bool CUIWindow::persist(CBPersistMgr *persistMgr) {

	CUIObject::persist(persistMgr);

	persistMgr->transfer(TMEMBER(_backInactive));
	persistMgr->transfer(TMEMBER(_clipContents));
	persistMgr->transfer(TMEMBER(_dragFrom));
	persistMgr->transfer(TMEMBER(_dragging));
	persistMgr->transfer(TMEMBER(_dragRect));
	persistMgr->transfer(TMEMBER(_fadeBackground));
	persistMgr->transfer(TMEMBER(_fadeColor));
	persistMgr->transfer(TMEMBER(_fontInactive));
	persistMgr->transfer(TMEMBER(_imageInactive));
	persistMgr->transfer(TMEMBER(_inGame));
	persistMgr->transfer(TMEMBER(_isMenu));
	persistMgr->transfer(TMEMBER_INT(_mode));
	persistMgr->transfer(TMEMBER(_shieldButton));
	persistMgr->transfer(TMEMBER(_shieldWindow));
	persistMgr->transfer(TMEMBER_INT(_titleAlign));
	persistMgr->transfer(TMEMBER(_titleRect));
	persistMgr->transfer(TMEMBER(_transparent));
	persistMgr->transfer(TMEMBER(_viewport));
	persistMgr->transfer(TMEMBER(_pauseMusic));

	_widgets.persist(persistMgr);

	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::moveFocus(bool forward) {
	int i;
	bool found = false;
	for (i = 0; i < _widgets.getSize(); i++) {
		if (_widgets[i] == _focusedWidget) {
			found = true;
			break;
		}
	}
	if (!found) _focusedWidget = NULL;

	if (!_focusedWidget) {
		if (_widgets.getSize() > 0) i = 0;
		else return STATUS_OK;
	}

	int numTries = 0;
	bool done = false;

	while (numTries <= _widgets.getSize()) {
		if (_widgets[i] != _focusedWidget && _widgets[i]->_canFocus && _widgets[i]->_visible && !_widgets[i]->_disable) {
			_focusedWidget = _widgets[i];
			done = true;
			break;
		}

		if (forward) {
			i++;
			if (i >= _widgets.getSize()) i = 0;
		} else {
			i--;
			if (i < 0) i = _widgets.getSize() - 1;
		}
		numTries++;
	}

	return done ? STATUS_OK : STATUS_FAILED;
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::goExclusive() {
	if (_mode == WINDOW_EXCLUSIVE) return STATUS_OK;

	if (_mode == WINDOW_NORMAL) {
		_ready = false;
		_mode = WINDOW_EXCLUSIVE;
		_visible = true;
		_disable = false;
		_gameRef->focusWindow(this);
		return STATUS_OK;
	} else return STATUS_FAILED;
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::goSystemExclusive() {
	if (_mode == WINDOW_SYSTEM_EXCLUSIVE) return STATUS_OK;

	makeFreezable(false);

	_mode = WINDOW_SYSTEM_EXCLUSIVE;
	_ready = false;
	_visible = true;
	_disable = false;
	_gameRef->focusWindow(this);

	_gameRef->freeze(_pauseMusic);
	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::close() {
	if (_mode == WINDOW_SYSTEM_EXCLUSIVE) {
		_gameRef->unfreeze();
	}

	_mode = WINDOW_NORMAL;
	_visible = false;
	_ready = true;

	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::listen(CBScriptHolder *param1, uint32 param2) {
	CUIObject *obj = (CUIObject *)param1;

	switch (obj->_type) {
	case UI_BUTTON:
		if (scumm_stricmp(obj->_name, "close") == 0) close();
		else return CBObject::listen(param1, param2);
		break;
	default:
		return CBObject::listen(param1, param2);
	}

	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
void CUIWindow::makeFreezable(bool freezable) {
	for (int i = 0; i < _widgets.getSize(); i++)
		_widgets[i]->makeFreezable(freezable);

	CBObject::makeFreezable(freezable);
}


//////////////////////////////////////////////////////////////////////////
bool CUIWindow::getWindowObjects(CBArray<CUIObject *, CUIObject *> &objects, bool interactiveOnly) {
	for (int i = 0; i < _widgets.getSize(); i++) {
		CUIObject *control = _widgets[i];
		if (control->_disable && interactiveOnly) continue;

		switch (control->_type) {
		case UI_WINDOW:
			((CUIWindow *)control)->getWindowObjects(objects, interactiveOnly);
			break;

		case UI_BUTTON:
		case UI_EDIT:
			objects.add(control);
			break;

		default:
			if (!interactiveOnly) objects.add(control);
		}
	}
	return STATUS_OK;
}

} // end of namespace WinterMute
