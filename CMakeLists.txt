cmake_minimum_required (VERSION 2.6) 
project (hellov8) 

add_executable (helloDemo main.cpp) 

link_directories (/home/dev/Downloads/v8/out/x64.release/obj.target/tools/gyp)

include_directories (/home/dev/Downloads/v8/include) 
TARGET_LINK_LIBRARIES(helloDemo /home/dev/Downloads/v8/out/x64.release/obj.target/tools/gyp/libv8_base.x64.a /home/dev/Downloads/v8/out/x64.release/obj.target/tools/gyp/libv8_snapshot.a pthread) 