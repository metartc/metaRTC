关于rk1109运行说明
部分rv1109 sdk里面的头文件和编译使用库没有上传，所以运行时请按如下说明
1. 把你开发板sdk里的 rv1109_sd/external/rkmedia/include 目录下的头文件拷贝到 metaRTC/metaipc_rk1109/lib/include 目录下
2. 把RV1109开发板 sdk里面编译得到的库文件 拷贝到 metaRTC/metaipc_rk1109/lib目录下
3. 所需的sdk库文件参见metaRTC/metaipc_rk1109/CmakeLists.txt 文件