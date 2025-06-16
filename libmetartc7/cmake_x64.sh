rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

if [ ! -d "../../bin/lib_debug" ] ; then
mkdir -p ../../bin/lib_debug
fi
cp ./libmetartc7.a ../../bin/lib_debug/

