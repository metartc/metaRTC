rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
if [ ! -d "../../bin/app_debug" ] ; then
mkdir -p ../../bin/app_debug
fi
cp metaipc5 ../../bin/app_debug

