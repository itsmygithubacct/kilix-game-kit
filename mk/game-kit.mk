# Include this file from a game Makefile after adding kilix-game-kit as a
# recursive submodule. Consumers link one archive and keep their own targets.

KILIX_GAME_KIT_ROOT ?= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
KILIX_GAME_KIT_BUILD_DIR ?= $(KILIX_GAME_KIT_ROOT)/build
KILIX_GAME_KIT_LIB := $(KILIX_GAME_KIT_BUILD_DIR)/libkilix-game-kit.a

KILIX_GAME_KIT_CPPFLAGS := \
	-I$(KILIX_GAME_KIT_ROOT)/include \
	-I$(KILIX_GAME_KIT_ROOT)/third_party/kitty-terminal-session/include \
	-I$(KILIX_GAME_KIT_ROOT)/third_party/kitty-terminal-session/third_party/kitty-framebuffer/include \
	-I$(KILIX_GAME_KIT_ROOT)/third_party/kitty-terminal-session/third_party/kitty-input/include \
	-I$(KILIX_GAME_KIT_ROOT)/third_party/kitty-terminal-session/third_party/kitty-input/third_party/kitty_keyboard/include \
	-I$(KILIX_GAME_KIT_ROOT)/third_party/soft-raster/include \
	-I$(KILIX_GAME_KIT_ROOT)/third_party/pcm-mixer/include \
	-I$(KILIX_GAME_KIT_ROOT)/third_party/kilix-state/include
KILIX_GAME_KIT_LDLIBS := -lz -lpthread -lm

$(KILIX_GAME_KIT_LIB):
	$(MAKE) -C $(KILIX_GAME_KIT_ROOT) \
		BUILD_DIR=$(KILIX_GAME_KIT_BUILD_DIR) $(KILIX_GAME_KIT_LIB)
