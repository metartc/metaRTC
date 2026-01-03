yang_main_os=7
yang_main_bit=64
yang_main_pic=0

cd libyangutil8
./cmake_build.sh ${yang_main_os} ${yang_main_bit} ${yang_main_pic}
cd ../libmetartccore8
./cmake_build.sh ${yang_main_os} ${yang_main_bit} ${yang_main_pic}
cd ../libyangwhip8
./cmake_build.sh ${yang_main_os} ${yang_main_bit} ${yang_main_pic}
cd ../libmetartc8
./cmake_build.sh ${yang_main_os} ${yang_main_bit} ${yang_main_pic}

