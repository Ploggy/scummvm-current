MODULE := engines/fullpipe

MODULE_OBJS = \
	behavior.o \
	detection.o \
	floaters.o \
	fullpipe.o \
	gameloader.o \
	gfx.o \
	init.o \
	input.o \
	interaction.o \
	inventory.o \
	lift.o \
	messagehandlers.o \
	messages.o \
	modal.o \
	motion.o \
	ngiarchive.o \
	scene.o \
	scenes.o \
	sound.o \
	stateloader.o \
	statics.o \
	utils.o \
	scenes/sceneIntro.o \
	scenes/scene01.o \
	scenes/scene02.o

# This module can be built as a plugin
ifeq ($(ENABLE_FULLPIPE), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
