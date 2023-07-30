NDK_ROOT=/home/yang/pro/ndk
API=29
rm -rf build
mkdir build
cd build
#arm64-v8a armeabi x86_64
ARCH=arm64-v8a
cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$NDK_ROOT/build/cmake/android.toolchain.cmake -DANDROID_NDK=$NDK_ROOT -DCMAKE_SYSTEM_NAME=Android -DANDROID_PLATFORM=android-${API} -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI=${ARCH} -DAndroid=ON -DANDROID_STL=c++_static -DCMAKE_CXX_STANDARD=11 -DANDROID_NATIVE_API_LEVEL=${API} ..
make
if [ ! -d "../../bin/lib_android/arm64-v8a" ] ; then
mkdir -p ../../bin/lib_android/arm64-v8a
fi
cp ./libmetartccore7.a ../../bin/lib_android/arm64-v8a/

cd ..
rm -rf build
mkdir build
cd build
ARCH=x86_64
cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$NDK_ROOT/build/cmake/android.toolchain.cmake -DANDROID_NDK=$NDK_ROOT -DCMAKE_SYSTEM_NAME=Android -DANDROID_PLATFORM=android-${API} -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI=${ARCH} -DAndroid=ON -DANDROID_STL=c++_static -DCMAKE_CXX_STANDARD=11 -DANDROID_NATIVE_API_LEVEL=${API} ..
make
if [ ! -d "../../bin/lib_android/x86_64" ] ; then
mkdir -p ../../bin/lib_android/x86_64
fi
cp ./libmetartccore7.a ../../bin/lib_android/x86_64/



