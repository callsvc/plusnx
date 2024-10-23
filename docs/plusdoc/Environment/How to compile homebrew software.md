- https://switchbrew.org/wiki/Setting_up_Development_Environment
- We will use the RetroArch project as an example
### Dev Notes
	- Every Switch application must contain its own internal SDK, with no external dependencies, for this we will use the user-space library called libnx

#### Install Docker
#### Run Debian
#### Install pacman-like devkitPro package manager
- https://github.com/devkitPro/pacman/releases
#### Install the required packages
- dkp-pacman -Sy devkit-env devkitA64 libnx switch-tools switch-mesa switch-zlib switch-bzip2 switch-liblzma switch-freetype switch-libpng switch-libvpx switch-ffmpeg

###  Select and clone the desired core to compile
- Clone https://github.com/libretro/libretro-super
- For (snes9x2010): ./libretro-fetch.sh snes9x2010 fceumm