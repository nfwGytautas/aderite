## This directory contains third party libraries that aderite uses.


In order for aderite to find these libraries all includes must be in directory include/.

All compiled binaries must be in directory lib/{debug/release}/.

The provided dependencies.7z contains all the libraries needed for aderite to compile

## List of dependencies:
### Runtime:
- https://github.com/glfw/glfw  (Used to create windows)
- https://github.com/bkaradzic/bgfx (Used to provide the graphics API)
- https://github.com/NVIDIAGameWorks/PhysX (Physics engine)
- https://www.fmod.com/ (Audio API)
- https://github.com/mono/mono (Scripting)
- https://github.com/gabime/spdlog (Logging API)
- https://github.com/g-truc/glm (Math library)
- https://github.com/assimp/assimp (Mesh and object loading)
- https://github.com/nothings/stb (Image loading)
- https://github.com/skypjack/entt (ECS system)
- https://github.com/jbeder/yaml-cpp (Serialization)

### Editor:
- https://github.com/ocornut/imgui (Editor UI)
- https://github.com/Nelarius/imnodes (Graphs)
- https://github.com/samhocevar/portable-file-dialogs (File selection API)
