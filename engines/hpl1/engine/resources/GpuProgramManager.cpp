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
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 */

#include "hpl1/engine/resources/GpuProgramManager.h"
#include "hpl1/engine/graphics/GPUProgram.h"
#include "hpl1/engine/graphics/LowLevelGraphics.h"
#include "hpl1/engine/system/String.h"
#include "hpl1/engine/system/low_level_system.h"

namespace hpl {

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cGpuProgramManager::cGpuProgramManager(cFileSearcher *apFileSearcher, iLowLevelGraphics *apLowLevelGraphics,
									   LowLevelResources *apLowLevelResources, LowLevelSystem *apLowLevelSystem)
	: iResourceManager(apFileSearcher, apLowLevelResources, apLowLevelSystem) {
	mpLowLevelGraphics = apLowLevelGraphics;
}

cGpuProgramManager::~cGpuProgramManager() {
	DestroyAll();

	Log(" Done with Gpu programs\n");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

/**
 * Since further parameters are needed for the gpu prog this does not work...
 * For now at least.
 * \param asName
 * \return
 */
iResourceBase *cGpuProgramManager::Create(const tString &asName) {
	return NULL;
}

//-----------------------------------------------------------------------

iGpuProgram *cGpuProgramManager::CreateProgram(const tString &asName, const tString &asEntry,
											   eGpuProgramType aType) {
	return nullptr;
#if 0
	tString sPath;
	iGpuProgram *pProgram;
	pProgram = static_cast<iGpuProgram *>(FindLoadedResource(asName, sPath));

	BeginLoad(asName);

	if (pProgram == NULL && sPath != "") {
		pProgram = mpLowLevelGraphics->CreateGpuProgram(asName, aType);

		if (pProgram->CreateFromFile(sPath, asEntry) == false) {
			Error("Couldn't create program '%s'\n", asName.c_str());
			hplDelete(pProgram);
			EndLoad();
			return NULL;
		}

		AddResource(pProgram);
	}

	if (pProgram)
		pProgram->IncUserCount();
	else
		Error("Couldn't load program '%s'\n", asName.c_str());

	EndLoad();
	return pProgram;
#endif
}

//-----------------------------------------------------------------------

void cGpuProgramManager::Unload(iResourceBase *apResource) {
}
//-----------------------------------------------------------------------

void cGpuProgramManager::Destroy(iResourceBase *apResource) {
	apResource->DecUserCount();

	if (apResource->HasUsers() == false) {
		RemoveResource(apResource);
		hplDelete(apResource);
	}
}

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
} // namespace hpl
