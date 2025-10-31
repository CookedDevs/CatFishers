CatFishers is a multiplayer cat fishing game

### platforms
  [`windows`](#build-windows)
  [`linux`](#build-linux)
  [`android`](#build-android)

# Libraries
- raylib
- raymob
- [commandline](https://github.com/lionkor/commandline)
- enet6
- nlogmann/json

# Building
this project is build using `xmake`

<a id="build-windows"></a>
## windows
Run `xmake project -k vsxmake` to create visual studio files for the project.
<a id="build-linux"></a>
## Linux
Install dependencies
`sudo apt-get update && sudo apt-get install -y libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev`

`xmake` will build the project
`xmake run` to run project.


<a id="build-android"></a>
## Android
Install dependencies
`sudo apt-get update && sudo apt-get install -y libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev`
`sudo apt install -y openjdk-17-jdk`

and download android jdk and ndk
