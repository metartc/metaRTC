#!/bin/bash

NDK_ROOT=/home/yang/pro/ndk
#NDK_ROOT=/home/mlz/Android/ndk/android-ndk-r21e

API=29

build() {
    rm -rf build
    mkdir build
    cd build

    echo "NDK_ROOT: ${NDK_ROOT}"
    echo "ARCH: ${1}"

    cmake -G"Unix Makefiles" \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_TOOLCHAIN_FILE=$NDK_ROOT/build/cmake/android.toolchain.cmake \
    -DANDROID_NDK=$NDK_ROOT \
    -DANDROID_PLATFORM=android-${API} \
    -DCMAKE_BUILD_TYPE=Debug \
    -DANDROID_ABI=$1 \
    -DANDROID_STL=c++_static \
    -DCMAKE_CXX_STANDARD=11 \
    -DANDROID_NATIVE_API_LEVEL=${API} \
    ..

    make

    if [ ! -d "../../bin/lib_android/$1" ] ; then
        mkdir -p ../../bin/lib_android/$1
    fi
        cp ./libmetartccore6.a ../../bin/lib_android/$1/

    cd ..
}

build x86

build x86_64

build armeabi-v7a

build arm64-v8a
