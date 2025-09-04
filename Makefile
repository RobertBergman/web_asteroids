CC = gcc
EMCC = emcc
CFLAGS = -std=c23 -Wall -Wextra -O2
INCLUDES = -I./src
LDFLAGS = -lraylib -lm -lpthread -ldl

PLATFORM ?= PLATFORM_DESKTOP

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/game.c \
          $(SRC_DIR)/entities.c \
          $(SRC_DIR)/input.c \
          $(SRC_DIR)/audio.c \
          $(SRC_DIR)/utils.c

OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE = $(BIN_DIR)/asteroids

ifeq ($(PLATFORM),PLATFORM_WEB)
    CC = $(EMCC)
    EXECUTABLE = $(BIN_DIR)/asteroids.html
    CFLAGS += -DPLATFORM_WEB
    LDFLAGS = -s USE_GLFW=3 -s ASYNCIFY \
              -s TOTAL_MEMORY=67108864 \
              -s FORCE_FILESYSTEM=1 \
              --shell-file shell.html \
              --preload-file assets
    RAYLIB_PATH ?= ./emsdk/raylib/src
    INCLUDES += -I$(RAYLIB_PATH)
    LDFLAGS += $(RAYLIB_PATH)/libraylib.web.a
endif

all: directories $(EXECUTABLE)

directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
ifeq ($(PLATFORM),PLATFORM_WEB)
	@echo "Build complete! Open $(EXECUTABLE) in a web browser"
else
	@echo "Build complete! Run with: ./$(EXECUTABLE)"
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: all
ifeq ($(PLATFORM),PLATFORM_WEB)
	python3 -m http.server 8000 --directory $(BIN_DIR)
else
	./$(EXECUTABLE)
endif

web:
	$(MAKE) PLATFORM=PLATFORM_WEB

desktop:
	$(MAKE) PLATFORM=PLATFORM_DESKTOP

.PHONY: all clean run web desktop directories