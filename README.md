# Web Asteroids

A faithful recreation of the classic Asteroids arcade game, built with C23 and compiled to WebAssembly for browser play.

## Features

- Classic vector-style graphics
- Player-controlled spaceship with inertia-based physics
- Destructible asteroids in three sizes
- Enemy UFOs (large and small variants)
- Progressive difficulty with increasing levels
- High score tracking via browser local storage
- Smooth 60 FPS gameplay

## Controls

- **Thrust**: Up arrow / W
- **Rotate Left**: Left arrow / A  
- **Rotate Right**: Right arrow / D
- **Fire**: Space
- **Hyperspace**: H (random teleport with risk)
- **Pause**: P / Escape

## Technical Details

- **Language**: C23 (ISO/IEC 9899:2024)
- **Graphics**: Raylib 5.0+
- **Target**: WebAssembly via Emscripten
- **Browser Support**: Chrome, Firefox, Safari, Edge

## Building from Source

### Prerequisites

1. Install Emscripten SDK:
```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

2. Build Raylib for Emscripten:
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_WEB -B
```

### Compilation

```bash
emcc -std=c23 -o asteroids.html main.c game.c entities.c input.c audio.c utils.c \
     -s USE_GLFW=3 -s ASYNCIFY \
     -I/path/to/raylib/src -L/path/to/raylib/src -lraylib \
     --shell-file /path/to/raylib/src/shell.html \
     --preload-file assets@/assets
```

### Local Testing

```bash
python -m http.server 8000
# Open http://localhost:8000/asteroids.html
```

## Game Mechanics

### Scoring
- Large asteroids: 20 points
- Medium asteroids: 50 points
- Small asteroids: 100 points
- Large UFO: 200 points
- Small UFO: 1000 points

### Entity Behaviors
- Asteroids split when hit (large→medium→small→destroyed)
- Bullets have limited range and lifetime
- UFOs appear periodically with increasing frequency
- All entities wrap around screen edges (except UFOs)

## Project Structure

```
web_asteroids/
├── src/
│   ├── main.c         # Entry point and game loop
│   ├── game.c         # Core game logic
│   ├── entities.c     # Entity definitions and behaviors
│   ├── input.c        # Input handling
│   ├── audio.c        # Sound effects
│   └── utils.c        # Math and utility functions
├── assets/
│   ├── sounds/
│   └── fonts/
├── Makefile
└── README.md
```

## Deployment

1. Build for production with optimizations
2. Upload `asteroids.html`, `asteroids.js`, `asteroids.wasm` to static hosting
3. Recommended platforms: GitHub Pages, Netlify, Vercel

## License

This project is a recreation of the classic Asteroids arcade game for educational purposes.

## Future Enhancements

- Multiplayer support via WebSockets
- Touch controls for mobile browsers
- Online leaderboards
- Power-ups and weapon upgrades
- Additional enemy types