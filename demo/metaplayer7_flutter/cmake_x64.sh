
cd metaplayer7_flutter
flutter build linux --debug

cd ..
rm -rf ../../bin/app_debug/flutter
cp -r ./metaplayer7_flutter/build/linux/x64/debug/bundle  ../../bin/app_debug/
mv ../../bin/app_debug/bundle ../../bin/app_debug/flutter

rm -rf build
mkdir build
cd build
#cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
cmake -DCMAKE_BUILD_TYPE=Release ..
make

cd ..

if [ ! -d "../../bin/app_debug/flutter/lib" ] ; then
mkdir -p ../../bin/app_debug/flutter/lib
fi

cp ./build/libmetaplayer7.so ../../bin/app_debug/flutter/lib/
cp ./yang_config.ini ../../bin/app_debug/flutter/

