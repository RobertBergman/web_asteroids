# CLAUDE.md - Asteroids Game Project

## Atoms (Core Concepts)

### Game Entities
- **Spaceship**: Player-controlled triangular vessel with inertia-based movement
- **Asteroids**: Destructible objects in three sizes (large, medium, small)
- **Bullets**: Projectiles fired by spaceship with limited lifetime
- **UFOs**: Enemy saucers (large and small variants) that traverse the screen and shoot at player
  - Large UFO: Slower, random shooting direction, 200 points
  - Small UFO: Faster, aims at player, 1000 points

### Physics System
- **Newtonian Movement**: Velocity persists until thrust or collision
- **Screen Wrapping**: Toroidal topology for all entities
- **Collision Detection**: Between all entity pairs

### Technical Stack
- **Language**: C23 (ISO/IEC 9899:2024)
- **Graphics Library**: Raylib (latest stable, 5.0+)
- **Compilation Target**: WebAssembly via Emscripten
- **Browser Support**: Chrome, Firefox, Safari, Edge

## Molecules (Components)

### Core Modules
- **main.c**: Entry point and game loop
- **game.c**: Core game logic, entity updates, collisions, scoring
- **entities.c**: Entity definitions and behaviors
- **input.c**: User input handling via Raylib
- **audio.c**: Sound effects management
- **utils.c**: Math and random generation helpers

### Data Structures
```c
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float rotation;
} Spaceship;
```

### Game States
- Menu/Start screen
- Playing
- Game Over
- Pause

## Organisms (Systems)

### Game Loop Architecture
1. Initialize Raylib window
2. Main loop: Update → Draw → Check exit condition
3. Cleanup resources

### Scoring System
- Large asteroids: 20 points
- Medium asteroids: 50 points
- Small asteroids: 100 points
- Large UFO: 200 points
- Small UFO: 1000 points
- High score tracking via browser local storage

### Level Progression
- Clear all asteroids to advance
- Increasing asteroid count and speed per level

## Build Instructions

### Prerequisites
```bash
# Install Emscripten SDK
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

# Download and build Raylib for Emscripten
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_WEB -B
```

### Compilation Command
```bash
emcc -std=c23 -o asteroids.html main.c game.c entities.c input.c audio.c utils.c \
     -s USE_GLFW=3 -s ASYNCIFY \
     -I/path/to/raylib/src -L/path/to/raylib/src -lraylib \
     --shell-file /path/to/raylib/src/shell.html \
     --preload-file assets@/assets
```

### Local Testing
```bash
# Serve the compiled files
python -m http.server 8000
# Open http://localhost:8000/asteroids.html
```

## Development Guidelines

### C23 Features to Use
- `nullptr` instead of NULL
- Improved enums with underlying type specification
- `[[nodiscard]]` attribute for critical functions
- Designated initializers for structs
- `bool` type from `<stdbool.h>`

### Performance Targets
- 60 FPS target frame rate
- Resolution: Scalable, default 800x600
- Optimize for low-end devices

### Code Quality
- No external dependencies beyond Raylib
- Clean separation of concerns across modules
- Proper resource management (init/cleanup)

## Testing Checklist
- [ ] Browser compatibility (Chrome, Firefox, Safari, Edge)
- [ ] Performance profiling with browser dev tools
- [ ] Input responsiveness
- [ ] Audio playback
- [ ] Screen wrapping behavior
- [ ] Collision detection accuracy
- [ ] Score persistence

## Deployment
- Build for production with optimizations
- Upload `asteroids.html`, `asteroids.js`, `asteroids.wasm` to static hosting
- Recommended platforms: GitHub Pages, Netlify, Vercel

## Future Enhancements
- Multiplayer support via WebSockets
- Touch controls for mobile browsers
- Online leaderboards
- Power-ups and weapon upgrades
- Additional enemy types

## Project Structure
```
web_asteroids/
├── src/
│   ├── main.c
│   ├── game.c
│   ├── entities.c
│   ├── input.c
│   ├── audio.c
│   └── utils.c
├── assets/
│   ├── sounds/
│   └── fonts/
├── Makefile
├── CLAUDE.md
└── README.md
```

## Quick Reference

### Controls
- **Thrust**: Up arrow / W
- **Rotate Left**: Left arrow / A
- **Rotate Right**: Right arrow / D
- **Fire**: Space
- **Hyperspace**: H (random teleport with risk)
- **Pause**: P / Escape

### Entity Behaviors
- Asteroids split when hit (large→medium→small→destroyed)
- Bullets have limited range/lifetime
- UFOs appear periodically:
  - Spawn from screen edges at random intervals
  - Large UFO: Moves horizontally, shoots randomly
  - Small UFO: Zigzag pattern, targets player position
  - Both types fire bullets at regular intervals
  - Disappear after crossing screen or when destroyed
  - Spawn frequency increases with score/level
- All entities wrap around screen edges (except UFOs which exit)

### Visual Style
- 2D vector graphics (line art)
- Monochrome or limited color palette
- Classic arcade aesthetic

## Notes
- This is a faithful recreation of the classic Asteroids arcade game
- Focus on simplicity and performance over complex features
- Browser-native execution without plugins required
- Educational project demonstrating C23 and WASM capabilities