//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangandroid/YangShaderUtils.h>

std::string *YangShaderUtils::openAssetsFile(AAssetManager *mgr, char *file_name) {
    //打开asset文件夹
    AAssetDir *dir = AAssetManager_openDir(mgr, "");
    const char *file = NULL;
    std::string *result = new std::string;//调用后需要delete，否则内存泄漏

    while ((file = AAssetDir_getNextFileName(dir)) != NULL) {
        if (strcmp(file, file_name) == 0) {
            AAsset *asset = AAssetManager_open(mgr, file, AASSET_MODE_STREAMING);
            char buf[1024];
            int nb_read = 0;
            while ((nb_read = AAsset_read(asset, buf, 1024)) > 0) {
                result->append(buf, (unsigned long)nb_read);
            }
            AAsset_close(asset);
            break;
        }
    }
    AAssetDir_close(dir);
    return result;
}
