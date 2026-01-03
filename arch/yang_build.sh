#0:x64 1:mips 2:arm 3:risc-v 4:loongson 5:wrt 6:mac
yang_moc=2
yang_bit=64
yang_pic=1

if [ $1 ];then
yang_moc=$1
fi

if [ $2 ];then
yang_bit=$2
fi

if [ $3 ];then
yang_pic=$3
fi

ARCH=aarch64.cmake
if [ $yang_moc -eq 0 ];then
	if [ $yang_bit -eq 32 ];then
	ARCH=x86_32.cmake
	else
	ARCH=x86_64.cmake
	fi
fi

if [ $yang_moc -eq 1 ];then
	if [ $yang_bit -eq 32 ];then
	ARCH=mips32.cmake
	else
	ARCH=mips64.cmake
	fi
fi

if [ $yang_moc -eq 2 ];then
	if [ $yang_bit -eq 32 ];then
	ARCH=arm32.cmake
	else
	ARCH=aarch64.cmake
	fi
fi

if [ $yang_moc -eq 5 ];then
	if [ $yang_bit -eq 32 ];then
	ARCH=wrt32.cmake
	else
	ARCH=wrt64.cmake
	fi
fi

if [ $yang_moc -eq 6 ];then
	
	ARCH=mac_arm64.cmake

fi

if [ $yang_moc -eq 7 ];then
	
	ARCH=ios64.cmake

fi
