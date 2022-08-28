NDK_ROOT=/home/yang/pro/ndk
rm -rf build
mkdir build
cd build
#arm64-v8a armeabi x86_64
ARCH=x86_64
API=29
cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$NDK_ROOT/build/cmake/android.toolchain.cmake -DANDROID_NDK=$NDK_ROOT -DCMAKE_SYSTEM_NAME=Android -DANDROID_PLATFORM=android-${API} -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI=${ARCH} -DAndroid=ON -DANDROID_STL=c++_static -DCMAKE_CXX_STANDARD=11 -DANDROID_NATIVE_API_LEVEL=${API} ..
make
if [ ! -d "../../bin/lib_android" ] ; then
mkdir -p ../../bin/lib_android
fi
cp ./libmetartccore5.a ../../bin/lib_android/

