rm -rf build
mkdir build
cd build
ARCH=ios64.cmake
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../$ARCH  ..
make
if [ ! -d "../../bin/lib_debug" ] ; then
mkdir -p ../../bin/lib_debug
fi
cp ./libmetartccore7.a ../../bin/lib_debug/

