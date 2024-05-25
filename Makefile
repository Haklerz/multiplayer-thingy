# Compiler
CC := gcc

# Warnings
WARNINGS := -Wall -Wextra -Wno-missing-braces

# Compiler flags
CFLAGS := -I include -O1 -std=c99 -g $(WARNINGS)

# Linker flags
LDFLAGS_CLIENT := -L lib -lraylib -lopengl32 -lgdi32 -lwinmm # -lws2_32
LDFLAGS_SERVER := # -lws2_32

# Source files
SRCS_COMMON := \
src/events.c

SRCS_CLIENT := \
src/client.c \
src/game_server.c

SRCS_SERVER := \
src/server.c

# Object files
OBJS_COMMON := $(subst src, obj, $(SRCS_COMMON:.c=.o))
OBJS_CLIENT := $(subst src, obj, $(SRCS_CLIENT:.c=.o))
OBJS_SERVER := $(subst src, obj, $(SRCS_SERVER:.c=.o))

# Build Targets
TARGET_CLIENT := bin/client.exe
TARGET_SERVER := bin/server.exe

# ------------------------------------

# Default target
all: $(TARGET_CLIENT) $(TARGET_SERVER)

# Build client
client: $(TARGET_CLIENT)

# Build server
server: $(TARGET_SERVER)

# Build client executible
$(TARGET_CLIENT): $(OBJS_COMMON) $(OBJS_CLIENT) | bin
	@echo.
	@echo Linking executible: $@
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_CLIENT)

# Build server executible
$(TARGET_SERVER): $(OBJS_COMMON) $(OBJS_SERVER) | bin
	@echo.
	@echo Linking executible: $@
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_SERVER)

# Compile source files to object files
obj/%.o: src/%.c | obj
	@echo.
	@echo Compiling source file: $<
	$(CC) $(CFLAGS) -c -o $@ $<

# Header dependencies
src/client.c: \
include/events.h \
include/game_server.h

src/server.c: \
include/events.h

src/events.c: \
include/events.h

# Make build directories
bin obj:
	@echo.
	@echo Making build directory: $@
	mkdir $@

# --------------------------------------------

# Phony targets
.PHONY: clean very-clean run-server run-client

# Run client
run-client: $(TARGET_CLIENT)
	@.\$<

# Run server
run-server: $(TARGET_SERVER)
	@.\$<

# Clean the build directories
clean: obj bin
	@echo Cleaning build directories...
	@del /q /s $^

# Clean the project
very-clean: obj bin
	@echo Cleaning project...
	@rmdir /q /s $^
