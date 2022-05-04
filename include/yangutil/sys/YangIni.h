//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YYangIni__
#define __YYangIni__
#include <yangutil/yangavinfotype.h>
#ifndef __ANDROID__
class YangIni {
public:
	YangIni();
	~YangIni();
public:
	void init(const char *p_filename);
	int32_t IniReadValue(const char *section, const char *key, char *val);
	int32_t readStringValue(const char *section, const char *key, char *val,
			const char *p_defaultStr);
	int32_t readIntValue(const char *section, const char *key, int32_t p_defaultInt);
	void initVideo(YangVideoInfo *video);
	void initAudio(YangAudioInfo *audio);
	void initSys(YangSysInfo *sys);
	void initEnc(YangVideoEncInfo *enc);
        void initRtc(YangRtcInfo *rtc);

protected:

private:
	char *m_file;
	int32_t readStringValue1(const char *section, const char *key, char *val);

};
#endif
#endif
