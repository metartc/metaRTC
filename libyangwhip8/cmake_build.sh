#!/bin/bash

source ../arch/yang_build.sh

rm -rf build
mkdir build
cd build

if [ $yang_pic -eq 1 ];then
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DYang_Moc=${yang_moc} -DCMAKE_TOOLCHAIN_FILE=../../arch/$ARCH  ..
else
cmake -DCMAKE_BUILD_TYPE=Release -DYang_Moc=${yang_moc} -DCMAKE_TOOLCHAIN_FILE=../../arch/$ARCH  ..
fi
make
if [ ! -d "../../bin/lib_debug" ] ; then
mkdir -p ../../bin/lib_debug
fi
cp ./libyangwhip8.a ../../bin/lib_debug/


