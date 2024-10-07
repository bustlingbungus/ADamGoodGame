A Dam Good Game is a fast paced, arcade inspired action roguelike. 
Collect resources to block the river, and construct a base to survive as long as possible. 
Kill enemies by throwing building materials at them. Try to beat your own high scores!

## Build Instructions

To build the game from source code, You need the SDL2, SDL2_Image, SDL2_ttf, and SDL2_mixer libraries installed
You will also need CMake, and conan C++ package manager

### If all of these are present, execute the following build steps:

First run ```conan install . --build=missing```. Since there is a bug in the recipe we need to install our packages seperately. 
Edit conanfile.py comment out the sdl_ttf requirement and uncomment the sdl_image requirement. Repeat the previous step.
Then, do the same after commenting out the sdl_image requirement and uncommenting the sdl_mixer requirement

Then, run these commands (on VScode, using the included build tasks by pressing ```ctrl+shift+b``` should also work):
```
cmake --preset conan-default
cmake --build --preset conan-release
cd build\Release
./ADamGoodGame.exe
```


## Download

If you are having trouble building the game, or would like to support the creators, check out the game on ```Itch.io```, here:
[https://bustling-bunguss.itch.io/a-dam-good-game](https://bustling-bunguss.itch.io/a-dam-good-game)
