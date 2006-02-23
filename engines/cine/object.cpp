/* ScummVM - Scumm Interpreter
 * Copyright (C) 2006 The ScummVM project
 *
 * cinE Engine is (C) 2004-2005 by CinE Team
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
 * $URL$
 * $Id$
 *
 */

#include "cine/cine.h"

objectStruct objectTable[NUM_MAX_OBJECT];
uint16 globalVars[NUM_MAX_OBJECTDATA];
overlayHeadElement overlayHead;

void unloadAllMasks(void) {
	overlayHeadElement *current = overlayHead.next;

	while (current) {
		overlayHeadElement *next = current->next;

		free(current);

		current = next;
	}

	resetMessageHead();
}

void resetMessageHead(void) {
	overlayHead.next = NULL;
	overlayHead.previous = NULL;
}

void loadObject(char *pObjectName) {
	uint16 numEntry;
	uint16 entrySize;
	uint16 i;
	uint8 *ptr;

	checkDataDisk(-1);

	ptr = readBundleFile(findFileInBundle(pObjectName));

	processPendingUpdates(1);

	numEntry = *(uint16 *) ptr;
	ptr += 2;
	flipU16(&numEntry);

	entrySize = *(uint16 *) ptr;
	ptr += 2;
	flipU16(&entrySize);

	ASSERT(numEntry <= NUM_MAX_OBJECT);
	ASSERT(entrySize == sizeof(objectStruct));	// carefull, it's directly read to memory

	for (i = 0; i < numEntry; i++) {
		if (objectTable[i].costume != -2)	// flag is keep ?
		{
			memcpy(&objectTable[i], ptr, entrySize);

			flipU16((uint16 *) & objectTable[i].x);
			flipU16((uint16 *) & objectTable[i].y);
			flipU16(&objectTable[i].mask);
			flipU16((uint16 *) & objectTable[i].frame);
			flipU16((uint16 *) & objectTable[i].costume);
			flipU16(&objectTable[i].part);
		}

		ptr += entrySize;
	}

	if (!strcmp(pObjectName, "INTRO.OBJ")) {
		for (i = 0; i < 10; i++) {
			objectTable[i].costume = 0;
		}
	}
}

int8 removeOverlayElement(uint16 objIdx, uint16 param) {
	overlayHeadElement *currentHeadPtr = &overlayHead;
	overlayHeadElement *tempHead = currentHeadPtr;
	overlayHeadElement *tempPtr2;

	currentHeadPtr = tempHead->next;

	while (currentHeadPtr && (objIdx == currentHeadPtr->objIdx
		|| param == currentHeadPtr->type)) {
		tempHead = currentHeadPtr;

		currentHeadPtr = tempHead->next;
	}

	if (!currentHeadPtr) {
		return -1;
	}

	if (objIdx != currentHeadPtr->objIdx || param != currentHeadPtr->type) {
		return -1;
	}

	tempHead->next = currentHeadPtr->next;
	tempPtr2 = currentHeadPtr->next;

	if (!tempPtr2) {
		tempPtr2 = &overlayHead;
	}

	tempPtr2->previous = currentHeadPtr->previous;

	free(currentHeadPtr);

	return (0);
}

int16 freeOverlay(uint16 objIdx, uint16 param) {
	overlayHeadElement *currentHeadPtr = overlayHead.next;
	overlayHeadElement *tempHead = &overlayHead;
	overlayHeadElement *tempPtr2;

	while (currentHeadPtr && ((currentHeadPtr->objIdx != objIdx)
		|| (currentHeadPtr->type != param))) {
		tempHead = currentHeadPtr;
		currentHeadPtr = tempHead->next;
	}

	if (!currentHeadPtr) {
		return -1;
	}

	if (!((currentHeadPtr->objIdx == objIdx)
		&& (currentHeadPtr->type == param))) {
		return -1;
	}

	tempHead->next = currentHeadPtr->next;
	tempPtr2 = currentHeadPtr->next;

	if (!tempPtr2) {
		tempPtr2 = &overlayHead;
	}

	tempPtr2->previous = currentHeadPtr->previous;

	//TODO: fix !
	//free(currentHeadPtr);

	return 0;
}

void loadOverlayElement(uint16 objIdx, uint16 param) {
	overlayHeadElement *currentHeadPtr = &overlayHead;
	overlayHeadElement *pNewElement;

	uint16 si = objectTable[objIdx].mask;

	overlayHeadElement *tempHead = currentHeadPtr;

	currentHeadPtr = tempHead->next;

	while (currentHeadPtr
	    && (objectTable[currentHeadPtr->objIdx].mask < si)) {
		tempHead = currentHeadPtr;

		currentHeadPtr = tempHead->next;
	}

	pNewElement =
	    (overlayHeadElement *) malloc(sizeof(overlayHeadElement));

	ASSERT_PTR(pNewElement);

	pNewElement->next = tempHead->next;
	tempHead->next = pNewElement;

	pNewElement->objIdx = objIdx;
	pNewElement->type = param;

	if (!currentHeadPtr) {
		currentHeadPtr = &overlayHead;
	}

	pNewElement->previous = currentHeadPtr->previous;
	currentHeadPtr->previous = pNewElement;
}

void setupObject(uint8 objIdx, uint16 param1, uint16 param2, uint16 param3, uint16 param4) {
	objectTable[objIdx].x = param1;
	objectTable[objIdx].y = param2;
	objectTable[objIdx].mask = param3;
	objectTable[objIdx].frame = param4;

	if (!removeOverlayElement(objIdx, 0)) {
		loadOverlayElement(objIdx, 0);
	}
}

