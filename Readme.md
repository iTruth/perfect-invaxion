# Perfect Invaxion
Apply a perfect invaxion save in your computer!

## How to use it?
1. download from release  
2. extract and double click pinvaxion.exe  

Notice that if you're using [invaxion-server-emulator](https://github.com/Invaxion-Server-Emulator/invaxion-server-emulator), you should register an account before using this project.  
That's all!

## Build from source
Assume that you're using [MinGW-w64](https://www.mingw-w64.org/)  
1. compile and install [json](https://github.com/nlohmann/json) and [spdlog](https://github.com/gabime/spdlog)
2. `mkdir build && cd build`
3. `cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release`
4. `make`
5. `make package`

