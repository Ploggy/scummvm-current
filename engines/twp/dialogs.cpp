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

#include "gui/gui-manager.h"
#include "gui/widget.h"
#include "gui/widgets/edittext.h"
#include "gui/widgets/popup.h"
#include "gui/ThemeEval.h"

#include "twp/dialogs.h"

namespace Twp {

TwpOptionsContainerWidget::TwpOptionsContainerWidget(GuiObject *boss, const Common::String &name, const Common::String &domain) : OptionsContainerWidget(boss, name, "TwpGameOptionsDialog", false, domain) {
	GUI::StaticTextWidget *text = new GUI::StaticTextWidget(widgetsBoss(), "TwpGameOptionsDialog.VideoLabel", _("Video:"));
	text->setAlign(Graphics::TextAlign::kTextAlignStart);

	_enableToiletPaperOverGUICheckbox = new GUI::CheckboxWidget(widgetsBoss(), "TwpGameOptionsDialog.VideoCheck1",
																_("Toilet paper over"),
																_("The toilet paper in some toilets will be shown “over”.\nIt’s a joke option that has no effects on the gameplay.."));
	_enableAnnoyingInJokesGUICheckbox = new GUI::CheckboxWidget(widgetsBoss(), "TwpGameOptionsDialog.VideoCheck2",
																_("Annoying in-jokes"),
																_("The game will include in-jokes and references to past adventure games, in the form of both dialogues and objects.\nThere is a game achievement that can be obtained only if the in-jokes option is switched on."));

	text = new GUI::StaticTextWidget(widgetsBoss(), "TwpGameOptionsDialog.ConrolsLabel", _("Conrols:"));
	text->setAlign(Graphics::TextAlign::kTextAlignStart);

	_enableInvertVerbColorsGUICheckbox = new GUI::CheckboxWidget(widgetsBoss(), "TwpGameOptionsDialog.ControlsCheck1", _("Invert verb colors"), _(""));
	_enableRetroFontsGUICheckbox = new GUI::CheckboxWidget(widgetsBoss(), "TwpGameOptionsDialog.ControlsCheck2", _("Retro Fonts"), _(""));
	_enableRetroVerbsGUICheckbox = new GUI::CheckboxWidget(widgetsBoss(), "TwpGameOptionsDialog.ControlsCheck3", _("Retro Verbs"), _(""));
	_enableClassicSentenceGUICheckbox = new GUI::CheckboxWidget(widgetsBoss(), "TwpGameOptionsDialog.ControlsCheck4", _("Classic Sentence"), _(""));

	text = new GUI::StaticTextWidget(widgetsBoss(), "TwpGameOptionsDialog.TextAndSpeechLabel", _("Text and Speech:"));
	text->setAlign(Graphics::TextAlign::kTextAlignStart);

	_enableDisplayTextGUICheckbox = new GUI::CheckboxWidget(widgetsBoss(), "TwpGameOptionsDialog.TextCheck1", _("Display Text"), _(""));
	_enableHearVoiceGUICheckbox = new GUI::CheckboxWidget(widgetsBoss(), "TwpGameOptionsDialog.TextCheck2", _("Hear Vocie"), _(""));
}

void TwpOptionsContainerWidget::defineLayout(GUI::ThemeEval &layouts, const Common::String &layoutName, const Common::String &overlayedLayout) const {
	layouts.addDialog(layoutName, overlayedLayout);
	layouts.addLayout(GUI::ThemeLayout::kLayoutVertical).addPadding(0, 0, 8, 8);

	layouts.addPadding(0, 0, 8, 8)
		.addSpace(10)
		.addWidget("VideoLabel", "OptionsLabel")
		.addWidget("VideoCheck1", "Checkbox")
		.addWidget("VideoCheck2", "Checkbox")
		.addWidget("ConrolsLabel", "OptionsLabel")
		.addWidget("ControlsCheck1", "Checkbox")
		.addWidget("ControlsCheck2", "Checkbox")
		.addWidget("ControlsCheck3", "Checkbox")
		.addWidget("ControlsCheck4", "Checkbox")
		.addWidget("TextAndSpeechLabel", "OptionsLabel")
		.addWidget("TextCheck1", "Checkbox")
		.addWidget("TextCheck2", "Checkbox");

	layouts.closeLayout().closeDialog();
}

void TwpOptionsContainerWidget::load() {
	_enableToiletPaperOverGUICheckbox->setState(ConfMan.getBool("toiletPaperOver", _domain));
	_enableAnnoyingInJokesGUICheckbox->setState(ConfMan.getBool("annoyingInJokes", _domain));
	_enableInvertVerbColorsGUICheckbox->setState(ConfMan.getBool("invertVerbHighlight", _domain));
	_enableRetroFontsGUICheckbox->setState(ConfMan.getBool("retroFonts", _domain));
	_enableRetroVerbsGUICheckbox->setState(ConfMan.getBool("retroVerbs", _domain));
	_enableClassicSentenceGUICheckbox->setState(ConfMan.getBool("hudSentence", _domain));
	_enableDisplayTextGUICheckbox->setState(ConfMan.getBool("talkiesShowText", _domain));
	_enableHearVoiceGUICheckbox->setState(ConfMan.getBool("talkiesHearVoice", _domain));
}

bool TwpOptionsContainerWidget::save() {
	ConfMan.setBool("toiletPaperOver", _enableToiletPaperOverGUICheckbox->getState(), _domain);
	ConfMan.setBool("annoyingInJokes", _enableAnnoyingInJokesGUICheckbox->getState(), _domain);
	ConfMan.setBool("invertVerbHighlight", _enableInvertVerbColorsGUICheckbox->getState(), _domain);
	ConfMan.setBool("retroFonts", _enableRetroFontsGUICheckbox->getState(), _domain);
	ConfMan.setBool("retroVerbs", _enableRetroVerbsGUICheckbox->getState(), _domain);
	ConfMan.setBool("hudSentence", _enableClassicSentenceGUICheckbox->getState(), _domain);
	ConfMan.setBool("talkiesShowText", _enableDisplayTextGUICheckbox->getState(), _domain);
	ConfMan.setBool("talkiesHearVoice", _enableHearVoiceGUICheckbox->getState(), _domain);
	return true;
}

} // namespace Twp
