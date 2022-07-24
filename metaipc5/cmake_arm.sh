rm -rf build
mkdir build
cd build
ARCH=aarch64.cmake
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../$ARCH  ..
make

