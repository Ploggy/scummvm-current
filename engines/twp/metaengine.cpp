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

#include "common/translation.h"
#include "common/savefile.h"
#include "graphics/scaler.h"
#include "image/png.h"
#include "twp/metaengine.h"
#include "twp/detection.h"
#include "twp/twp.h"
#include "twp/savegame.h"
#include "twp/time.h"

#define MAX_SAVES 99

namespace Twp {

static const ADExtraGuiOptionsMap optionsList[] = {
	{GAMEOPTION_ORIGINAL_SAVELOAD,
	 {_s("Use original save/load screens"),
	  _s("Use the original save/load screens instead of the ScummVM ones"),
	  "original_menus",
	  false,
	  0,
	  0}},
	AD_EXTRA_GUI_OPTIONS_TERMINATOR};

} // End of namespace Twp

const char *TwpMetaEngine::getName() const {
	return "twp";
}

const ADExtraGuiOptionsMap *TwpMetaEngine::getAdvancedExtraGuiOptions() const {
	return Twp::optionsList;
}

Common::Error TwpMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	*engine = new Twp::TwpEngine(syst, desc);
	return Common::kNoError;
}

bool TwpMetaEngine::hasFeature(MetaEngineFeature f) const {
	return checkExtendedSaves(f) ||
		   (f == kSupportsLoadingDuringStartup);
}

int TwpMetaEngine::getMaximumSaveSlot() const {
	return MAX_SAVES;
}

SaveStateDescriptor TwpMetaEngine::querySaveMetaInfos(const char *target, int slot) const {
	Common::String filename = Common::String::format("%s%02d.save", target, slot);
	Common::InSaveFile *f = g_system->getSavefileManager()->openForLoading(filename);

	if (f) {

		Common::InSaveFile *thumbnailFile = g_system->getSavefileManager()->openForLoading(Common::String::format("%s%02d.png", target, slot));

		// Create the return descriptor
		SaveStateDescriptor desc(this, slot, "?");

		desc.setAutosave(slot == 1);
		if (thumbnailFile) {
			Image::PNGDecoder png;
			if (png.loadStream(*thumbnailFile)) {
				Graphics::ManagedSurface *thumbnail = new Graphics::ManagedSurface();
				thumbnail->copyFrom(*png.getSurface());
				Graphics::Surface *thumbnailSmall = new Graphics::Surface();
				createThumbnail(thumbnailSmall, thumbnail);
				desc.setThumbnail(thumbnailSmall);
			}
		}
		Twp::SaveGame savegame;
		Twp::SaveGameManager::getSaveGame(f, savegame);
		Common::String time = Twp::formatTime(savegame.time, "%b %d at %H:%M");
		if (savegame.easyMode)
			time += " (casual)";
		desc.setDescription(time);
		desc.setPlayTime(savegame.gameTime * 1000);
		Twp::DateTime dt = Twp::toDateTime(savegame.time);
		desc.setSaveDate(dt.year, dt.month, dt.day);
		desc.setSaveTime(dt.hour, dt.min);

		return desc;
	}

	return SaveStateDescriptor();
}

SaveStateList TwpMetaEngine::listSaves(const char *target) const {
	Common::SaveFileManager *saveFileMan = g_system->getSavefileManager();
	Common::StringArray filenames;
	Common::String saveDesc;
	Common::String pattern = Common::String::format("%s##.save", target);

	filenames = saveFileMan->listSavefiles(pattern);

	SaveStateList saveList;
	for (Common::StringArray::const_iterator file = filenames.begin(); file != filenames.end(); ++file) {
		const char *ext = strrchr(file->c_str(), '.');
		int slot = ext ? atoi(ext - 2) : -1;

		if (slot >= 0 && slot <= MAX_SAVES) {
			Common::InSaveFile *in = g_system->getSavefileManager()->openForLoading(*file);

			if (in) {
				saveList.push_back(SaveStateDescriptor(this, slot, "?"));
			}
		}
	}

	// Sort saves based on slot number.
	Common::sort(saveList.begin(), saveList.end(), SaveStateDescriptorSlotComparator());
	return saveList;
}

#if PLUGIN_ENABLED_DYNAMIC(TWP)
REGISTER_PLUGIN_DYNAMIC(TWP, PLUGIN_TYPE_ENGINE, TwpMetaEngine);
#else
REGISTER_PLUGIN_STATIC(TWP, PLUGIN_TYPE_ENGINE, TwpMetaEngine);
#endif
