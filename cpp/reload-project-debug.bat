@echo off
if not exist cmake-build-debug\NUL mkdir cmake-build-debug
cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ../ 
cd ..