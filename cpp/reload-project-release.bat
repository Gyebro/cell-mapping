@echo off
if not exist cmake-build-release\NUL mkdir cmake-build-release
cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ../ 
cd ..