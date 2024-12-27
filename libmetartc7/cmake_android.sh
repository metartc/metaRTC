#!/usr/bin/env bash

NDK_ROOT=/media/gaojie/lvm2/Android/Sdk/ndk/28.0.12433566/
API=34

#arm64-v8a armeabi x86_64
ARCHS="arm64-v8a armeabi-v7a x86_64"

build() {
    ARCH=$1

    echo "Build for $ARCH"
    BUILD_DIR=build/$ARCH
    echo "Build dir is: $BUILD_DIR"

    #clean build dir
    # rm -rf $BUILD_DIR

    # configuration 
    cmake -B $BUILD_DIR \
        -DCMAKE_TOOLCHAIN_FILE=$NDK_ROOT/build/cmake/android.toolchain.cmake \
        -DCMAKE_SYSTEM_NAME=Android        \
        -DCMAKE_BUILD_TYPE=Release         \
        -DCMAKE_CXX_STANDARD=11            \
        -DANDROID_NDK=$NDK_ROOT            \
        -DANDROID_PLATFORM=android-${API}  \
        -DANDROID_ABI=${ARCH} -DAndroid=ON \
        -DANDROID_STL=c++_static           \
        -DANDROID_NATIVE_API_LEVEL=${API}  \
        -DYang_OS_ANDROID=ON            

    # build
    cmake --build $BUILD_DIR

}


for arch in $ARCHS
do
    build $arch
done 



