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

#include "backends/keymapper/keymapper.h"

#ifdef ENABLE_KEYMAPPER

#include "backends/keymapper/action.h"

#include "common/config-manager.h"
#include "common/system.h"

namespace Common {

// These magic numbers are provided by fuzzie and WebOS
static const uint32 kDelayKeyboardEventMillis = 250;
static const uint32 kDelayMouseEventMillis = 50;

void Keymapper::Domain::addKeymap(Keymap *map) {
	iterator it = find(map->getName());

	if (it != end())
		delete it->_value;

	setVal(map->getName(), map);
}

void Keymapper::Domain::deleteAllKeyMaps() {
	for (iterator it = begin(); it != end(); ++it)
		delete it->_value;

	clear();
}

Keymap *Keymapper::Domain::getKeymap(const String& name) {
	iterator it = find(name);

	if (it != end())
		return it->_value;
	else
		return 0;
}

Keymapper::Keymapper(EventManager *evtMgr)
	: _eventMan(evtMgr), _enabled(true), _hardwareInputs(0) {
	ConfigManager::Domain *confDom = ConfMan.getDomain(ConfigManager::kKeymapperDomain);

	_globalDomain.setConfigDomain(confDom);
}

Keymapper::~Keymapper() {
	delete _hardwareInputs;
}

void Keymapper::registerHardwareInputSet(HardwareInputSet *inputs) {
	if (_hardwareInputs)
		error("Hardware input set already registered");

	if (!inputs) {
		warning("No hardware input were defined, using defaults");
		inputs = new HardwareInputSet(true);
	}

	_hardwareInputs = inputs;
}

void Keymapper::addGlobalKeymap(Keymap *keymap) {
	initKeymap(_globalDomain, keymap);
}

void Keymapper::addGameKeymap(Keymap *keymap) {
	if (ConfMan.getActiveDomain() == 0)
		error("Call to Keymapper::addGameKeymap when no game loaded");

	// Detect whether the active game changed since last call.
	// If so, flush the game key configuration.
	if (_gameDomain.getConfigDomain() != ConfMan.getActiveDomain()) {
		cleanupGameKeymaps();
		_gameDomain.setConfigDomain(ConfMan.getActiveDomain());
	}

	initKeymap(_gameDomain, keymap);
}

void Keymapper::initKeymap(Domain &domain, Keymap *map) {
	if (!_hardwareInputs) {
		warning("No hardware inputs were registered yet (%s)", map->getName().c_str());
		return;
	}

	map->setConfigDomain(domain.getConfigDomain());
	map->loadMappings(_hardwareInputs);

	domain.addKeymap(map);
}

void Keymapper::cleanupGameKeymaps() {
	// Flush all game specific keymaps
	_gameDomain.deleteAllKeyMaps();

	// Now restore the stack of active maps. Re-add all global keymaps, drop
	// the game specific (=deleted) ones.
	Stack<MapRecord> newStack;

	for (Stack<MapRecord>::size_type i = 0; i < _activeMaps.size(); i++) {
		if (_activeMaps[i].global)
			newStack.push(_activeMaps[i]);
	}

	_activeMaps = newStack;
}

Keymap *Keymapper::getKeymap(const String& name, bool *globalReturn) {
	Keymap *keymap = _gameDomain.getKeymap(name);
	bool global = false;

	if (!keymap) {
		keymap = _globalDomain.getKeymap(name);
		global = true;
	}

	if (globalReturn)
		*globalReturn = global;

	return keymap;
}

bool Keymapper::pushKeymap(const String& name, bool transparent) {
	bool global;

	assert(!name.empty());
	Keymap *newMap = getKeymap(name, &global);

	if (!newMap) {
		warning("Keymap '%s' not registered", name.c_str());
		return false;
	}

	pushKeymap(newMap, transparent, global);

	return true;
}

void Keymapper::pushKeymap(Keymap *newMap, bool transparent, bool global) {
	MapRecord mr = {newMap, transparent, global};

	_activeMaps.push(mr);
}

void Keymapper::popKeymap(const char *name) {
	if (!_activeMaps.empty()) {
		if (name) {
			String topKeymapName = _activeMaps.top().keymap->getName();
			if (topKeymapName.equals(name))
				_activeMaps.pop();
			else
				warning("An attempt to pop wrong keymap was blocked (expected %s but was %s)", name, topKeymapName.c_str());
		} else {
			_activeMaps.pop();
		}
	}

}

List<Event> Keymapper::mapEvent(const Event &ev, EventSource *source) {
	if (!_enabled || _activeMaps.empty()) {
		return DefaultEventMapper::mapEvent(ev, source);
	}
	if (source && !source->allowMapping()) {
		return DefaultEventMapper::mapEvent(ev, source);
	}

	const HardwareInput *hwInput = findHardwareInput(ev);
	if (!hwInput) {
		return DefaultEventMapper::mapEvent(ev, source);
	}

	List<Event> mappedEvents;
	for (int i = _activeMaps.size() - 1; i >= 0; --i) {
		MapRecord mr = _activeMaps[i];
		debug(5, "Keymapper::mapKey keymap: %s", mr.keymap->getName().c_str());

		Action *action = mr.keymap->getMappedAction(hwInput);
		if (action) {
			IncomingEventType incomingEventType = convertToIncomingEventType(ev);
			mappedEvents.push_back(executeAction(action, incomingEventType));
			break;
		}

		if (!mr.transparent)
			break;
	}

	if (mappedEvents.empty()) {
		return DefaultEventMapper::mapEvent(ev, source);
	}

	return mappedEvents;
}

Keymapper::IncomingEventType Keymapper::convertToIncomingEventType(const Event &ev) const {
	if (ev.type == EVENT_CUSTOM_BACKEND_HARDWARE) {
		return kIncomingNonKey;
	} else if (ev.type == EVENT_KEYDOWN) {
		return kIncomingKeyDown;
	} else {
		return kIncomingKeyUp;
	}
}

Event Keymapper::executeAction(const Action *action, IncomingEventType incomingType) {
	Event evt = Event(action->event);
	EventType convertedType = convertDownToUp(evt.type);

	// hardware keys need to send up instead when they are up
	if (incomingType == kIncomingKeyUp) {
		evt.type = convertedType;
	}

	evt.mouse = _eventMan->getMousePos();

	// Check if the event is coming from a non-key hardware event
	// that is mapped to a key event
	if (incomingType == kIncomingNonKey && convertedType != EVENT_INVALID) {
		// WORKAROUND: Delay the down events coming from non-key hardware events
		// with a zero delay. This is to prevent DOWN1 DOWN2 UP1 UP2.
		addDelayedEvent(0, evt);

		// non-keys need to send up as well
		// WORKAROUND: Delay the up events coming from non-key hardware events
		// This is for engines that run scripts that check on key being down
		evt.type = convertedType;
		const uint32 delay = (convertedType == EVENT_KEYUP ? kDelayKeyboardEventMillis : kDelayMouseEventMillis);
		addDelayedEvent(delay, evt);
	}

	return evt;
}

EventType Keymapper::convertDownToUp(EventType type) {
	EventType result = EVENT_INVALID;
	switch (type) {
	case EVENT_KEYDOWN:
		result = EVENT_KEYUP;
		break;
	case EVENT_LBUTTONDOWN:
		result = EVENT_LBUTTONUP;
		break;
	case EVENT_RBUTTONDOWN:
		result = EVENT_RBUTTONUP;
		break;
	case EVENT_MBUTTONDOWN:
		result = EVENT_MBUTTONUP;
		break;
	default:
		break;
	}
	return result;
}

const HardwareInput *Keymapper::findHardwareInput(const Event &event) {
	// FIXME: Performance
	switch (event.type) {
		case EVENT_KEYDOWN:
		case EVENT_KEYUP:
			return _hardwareInputs->findHardwareInput(event.kbd);
		case EVENT_CUSTOM_BACKEND_HARDWARE:
			return _hardwareInputs->findHardwareInput(event.customType);
		default:
			return nullptr;
	}
}

void Keymapper::registerMapping(Action *action, const HardwareInput *input) {
	Keymap *keymap = action->getParent();
	keymap->registerMapping(action, input);
	keymap->saveMappings();
}

void Keymapper::clearMapping(Action *action) {
	Keymap *keymap = action->getParent();
	keymap->unregisterMapping(action);
	keymap->saveMappings();
}

} // End of namespace Common

#endif // #ifdef ENABLE_KEYMAPPER
