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

#ifndef WINTERMUTE_BREGION_H
#define WINTERMUTE_BREGION_H

#include "engines/wintermute/Base/BPoint.h"
#include "engines/wintermute/Base/BObject.h"

namespace WinterMute {

class CBRegion : public CBObject {
public:
	float _lastMimicScale;
	int _lastMimicX;
	int _lastMimicY;
	void cleanup();
	bool mimic(CBRegion *region, float scale = 100.0f, int x = 0, int y = 0);
	bool getBoundingRect(Rect32 *rect);
	bool ptInPolygon(int x, int y);
	DECLARE_PERSISTENT(CBRegion, CBObject)
	bool _active;
	int _editorSelectedPoint;
	CBRegion(CBGame *inGame);
	virtual ~CBRegion();
	bool pointInRegion(int x, int y);
	bool createRegion();
	bool loadFile(const char *filename);
	bool loadBuffer(byte *buffer, bool complete = true);
	Rect32 _rect;
	CBArray<CBPoint *, CBPoint *> _points;
	virtual bool saveAsText(CBDynBuffer *buffer, int indent, const char *nameOverride = NULL);

	// scripting interface
	virtual CScValue *scGetProperty(const char *name);
	virtual bool scSetProperty(const char *name, CScValue *value);
	virtual bool scCallMethod(CScScript *script, CScStack *stack, CScStack *thisStack, const char *name);
	virtual const char *scToString();
};

} // end of namespace WinterMute

#endif
