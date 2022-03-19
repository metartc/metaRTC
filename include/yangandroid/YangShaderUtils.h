//
// Copyright (c) 2019-2022 yanggaofeng
//

#ifndef YUVPLAYER_SHADERUTILS_H
#define YUVPLAYER_SHADERUTILS_H

//#include "LogUtils.h"
#include <cstring>
#include <string>
#include <android/asset_manager.h>

class YangShaderUtils {
public:
    static std::string * openAssetsFile(AAssetManager *mgr, char *file_name);
};


#endif //YUVPLAYER_SHADERUTILS_H
