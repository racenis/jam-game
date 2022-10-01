# Dzīvības Partikula
This game was made for the "Alternative Engine Game Jam"

Source code for the engine is on the [github](https://github.com/racenis/tram-sdk).

Assets are self-made, if not [mentioned otherwise](assetsources.md).

If you don't want to compile it yourself, then binaries for windows are on the [itch.io](https://racenis.itch.io/dziiviibas-partikula).

## Compiling
### Compiler
The engine used needs a compiler that supports C++20. The windows binaries were compiled with MinGW-W64 12.1.0, so that obviously works. GCC 11 will probably also work, but it's probably a good idea to update it anyway. The clang compiler should work, but have not tested it, and the microsoft visual c++ compiler will probably catch on fire when you try to use it. 
### Acquiring the libraries
First you'll need to get the [GLFW](https://www.glfw.org/), [Bullet physics](https://github.com/bulletphysics/bullet3/releases) and [OpenAL-Soft](https://github.com/kcat/openal-soft) libraries. If you don't want to wait an hour for them to compile and you're using Linux, then your package manager should already have these and you can just install them. Also if you're compiling the libraries yourself, when generating makefiles for Bullet physics with cmake, you need to set it to use single (32-bit) floating point precision, for other libraries use defaults.
After that you'll need to get the [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h), [stb_vorbis.c](https://github.com/nothings/stb/blob/master/stb_vorbis.c) and [glm](https://github.com/g-truc/glm) libraries. Also you'll a loader for OpenGL, so you can use [glad](https://glad.dav1d.de/) to generate them, go to their website and set ``gl`` to 'Version 4.0' and ``Profile`` to 'Core'.
### Actually compiling
Acquire the libraries and follow their installation instructions.
Then you can compile the game itself. If you use Codelite IDE, then there's a project file for that, otherwise just compile all of the .cpp files. When linking on windows use the parameters ``-lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lglfw3 -lOpenAL32.dll -lglfw3 -lgdi32 -lopengl32`` (at least that's the order that works for me). For Linux you'll need use ``-lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lglfw3 -lopenal -lglfw3`` or something similar. If it doens't work then just experiment with it, but make sure that the Bullet physics (the first 4) flags are in the same order, otherwise it will not work.

## Editor
If you want to try using the level editor, then it comes with the windows binaries. To compile, it you'll need the wxWidgets library, and it takes a very long time to compile it. It should also be available as a Linux package. When compiling it, you'll only need wxWidgets, glad and stb_image libraries.