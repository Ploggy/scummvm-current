MODULE := engines/ags

MODULE_OBJS = \
	ags.o \
	events.o \
	game_scanner.o \
	metaengine.o \
	music.o \
	lib/aastr-0.1.1/aarot.o \
	lib/aastr-0.1.1/aastr.o \
	lib/aastr-0.1.1/aautil.o \
	lib/alfont/alfont.o \
	lib/allegro.o \
	lib/allegro/colblend.o \
	lib/allegro/color.o \
	lib/allegro/config.o \
	lib/allegro/draw.o \
	lib/allegro/error.o \
	lib/allegro/file.o \
	lib/allegro/fixed.o \
	lib/allegro/flood.o \
	lib/allegro/gfx.o \
	lib/allegro/graphics.o \
	lib/allegro/keyboard.o \
	lib/allegro/math.o \
	lib/allegro/mouse.o \
	lib/allegro/surface.o \
	lib/allegro/system.o \
	lib/allegro/unicode.o \
	lib/hq2x/hq2x3x.o \
	lib/std/std.o \
	lib/system/datetime.o \
	shared/ac/dynobj/scriptaudioclip.o \
	shared/ac/audiocliptype.o \
	shared/ac/characterinfo.o \
	shared/ac/common.o \
	shared/ac/dialogtopic.o \
	shared/ac/gamesetupstruct.o \
	shared/ac/gamesetupstructbase.o \
	shared/ac/inventoryiteminfo.o \
	shared/ac/mousecursor.o \
	shared/ac/spritecache.o \
	shared/ac/view.o \
	shared/ac/wordsdictionary.o \
	shared/core/asset.o \
	shared/core/assetmanager.o \
	shared/debugging/debugmanager.o \
	shared/font/fonts.o \
	shared/font/ttffontrenderer.o \
	shared/font/wfnfont.o \
	shared/font/wfnfontrenderer.o \
	shared/game/customproperties.o \
	shared/game/interactions.o \
	shared/game/main_game_file.o \
	shared/game/room_file.o \
	shared/game/room_file_deprecated.o \
	shared/game/roomstruct.o \
	shared/gfx/allegrobitmap.o \
	shared/gfx/bitmap.o \
	shared/gfx/image.o \
	shared/gui/guibutton.o \
	shared/gui/guiinv.o \
	shared/gui/guilabel.o \
	shared/gui/guilistbox.o \
	shared/gui/guimain.o \
	shared/gui/guiobject.o \
	shared/gui/guislider.o \
	shared/gui/guitextbox.o \
	shared/script/cc_error.o \
	shared/script/cc_options.o \
	shared/script/cc_script.o \
	shared/util/alignedstream.o \
	shared/util/bufferedstream.o \
	shared/util/compress.o \
	shared/util/datastream.o \
	shared/util/directory.o \
	shared/util/file.o \
	shared/util/filestream.o \
	shared/util/geometry.o \
	shared/util/inifile.o \
	shared/util/ini_util.o \
	shared/util/lzw.o \
	shared/util/misc.o \
	shared/util/mutifilelib.o \
	shared/util/path.o \
	shared/util/proxystream.o \
	shared/util/stdio_compat.o \
	shared/util/stream.o \
	shared/util/string.o \
	shared/util/string_compat.o \
	shared/util/string_utils.o \
	shared/util/textstreamreader.o \
	shared/util/textstreamwriter.o \
	shared/util/version.o \
	shared/util/wgt2allg.o \
	engine/globals.o \
	engine/ac/dynobj/cc_agsdynamicobject.o \
	engine/ac/dynobj/cc_audiochannel.o \
	engine/ac/dynobj/cc_audioclip.o \
	engine/ac/dynobj/cc_character.o \
	engine/ac/dynobj/cc_dialog.o \
	engine/ac/dynobj/cc_dynamicarray.o \
	engine/ac/dynobj/cc_dynamicobject.o \
	engine/ac/dynobj/cc_gui.o \
	engine/ac/dynobj/cc_guiobject.o \
	engine/ac/dynobj/cc_hotspot.o \
	engine/ac/dynobj/cc_inventory.o \
	engine/ac/dynobj/cc_object.o \
	engine/ac/dynobj/cc_region.o \
	engine/ac/dynobj/cc_serializer.o \
	engine/ac/dynobj/managedobjectpool.o \
	engine/ac/dynobj/scriptcamera.o \
	engine/ac/dynobj/scriptdatetime.o \
	engine/ac/dynobj/scriptdialogoptionsrendering.o \
	engine/ac/dynobj/scriptdict.o \
	engine/ac/dynobj/scriptdrawingsurface.o \
	engine/ac/dynobj/scriptdynamicsprite.o \
	engine/ac/dynobj/scriptfile.o \
	engine/ac/dynobj/scriptoverlay.o \
	engine/ac/dynobj/scriptstring.o \
	engine/ac/dynobj/scriptuserobject.o \
	engine/ac/dynobj/scriptviewframe.o \
	engine/ac/dynobj/scriptviewport.o \
	engine/ac/dynobj/scriptset.o \
	engine/ac/statobj/agsstaticobject.o \
	engine/ac/statobj/staticarray.o \
	engine/ac/audiochannel.o \
	engine/ac/audioclip.o \
	engine/ac/button.o \
	engine/ac/cdaudio.o \
	engine/ac/character.o \
	engine/ac/characterextras.o \
	engine/ac/characterinfo_engine.o \
	engine/ac/datetime.o \
	engine/ac/dialog.o \
	engine/ac/dialogoptionsrendering.o \
	engine/ac/display.o \
	engine/ac/draw.o \
	engine/ac/drawingsurface.o \
	engine/ac/draw_software.o \
	engine/ac/dynamicsprite.o \
	engine/ac/event.o \
	engine/ac/file.o \
	engine/ac/game.o \
	engine/ac/gamesetup.o \
	engine/ac/gamestate.o \
	engine/ac/global_api.o \
	engine/ac/global_audio.o \
	engine/ac/global_button.o \
	engine/ac/global_character.o \
	engine/ac/global_datetime.o \
	engine/ac/global_debug.o \
	engine/ac/global_dialog.o \
	engine/ac/global_display.o \
	engine/ac/global_drawingsurface.o \
	engine/ac/global_dynamicsprite.o \
	engine/ac/global_file.o \
	engine/ac/global_game.o \
	engine/ac/global_gui.o \
	engine/ac/global_hotspot.o \
	engine/ac/global_inventoryitem.o \
	engine/ac/global_invwindow.o \
	engine/ac/global_label.o \
	engine/ac/global_listbox.o \
	engine/ac/global_mouse.o \
	engine/ac/global_object.o \
	engine/ac/global_overlay.o \
	engine/ac/global_palette.o \
	engine/ac/global_parser.o \
	engine/ac/global_record.o \
	engine/ac/global_region.o \
	engine/ac/global_room.o \
	engine/ac/global_screen.o \
	engine/ac/global_slider.o \
	engine/ac/global_string.o \
	engine/ac/global_textbox.o \
	engine/ac/global_timer.o \
	engine/ac/global_translation.o \
	engine/ac/global_video.o \
	engine/ac/global_viewframe.o \
	engine/ac/global_viewport.o \
	engine/ac/global_walkablearea.o \
	engine/ac/global_walkbehind.o \
	engine/ac/gui.o \
	engine/ac/guicontrol.o \
	engine/ac/guiinv.o \
	engine/ac/hotspot.o \
	engine/ac/interfacebutton.o \
	engine/ac/interfaceelement.o \
	engine/ac/inventoryitem.o \
	engine/ac/invwindow.o \
	engine/ac/keycode.o \
	engine/ac/label.o \
	engine/ac/listbox.o \
	engine/ac/math.o \
	engine/ac/mouse.o \
	engine/ac/movelist.o \
	engine/ac/object.o \
	engine/ac/overlay.o \
	engine/ac/parser.o \
	engine/ac/properties.o \
	engine/ac/route_finder_impl.o \
	engine/ac/route_finder_impl_legacy.o \
	engine/ac/scriptcontainers.o \
	engine/ac/region.o \
	engine/ac/richgamemedia.o \
	engine/ac/room.o \
	engine/ac/roomobject.o \
	engine/ac/roomstatus.o \
	engine/ac/route_finder.o \
	engine/ac/route_finder_jps.o \
	engine/ac/screen.o \
	engine/ac/screenoverlay.o \
	engine/ac/slider.o \
	engine/ac/speech.o \
	engine/ac/sprite.o \
	engine/ac/spritecache_engine.o \
	engine/ac/string.o \
	engine/ac/sys_events.o \
	engine/ac/system.o \
	engine/ac/textbox.o \
	engine/ac/timer.o \
	engine/ac/translation.o \
	engine/ac/tree_map.o \
	engine/ac/viewframe.o \
	engine/ac/viewport_script.o \
	engine/ac/walkablearea.o \
	engine/ac/walkbehind.o \
	engine/debugging/consoleoutputtarget.o \
	engine/debugging/debug.o \
	engine/debugging/filebasedagsdebugger.o \
	engine/debugging/logfile.o \
	engine/debugging/messagebuffer.o \
	engine/device/mousew32.o \
	engine/font/fonts_engine.o \
	engine/game/game_init.o \
	engine/game/savegame.o \
	engine/game/savegame_components.o \
	engine/game/viewport.o \
	engine/gfx/ali3dscummvm.o \
	engine/gfx/blender.o \
	engine/gfx/color_engine.o \
	engine/gfx/gfx_util.o \
	engine/gfx/gfxdriverbase.o \
	engine/gfx/gfxdriverfactory.o \
	engine/gfx/gfxfilter_aad3d.o \
	engine/gfx/gfxfilter_allegro.o \
	engine/gfx/gfxfilter_d3d.o \
	engine/gfx/gfxfilter_hqx.o \
	engine/gfx/gfxfilter_scaling.o \
	engine/gui/animatingguibutton.o \
	engine/gui/cscidialog.o \
	engine/gui/guidialog.o \
	engine/gui/gui_engine.o \
	engine/gui/mylabel.o \
	engine/gui/mylistbox.o \
	engine/gui/mypushbutton.o \
	engine/gui/mytextbox.o \
	engine/gui/newcontrol.o \
	engine/main/config.o \
	engine/main/engine.o \
	engine/main/engine_setup.o \
	engine/main/game_file.o \
	engine/main/game_run.o \
	engine/main/game_start.o \
	engine/main/graphics_mode.o \
	engine/main/quit.o \
	engine/main/update.o \
	engine/media/audio/ambientsound.o \
	engine/media/audio/audio.o \
	engine/media/audio/clip_mymidi.o \
	engine/media/audio/queuedaudioitem.o \
	engine/media/audio/sound.o \
	engine/media/audio/soundcache.o \
	engine/media/audio/soundclip.o \
	engine/media/video/video.o \
	engine/platform/base/agsplatformdriver.o \
	engine/platform/linux/acpllnx.o \
	engine/script/cc_instance.o \
	engine/script/executingscript.o \
	engine/script/exports.o \
	engine/script/runtimescriptvalue.o \
	engine/script/script.o \
	engine/script/script_api.o \
	engine/script/script_engine.o \
	engine/script/script_runtime.o \
	engine/script/systemimports.o \
	plugins/agsplugin.o \
	plugins/global_plugin.o \
	plugins/plugin_base.o \
	plugins/pluginobjectreader.o \
	plugins/ags_blend/ags_blend.o \
	plugins/ags_creditz/ags_creditz.o \
	plugins/ags_creditz/ags_creditz1.o \
	plugins/ags_creditz/ags_creditz2.o \
	plugins/ags_creditz/drawing.o \
	plugins/ags_flashlight/ags_flashlight.o \
	plugins/ags_galaxy_steam/ags_blackwell_steam.o \
	plugins/ags_galaxy_steam/ags_galaxy_steam.o \
	plugins/ags_pal_render/ags_pal_render.o \
	plugins/ags_pal_render/raycast.o \
	plugins/ags_parallax/ags_parallax.o \
	plugins/ags_snow_rain/ags_snow_rain.o \
	plugins/ags_snow_rain/weather.o \
	plugins/ags_sprite_font/ags_sprite_font.o \
	plugins/ags_sprite_font/character_entry.o \
	plugins/ags_sprite_font/color.o \
	plugins/ags_sprite_font/sprite_font.o \
	plugins/ags_sprite_font/sprite_font_renderer.o \
	plugins/ags_sprite_font/variable_width_font.o \
	plugins/ags_sprite_font/variable_width_sprite_font.o \
	plugins/ags_tcp_ip/ags_tcp_ip.o

ifdef ENABLE_AGS_TESTS
MODULE_OBJS += \
	tests/test_all.o \
	tests/test_file.o \
	tests/test_gfx.o \
	tests/test_inifile.o \
	tests/test_math.o \
	tests/test_memory.o \
	tests/test_sprintf.o \
	tests/test_string.o \
	tests/test_version.o
endif

# This module can be built as a plugin
ifeq ($(ENABLE_AGS), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
