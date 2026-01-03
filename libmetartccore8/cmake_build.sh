#!/bin/bash

source ../arch/yang_build.sh

rm -rf build
mkdir build
cd build



if [ $yang_pic -eq 1 ];then
cmake -DCMAKE_BUILD_TYPE=Release -DYang_Bit=${yang_bit} -DYang_Moc=${yang_moc}  -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_TOOLCHAIN_FILE=../../arch/$ARCH  ..
else
cmake -DCMAKE_BUILD_TYPE=Release -DYang_Bit=${yang_bit} -DYang_Moc=${yang_moc}  -DCMAKE_TOOLCHAIN_FILE=../../arch/$ARCH  ..
fi

make
if [ ! -d "../../bin/lib_debug" ] ; then
mkdir -p ../../bin/lib_debug
fi

cp ./libmetartccore8.a ../../bin/lib_debug/


