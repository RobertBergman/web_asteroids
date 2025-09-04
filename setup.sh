#!/bin/bash

set -e

echo "==================================="
echo "Asteroids Game Setup Script"
echo "==================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install Emscripten
install_emscripten() {
    echo -e "${YELLOW}Installing Emscripten SDK...${NC}"
    
    if [ -d "emsdk" ]; then
        echo "Emscripten SDK directory already exists, updating..."
        cd emsdk
        git pull
    else
        git clone https://github.com/emscripten-core/emsdk.git
        cd emsdk
    fi
    
    ./emsdk install latest
    ./emsdk activate latest
    source ./emsdk_env.sh
    
    cd "$SCRIPT_DIR"
    echo -e "${GREEN}Emscripten installed successfully!${NC}"
}

# Function to install Raylib
install_raylib() {
    echo -e "${YELLOW}Installing Raylib...${NC}"
    
    if [ -d "raylib" ]; then
        echo "Raylib directory already exists, updating..."
        cd raylib
        git pull
    else
        git clone https://github.com/raysan5/raylib.git
        cd raylib
    fi
    
    cd src
    
    # Build for desktop first
    echo "Building Raylib for desktop..."
    make PLATFORM=PLATFORM_DESKTOP clean
    make PLATFORM=PLATFORM_DESKTOP -j$(nproc)
    
    # Build for web if Emscripten is available
    if [ -f "$SCRIPT_DIR/emsdk/emsdk_env.sh" ]; then
        source "$SCRIPT_DIR/emsdk/emsdk_env.sh"
        echo "Building Raylib for web..."
        make PLATFORM=PLATFORM_WEB clean
        make PLATFORM=PLATFORM_WEB -j$(nproc)
    fi
    
    cd "$SCRIPT_DIR"
    echo -e "${GREEN}Raylib installed successfully!${NC}"
}

# Function to build the game
build_game() {
    echo -e "${YELLOW}Building Asteroids game...${NC}"
    
    # Update Makefile paths
    cat > Makefile.local << 'EOF'
CC = gcc
EMCC = emcc
CFLAGS = -Wall -Wextra -O2
INCLUDES = -I./src
LDFLAGS = -lm -lpthread -ldl

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

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    RAYLIB_PATH = ./raylib/src
    INCLUDES += -I$(RAYLIB_PATH)
    LDFLAGS += -L$(RAYLIB_PATH) -lraylib -lGL
endif

ifeq ($(PLATFORM),PLATFORM_WEB)
    CC = $(EMCC)
    EXECUTABLE = $(BIN_DIR)/asteroids.html
    CFLAGS += -DPLATFORM_WEB
    LDFLAGS = -s USE_GLFW=3 -s ASYNCIFY \
              -s TOTAL_MEMORY=67108864 \
              -s FORCE_FILESYSTEM=1 \
              --shell-file shell.html \
              --preload-file assets
    RAYLIB_PATH = ./raylib/src
    INCLUDES += -I$(RAYLIB_PATH)
    LDFLAGS += $(RAYLIB_PATH)/libraylib.a
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

.PHONY: all clean run directories
EOF

    # Try desktop build
    echo "Building for desktop..."
    if make -f Makefile.local PLATFORM=PLATFORM_DESKTOP; then
        echo -e "${GREEN}Desktop build successful!${NC}"
    else
        echo -e "${RED}Desktop build failed!${NC}"
    fi
    
    # Try web build if Emscripten is available
    if [ -f "$SCRIPT_DIR/emsdk/emsdk_env.sh" ]; then
        source "$SCRIPT_DIR/emsdk/emsdk_env.sh"
        echo "Building for web..."
        if make -f Makefile.local PLATFORM=PLATFORM_WEB; then
            echo -e "${GREEN}Web build successful!${NC}"
        else
            echo -e "${RED}Web build failed!${NC}"
        fi
    fi
}

# Main setup flow
main() {
    echo "This script will set up the build environment for the Asteroids game."
    echo ""
    
    # Check for git
    if ! command_exists git; then
        echo -e "${RED}Error: git is not installed. Please install git first.${NC}"
        exit 1
    fi
    
    # Check for make
    if ! command_exists make; then
        echo -e "${RED}Error: make is not installed. Please install make first.${NC}"
        exit 1
    fi
    
    # Install dependencies
    read -p "Install Emscripten SDK for web build? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        install_emscripten
    fi
    
    read -p "Install/Update Raylib? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        install_raylib
    fi
    
    # Build the game
    read -p "Build the game now? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        build_game
    fi
    
    echo ""
    echo -e "${GREEN}==================================="
    echo "Setup complete!"
    echo "==================================="
    echo ""
    echo "To run the game:"
    echo "  Desktop: ./bin/asteroids"
    echo "  Web: make -f Makefile.local run PLATFORM=PLATFORM_WEB"
    echo -e "${NC}"
}

# Run main function
main