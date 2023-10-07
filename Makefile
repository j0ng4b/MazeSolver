#-------------------------------------------------------------------------------
PLATFORM ?= PLATFORM_DESKTOP


#-------------------------------------------------------------------------------
GAME_NAME ?= Game
GAME_VERSION = 0.0.1

GAME_BUILD_MODE ?= RELEASE
GAME_USE_WAYLAND ?= FALSE


#-------------------------------------------------------------------------------
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	ifeq ($(OS),Windows_NT)
		PLATFORM_OS = WINDOWS
		EXTERNAL_LIB_PLATFORM = windows
	else
		UNAME = $(shell uname -s)

		ifeq ($(UNAME),Linux)
			PLATFORM_OS = LINUX
			EXTERNAL_LIB_PLATFORM = linux

			ifneq ($(WAYLAND_DISPLAY),)
				GAME_USE_WAYLAND = TRUE
			endif
		else ifeq ($(UNAME),Darwin)
			PLATFORM_OS = OSX
			EXTERNAL_LIB_PLATFORM = macos
		endif
	endif
endif


#-------------------------------------------------------------------------------
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	GRAPHICS ?= GRAPHICS_API_OPENGL_33
else ifeq ($(PLATFORM),PLATFORM_ANDROID)
	GRAPHICS = GRAPHICS_API_OPENGL_ES2
endif


#-------------------------------------------------------------------------------
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11
CPPFLAGS = -D$(PLATFORM) -D$(GRAPHICS) -D_GNU_SOURCE -MMD -MP


ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	ifeq ($(PLATFORM_OS),OSX)
		CC = clang
	endif
endif

ifeq ($(GAME_BUILD_MODE), DEBUG)
	CFLAGS += -g
	CPPFLAGS += -DDEBUG
else ifeq ($(GAME_BUILD_MODE), RELEASE)
	CFLAGS += -s -O2
	CPPFLAGS += -DNDEBUG
endif

ifeq ($(GAME_USE_WAYLAND),TRUE)
	CPPFLAGS += -D_GLFW_WAYLAND
endif


#-------------------------------------------------------------------------------
INCLUDE_PATHS = include
CPPFLAGS += $(foreach path,$(INCLUDE_PATHS),-I$(path))


#-------------------------------------------------------------------------------
LDLIBS = -lm
LDFLAGS = -Wl,--no-undefined

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	ifeq ($(PLATFORM_OS),WINDOWS)
		LDLIBS += -static-libgcc -lopengl32 -lgdi32 -lwinmm
	else ifeq ($(PLATFORM_OS),LINUX)
		LDLIBS += -lGL -lpthread -ldl -lrt

		ifneq ($(GAME_USE_WAYLAND),TRUE)
			LDLIBS += -lX11
		endif
	else ifeq ($(PLATFORM_OS),OSX)
		LDLIBS += -framework OpenGL -framework Cocoa -framework IOKit \
				  -framework CoreAudio -framework CoreVideo
	endif
endif


ifeq ($(GAME_USE_WAYLAND),TRUE)
	LDLIBS += -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon
endif


#-------------------------------------------------------------------------------
EXTERNAL_LIBS = raylib

CPPFLAGS += $(foreach path,$(EXTERNAL_LIBS),-Ilib/$(path)/include)

LDFLAGS += $(foreach path,$(EXTERNAL_LIBS),-Llib/$(path)/lib/$(EXTERNAL_LIB_PLATFORM))
LDLIBS := $(foreach path,$(EXTERNAL_LIBS),-l$(path)) $(LDLIBS)


#-------------------------------------------------------------------------------
ifeq ($(GAME_USE_WAYLAND),TRUE)
	WL_PROTOCOLS_DIR = $(shell pkg-config wayland-protocols --variable=pkgdatadir)
	WL_PROTOCOLS = stable/xdg-shell/xdg-shell.xml \
				   unstable/xdg-decoration/xdg-decoration-unstable-v1.xml \
				   stable/viewporter/viewporter.xml \
				   unstable/relative-pointer/relative-pointer-unstable-v1.xml \
				   unstable/pointer-constraints/pointer-constraints-unstable-v1.xml \
				   unstable/idle-inhibit/idle-inhibit-unstable-v1.xml

	# Add protocols path to protocols list
	WL_PROTOCOLS := $(foreach protocol,$(WL_PROTOCOLS),$(WL_PROTOCOLS_DIR)/$(protocol))

	WL_CLIENT_DIR = $(shell pkg-config wayland-client --variable=pkgdatadir)
	WL_CLIENT = $(WL_CLIENT_DIR)/wayland.xml

	WL_HEADERS = wayland-client-protocol \
				 wayland-xdg-shell-client-protocol \
				 wayland-xdg-decoration-client-protocol \
				 wayland-viewporter-client-protocol \
				 wayland-relative-pointer-unstable-v1-client-protocol \
				 wayland-pointer-constraints-unstable-v1-client-protocol \
				 wayland-idle-inhibit-unstable-v1-client-protocol

	# Add output path to wayland headers and -code header
	WL_HEADERS := $(foreach header,$(WL_HEADERS), \
						include/$(header).h)

	WL_PAIRS = $(join $(WL_CLIENT) $(WL_PROTOCOLS), \
						$(foreach header,$(WL_HEADERS),-$(header)))

	# Function to generate headers
	wl_generate = \
		$(eval protocol=$1) \
		$(eval basename=$2) \
		$(shell mkdir -p include ; \
			wayland-scanner client-header $(protocol) $(basename $(basename)).h) \
		$(shell mkdir -p include ; \
			wayland-scanner private-code $(protocol) $(basename $(basename))-code.h)
endif

#-------------------------------------------------------------------------------
SOURCES += $(wildcard src/*.c)

OBJECTS = $(subst src/,build/,$(SOURCES:.c=.o))
DEPENDENCIES = $(OBJECTS:.o=.d)

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	ifeq ($(PLATFORM_OS),WINDOWS)
		GAME_NAME_EXT := .exe
	endif

	GAME_NAME_BUILD := $(GAME_NAME)$(GAME_NAME_EXT)
else ifeq ($(PLATFORM),PLATFORM_ANDROID)
	GAME_NAME_BUILD := $(GAME_NAME).apk
endif


#-------------------------------------------------------------------------------
mkdir = $(shell mkdir -p $1)
RM = rm -rf

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	ifeq ($(PLATFORM_OS),WINDOWS)
	
	endif
endif


#-------------------------------------------------------------------------------
.PHONY: all
all: $(GAME_NAME_BUILD)

-include $(DEPENDENCIES)

$(GAME_NAME_BUILD): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

$(OBJECTS): $(WL_HEADERS)

build/%.o: src/%.c
	$(call mkdir,$(@D))
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) -o $@

$(WL_HEADERS): $(WL_PROTOCOLS) $(WL_CLIENT)
	@echo "Generating Wayland headers..."
	$(foreach pair,$(WL_PAIRS), \
			$(let protocol basename,$(subst .xml-include/,.xml include/,$(pair)), \
					$(call wl_generate $(protocol),$(basename))))

.PHONY: clean
clean:
	$(RM) build
	$(RM) $(GAME_NAME_BUILD)
	$(RM) $(foreach header,$(WL_HEADERS),$(header) $(basename $(header))-code.h)

