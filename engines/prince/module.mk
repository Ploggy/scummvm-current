MODULE := engines/prince

MODULE_OBJS = \
	animation.o \
	archive.o \
	cursor.o \
	debugger.o \
	decompress.o \
	detection.o \
	flags.o \
	font.o \
	graphics.o \
	hero.o \
	mhwanh.o \
	mob.o \
	object.o \
	prince.o \
	pscr.o \
	saveload.o \
	script.o \
	sound.o \
	variatxt.o \
	videoplayer.o

# This module can be built as a plugin
ifeq ($(ENABLE_PRINCE), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