void subObjectParam(uint8 objIdx, uint8 paramIdx, int16 newValue) {
	ASSERT(objIdx <= NUM_MAX_OBJECT);

	paramIdx--;

	ASSERT(paramIdx >= 0 && paramIdx <= 5);

	switch (paramIdx) {
	case 0:
		{
			objectTable[objIdx].x -= newValue;
			break;
		}
	case 1:
		{
			objectTable[objIdx].y -= newValue;
			break;
		}
	case 2:
		{
			objectTable[objIdx].mask -= newValue;

			if (!removeOverlayElement(objIdx, 0)) {
				loadOverlayElement(objIdx, 0);
			}
			break;
		}
	case 3:
		{
			objectTable[objIdx].frame -= newValue;
			break;
		}
	case 4:
		{
			objectTable[objIdx].costume -= newValue;
			break;
		}
	case 5:
		{
			objectTable[objIdx].part -= newValue;
			break;
		}
	}
}

void addObjectParam(uint8 objIdx, uint8 paramIdx, int16 newValue) {
	ASSERT(objIdx <= NUM_MAX_OBJECT);

	paramIdx--;

	ASSERT(paramIdx >= 0 && paramIdx <= 5);

	switch (paramIdx) {
	case 0:
		{
			objectTable[objIdx].x += newValue;
			break;
		}
	case 1:
		{
			objectTable[objIdx].y += newValue;
			break;
		}
	case 2:
		{
			objectTable[objIdx].mask += newValue;

			if (!removeOverlayElement(objIdx, 0)) {
				loadOverlayElement(objIdx, 0);
			}
			break;
		}
	case 3:
		{
			objectTable[objIdx].frame += newValue;
			break;
		}
	case 4:
		{
			objectTable[objIdx].costume += newValue;
			break;
		}
	case 5:
		{
			objectTable[objIdx].part += newValue;
			break;
		}
	}
}

void modifyObjectParam(uint8 objIdx, uint8 paramIdx, int16 newValue) {
	ASSERT(objIdx <= NUM_MAX_OBJECT);

	paramIdx--;

	ASSERT(paramIdx >= 0 && paramIdx <= 5);

	switch (paramIdx) {
	case 0:
		{
			objectTable[objIdx].x = newValue;
			break;
		}
	case 1:
		{
			objectTable[objIdx].y = newValue;
			break;
		}
	case 2:
		{
			objectTable[objIdx].mask = newValue;

			if (!removeOverlayElement(objIdx, 0)) {
				loadOverlayElement(objIdx, 0);
			}
			break;
		}
	case 3:
		{
			objectTable[objIdx].frame = newValue;
			break;
		}
	case 4:
		{
			if (newValue == -1) {
				objectTable[objIdx].costume = globalVars[0];
			} else {
				objectTable[objIdx].costume = newValue;
			}
			break;
		}
	case 5:
		{
			objectTable[objIdx].part = newValue;
			break;
		}
	}
}

uint8 compareObjectParam(uint8 objIdx, uint8 param1, int16 param2) {
	uint8 compareResult = 0;

	switch (param1 - 1) {
	case 0:
		{
			if (objectTable[objIdx].x == param2) {
				compareResult |= 1;
			}

			if (objectTable[objIdx].x > param2) {
				compareResult |= 2;
			}

			if (objectTable[objIdx].x < param2) {
				compareResult |= 4;
			}

			break;
		}
	case 1:
		{
			if (objectTable[objIdx].y == param2) {
				compareResult |= 1;
			}

			if (objectTable[objIdx].y > param2) {
				compareResult |= 2;
			}

			if (objectTable[objIdx].y < param2) {
				compareResult |= 4;
			}

			break;
		}
	case 2:
		{
			if (objectTable[objIdx].mask == param2) {
				compareResult |= 1;
			}

			if (objectTable[objIdx].mask > param2) {
				compareResult |= 2;
			}

			if (objectTable[objIdx].mask < param2) {
				compareResult |= 4;
			}

			break;
		}
	case 3:
		{
			if (objectTable[objIdx].frame == param2) {
				compareResult |= 1;
			}

			if (objectTable[objIdx].frame > param2) {
				compareResult |= 2;
			}

			if (objectTable[objIdx].frame < param2) {
				compareResult |= 4;
			}

			break;
		}
	case 4:
		{
			if (objectTable[objIdx].costume == param2) {
				compareResult |= 1;
			}

			if (objectTable[objIdx].costume > param2) {
				compareResult |= 2;
			}

			if (objectTable[objIdx].costume < param2) {
				compareResult |= 4;
			}

			break;
		}
	default:
		{
			error
			    ("Unsupported compare type: %d in compareObjectParam",
			    param1 - 1);
		}
	}

	return (compareResult);
}

int16 getObjectParam(uint16 objIdx, uint16 paramIdx) {
	ASSERT(objIdx <= NUM_MAX_OBJECT);

	paramIdx--;

	ASSERT(paramIdx >= 0 && paramIdx <= 5);

	switch (paramIdx) {
	case 0:
		{
			return objectTable[objIdx].x;
			break;
		}
	case 1:
		{
			return objectTable[objIdx].y;
			break;
		}
	case 2:
		{
			return objectTable[objIdx].mask;
			break;
		}
	case 3:
		{
			return objectTable[objIdx].frame;
			break;
		}
	case 4:
		{
			return objectTable[objIdx].costume;
			break;
		}
	case 5:
		{
			return objectTable[objIdx].part;
			break;
		}
	}

	return 0;
}
